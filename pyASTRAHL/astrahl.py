import json
import tempfile
import os
import subprocess
import pandas as pd

from typing import Dict

from .api import Expression, Const, Wire

FILE_DIR = os.path.dirname(__file__)

class Astrahl(object):
    TOOL_PATH = os.path.join(FILE_DIR, "..", "src")
    TOOL_NAME = "main"
    SPLITS_KEYWORD = "splits"
    PROBABILITY_KEYWORD = "propagate"
    TOOL_INPUT_TMP_PATH = os.path.join(FILE_DIR, "..", "tmp")

    def __init__(self, fault_tree, confidence_functions=None):
        self.ft = fault_tree
        self.conf_funcs = {}
        if confidence_functions:  # sometimes we pass None when we don't need them
            for wire, func_expression in confidence_functions.items():
                assert isinstance(func_expression, Expression)
                pos_exp = func_expression
                neg_exp = Const(0)  # we don't use negative confidence here
                self.conf_funcs[wire] = (pos_exp, neg_exp)

            if not self._check_conf_funcs():
                raise ValueError()

        # create ordered list
        if fault_tree is not None:  # it is only None in batch execution mode (see batch_run_splits.py)
            self._wire_order = sorted(list(self.ft.wires), key=str)

        # create tmp directory
        os.makedirs(self.TOOL_INPUT_TMP_PATH, exist_ok=True)

    def _check_conf_funcs(self):
        for wire in self.ft.wires:
            if not (wire in self.conf_funcs):
                print("Need to provide confidence functions for wire {}".format(wire))
                return False
            if isinstance(self.conf_funcs[wire], tuple) and len(self.conf_funcs[wire]) == 2:
                # check that there are two expressions in the tuple
                pass
            else:
                print("Need to provide a positive or a tuple of positive and a negative confidence function for wire {}".format(wire))
                return False

        return True  # all checks passed

    # -------------------- Entry points -------------------

    def calculate_allowances(self, point: Dict, total_budget: float, algorithm_config: Dict, cleanup: bool = True):
        """
        point : a dict of {wire: value} that shows the current starting confidence of each wire
        total_budget : how many resources we want to invest
        algorithm_config: some hyper-parameters for the algorithm (default should be Dict = {"max_step": 10000, "p_init": 0.999, "lambda": 1.0}
        cleanup : remove the file afterwards?
        """
        splits_config, alg_config = self.create_splits_config(point, total_budget, algorithm_config)
        splits_file = self._write_config_to_file(splits_config, suffix="splits_cfg")
        alg_file = self._write_config_to_file(alg_config, suffix="alg_cfg")

        result = self._tool_trigger(self.__class__.SPLITS_KEYWORD, splits_file, alg_file)
        lastline = result.splitlines()[-1]
        # print(lastline)
        words = [key_val.split("=") for key_val in lastline.split()]  # convert the output (1=0.3 2=0.3 4=0.4 5=0.5) to [(1,0.3), (2,0.3), ...]
        wire_splits = {self._wire_order[int(key)]: float(value) for (key, value) in words}
        # print(wire_splits)

        # checks
        assert abs(sum(wire_splits.values()) - total_budget) < (10**-7), "There was a significant difference between Astrahl's suggested split and the Total Resource Budget.\nAllowances: {}\nSum Allowances: {}\nTotal Budget: {}\nDifference: {}".format(wire_splits, total_budget, sum(wire_splits.values()), abs(sum(wire_splits.values()) - total_budget) )  # assert that input and output match
        assert all([v >= 0 for v in wire_splits.values()]), "Astrahl calculated a negative allowance.\nAllowances: {}\nSplits Config: {}\nAlgorithm Config: {}".format(wire_splits, splits_file, alg_file)  # assert that all values are positive

        if cleanup:
            os.remove(splits_file)
            os.remove(alg_file)

        return wire_splits

    def calculate_probability(self, point: dict, cleanup: bool = True):
        """Calculate the confidence in a system at a given point."""
        probability_config = self.create_probability_config(point)
        prob_file = self._write_config_to_file(probability_config, suffix="prob")
        result = self._tool_trigger(self.__class__.PROBABILITY_KEYWORD, prob_file)
        if cleanup:
            os.remove(prob_file)
        return float(result.splitlines()[0])

    def batch_calculate_splits(self, points_df, total_budget: float,  algorithm_config: Dict):
        """
        Python is limited by the number of files we can open at once.
        Therefore, for bulk tests, we want the file system (not Python) to create individual configuration files.
        jq and split help us in achieving this.

        points_df: has points as rows and wires as columns
        total_budget: how much we want to spend
        algorithm_config: (same for all) some hyper-parameters for the algorithm (default should be Dict = {"max_step": 10000, "p_init": 0.999, "lambda": 1.0}
        """
        splits_configs = []
        alg_configs = []
        # iterate over the points_df and create one splits_config and one alogrithm_config for each row
        # note that while the algorithm_config input is the same for all, the actual information written to file
        # is specific for each splits_config (as it requires the point-configuration)
        for rowidx, row in points_df.iterrows():
            splits_config, alg_config = self.create_splits_config(row, total_budget, algorithm_config)
            splits_configs.append(splits_config)
            alg_configs.append(alg_config)

        # Create two temporary files (one for splits, one for algorithm configs) and write the respective values in each
        with tempfile.TemporaryDirectory(dir="./tmp") as tmpdir:
            with tempfile.NamedTemporaryFile(mode="w", dir=tmpdir, prefix="batch_splits_", suffix=".json") as splits_config_file:
                # write all splits configs into a config file
                splits_filename = splits_config_file.name
                splits_config_file.write(json.dumps(splits_configs, indent=4))

                # write all algorithm configs into a config file
                with tempfile.NamedTemporaryFile(mode="w", dir=tmpdir, prefix="batch_algs_", suffix=".json") as alg_config_file:
                    alg_filename = alg_config_file.name
                    alg_config_file.write(json.dumps(alg_configs, indent=4))

                    # compose the command. We will do the following:
                    # use `jq` and `split` to grab each individual splits config in the file and write it into its own file of format config.splits.XXXXX
                    # use `jq` and `split` to grab each individual algorithm config in the file and write it into its own file of format config.alg.XXXXX
                    # --> NOTE, the XXXXX will match ! (splits starts at aaaaa -> aaaab -> aaaac -> ...)

                    # then, trigger our batch_run_splits.py file with the temporary directory as parameter
                    command = "jq -c '.[]' {0} | split -l 1 -a 5 - {2}/config.splits.;".format(splits_filename, alg_filename, tmpdir)
                    command += "jq -c '.[]' {1} | split -l 1 -a 5 - {2}/config.alg.;".format(splits_filename, alg_filename, tmpdir)
                    command += "python3 {0}/batch_run_splits.py {1}".format(FILE_DIR, tmpdir)
                    output = os.popen(command).read()

                    # The output of the command will a list of tool output strings
                    # we parse each into a dict individually, similar to the non-batch way (see above)
                    splits = []
                    for line in output.splitlines():
                        words = [key_val.split("=") for key_val in line.split()]
                        wire_splits = {self._wire_order[int(key)]: float(value) for (key, value) in words}
                        splits.append(wire_splits)

                    # create a pd.DataFrame (one row per input-point, columns are wires, cells show how much to spend on each wire)
                    # run a few checks to assert the data is correct
                    # no cell-values are negative
                    # all allowances for a given point (row) amount to the total_budget
                    splits_df = pd.DataFrame(splits)
                    assert not (splits_df < 0).any().any(), "There are no negative values in the increases"
                    assert ((splits_df.sum(axis=1) - total_budget).abs() < (10 ** -10)).all(), "All increases sum up to the TEST Budget"
                    return splits_df

    def batch_calculate_probability(self, points_df: pd.DataFrame):
        """
        Python is limited by the number of files we can open at once.
        Therefore, for bulk tests, we want the file system (not Python) to create individual configuration files.
        jq and split help us in achieving this.

        points_df: has points as rows and wires as columns
        """
        # convert each row into a probability config
        configs = points_df.apply(lambda row: self.create_probability_config(row.to_dict()), axis=1)
        configs = configs.to_list()  # configs is a pandas Series, make it into a vanilla Python list

        with tempfile.TemporaryDirectory(dir="./tmp") as tmpdir:
            with tempfile.NamedTemporaryFile(mode="w", dir=tmpdir, prefix="batch_", suffix=".json") as batchconfig:
                filename = batchconfig.name
                # print("Batch file name {}".format(filename))
                batchconfig.write(json.dumps(configs, indent=4))  # write the list of configs into the temporary file

                # call the command line to use
                # `jq` to split a configuration into individual lines (one line per configuration)
                # use `split` to grab an individual line (config) and write each it into its own file (with prefix="tmpfolder/config.")
                # use `ls` to write all created files into files.txt
                # read this files.txt and feed individually into our QCL tool (src/main propagate -f filename)
                command = "jq -c '.[]' {0} | split -l 1 -a 5 - {1}/config.;  ls {1}/config* > {1}/files.txt; while read in; do src/main propagate -f $in; done < {1}/files.txt".format(
                    filename, tmpdir)

                # the above command will trigger QCL on each line in files.txt
                # thus QCL will print one float-value (1 per config)
                # we read this output, split it and parse it into floats
                output = os.popen(command).read()
                return pd.Series([float(line) for line in output.splitlines()])

    # ------------------- CONFIGS -------------------

    def create_splits_config(self, point: Dict[Wire, float], resources: float, algorithm_config: Dict[str, float]):
        """
        point: Where we are right now (x-axis values of confidence function)
        resources: The budget we want to spend
        algorithm_config: configuration of the QCL tool e.g. {"max_step": 10000, "p_init": 0.999, "lambda": 1.0}
        """
        # check if the data looks alright
        if not all([self._check_point(point)]):
            return None

        conf_funcs = []
        for wire, exp in self.conf_funcs.items():
            windex = self._wire_order.index(wire)
            pos_exp_json = exp[0].to_json(windex * 2, windex)
            neg_exp_json = exp[1].to_json(windex * 2 + 1, windex)
            conf_funcs.append({"index": windex * 2, "name": "{} pos".format(wire), "expression": pos_exp_json})
            conf_funcs.append({"index": windex * 2 + 1, "name": "{} neg".format(wire), "expression": neg_exp_json})
        cfg = {
            "ft": self.ft.to_json(self._wire_order),
            "conf_funcs": conf_funcs,
            "point": [{"index": self._wire_order.index(wire), "name": str(wire), "value": value} for wire, value in
                      point.items()],
            "resources": float(resources)
        }
        # Algorithm config stuff
        alg_config_json = {"type": "sa",
                           "sa": {k: v for k, v in algorithm_config.items()}
                          }
        alg_config_json["sa"]["point"] = [v for v in cfg["point"]]
        return cfg, alg_config_json

    def create_probability_config(self, point: Dict):
        # check if the data looks alright
        if not any([self._check_point(point)]):
            return None

        cfg = {
            "ft": self.ft.to_json(self._wire_order),
            "point": [{"index": self._wire_order.index(wire), "value": value} for wire, value in point.items()]
        }

        return cfg

    def _check_point(self, point: Dict[Wire, float]):
        for wire in self.ft.wires:
            if wire not in point:
                breakpoint()
                print("Need to provide a point value for wire {}".format(wire))
                return False
        return True

    # ---------------------  -------------------

    def _tool_trigger(self, _function, config_filename, alg_filename=None):
        if not config_filename:
            raise ValueError("There is no config file specified. Stopping.")

        exe = os.path.join(self.__class__.TOOL_PATH, self.__class__.TOOL_NAME)
        command = [exe, _function, '-f', config_filename]
        if alg_filename:
            command.extend(["-o", alg_filename])

        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        # print("Opening subprocess with command `{}`".format(' '.join(process.args)))
        stdout, stderr = process.communicate()
        stderr = stderr.decode('ASCII').replace('cJSON_to_exp: no "name" specified, using "wire_index"', '').strip()
        if stderr:
            print("The tool invocation caused an error:\n\t{}".format(stderr))

        result = stdout.decode("ASCII")
        # logger.info("The Output is:\n{}".format(result))

        # delete the file we created
        return result

    def _write_config_to_file(self, configuration: Dict, suffix: str = ""):
        """
        configuration: what we want to write (is a Dict) (will be written as JSON)
        suffix: is helping to identify the individual tmp-files.
        """
        try:
            handle, filename = tempfile.mkstemp(dir=self.TOOL_INPUT_TMP_PATH, suffix="{}.json".format(suffix))
            fp = open(filename, "w")
            json.dump(configuration, fp, indent=4)
            return filename
        # except Exception as e:
        #     logger.error("ERROR: Problem when writing to file {}".format(filename), exc_info=True)
        finally:
            fp.close()

