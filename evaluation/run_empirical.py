import os
import sys
import time
import multiprocessing

import pandas as pd

FILE_DIR = os.path.dirname(__file__)
sys.path.append(os.path.join(FILE_DIR, ".."))

from pyASTRAHL.api import *  # for defining the Wires, FT, Confidence functions, etc.
from pyASTRAHL import qcl

# ------------------------------------------------------------------

# Create experiment
NUM_THREADS = multiprocessing.cpu_count()
evaluation_folder = "./evaluation"
evaluation_settings_folder = os.path.join(evaluation_folder, "settings")

# Hyper Parameters
COMPETITORS = ["Uniform", "Prop_iter_1", "Prop_iter_10"]
HYPER_PARAMS = {"max_step": 20000, "p_init": 0.0001, "lambda": 35, "function_constant": 0.99}

EMPIRICAL_ALLOWANCES = [60, 120, 240, 360, 480, 600]
# EMPIRICAL_ALLOWANCES = [60, 120]  # faster, for testing
TESTCOST = 10
EMPIRICAL_SAMPLE_SIZE = 100

FT_file = os.path.join(evaluation_settings_folder, "fault_trees.json")
SP_theoretical_file = os.path.join(evaluation_settings_folder, "starting_points_theoretical.json")
SP_empirical_file = os.path.join(evaluation_settings_folder, "starting_points_empirical.json")
FDist_file = os.path.join(evaluation_settings_folder, "fault_distributions_for_empirical.json")


# -------------------- Parsing Data
ft_df = pd.read_json(FT_file)
theo_sp_df = pd.read_json(SP_theoretical_file)
emp_sp_df = pd.read_json(SP_empirical_file)


def experiment_setup(config):
    # get the correct fault tree (in its serialized string-form) from the file (actually the dataframe)
    ft_json = ft_df.loc[config["FT_index"]]["FaultTree"]

    # parse it... (i.e. JSON-string -> FT-object)
    FT = FaultTree.from_json(ft_json)
    # define the confidence function for each Wire
    CONF_FUNCTIONS = {wire : 1 - 0.99 ** (Var() + 1) for wire in FT.wires}

    # Parse the starting point!
    name_to_wire = {wire.name : wire for wire in FT.wires}
    sp_dict = emp_sp_df.loc[config["SP_index"]].to_dict()
    STARTING_POINT = {name_to_wire[name]: value for name, value in sp_dict.items()}

    config["FT"] = FT
    config["SP"] = STARTING_POINT
    config["CF"] = CONF_FUNCTIONS


def run_empirical(config):
    FAULT_OBSERVABILITY = 0.1  # define fault observability globally

    # setup a theoretical experiment object for a given fault tree and confidence functions
    # also save the hyper-params for the QCL tool in that object (because they don't change)
    # store the TESTCOST hyper-parameter
    experiment = qcl.QCLEmpiricalExperiment(config["FT"], config["CF"])
    experiment.algorithm_config = HYPER_PARAMS
    experiment.TESTCOST = TESTCOST

    print("\n\n* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *")
    print("EMPRICIAL EXPERIMENT on FT", config["FT_index"], "SP", config["SP_index"])
    print("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *")
    # read Fault Distributions from the (huge) fault distributions file

    fd_df = pd.read_json(FDist_file)
    fd_df = fd_df[fd_df["SP_idx"] == config["SP_index"]]  # filter so we only have those that match our Starting Point

    # fd_df contains columns for starting points, the SP_index and the Faults for each wire
    # create a dataframe with only the fault counts per wire
    fault_counts_df = fd_df[[c for c in fd_df.columns if "Faults" in c]]

    SP_df = pd.DataFrame([config["SP"]])  # create a dataframe for the starting point-dict (so we can use the nice API)

    for allowance in EMPIRICAL_ALLOWANCES:
        print("Allowance", allowance, "- Astrahl")
        # calculate the resource splits according to Astrahl,
        astrahl_splits = experiment.astrahl.calculate_allowances(config["SP"], allowance, algorithm_config=HYPER_PARAMS, cleanup=True)
        # then probabilistically fix faults and calculate failure probability EMPIRICAL_SAMPLE_SIZE times
        astrahl_means = experiment.mean_fix_and_prob(astrahl_splits, fault_counts_df, FAULT_OBSERVABILITY, EMPIRICAL_SAMPLE_SIZE)

        # Uniform splits
        print("Allowance", allowance, "- Uniform")
        # calculate the resource splits according to Uniform Splitting
        uniform_splits = {c: (allowance / len(fault_counts_df.columns)) for c in fault_counts_df.columns}  # uniform_split = {wire: total / #num_wires}
        # then probabilistically fix faults and calculate failure probability EMPIRICAL_SAMPLE_SIZE times
        uniform_means = experiment.mean_fix_and_prob(uniform_splits, fault_counts_df, FAULT_OBSERVABILITY, EMPIRICAL_SAMPLE_SIZE)

        # Prop splits
        print("Allowance", allowance, "- Prop")
        # calculate the resource splits according to Proportional Splitting
        current_confidences_df = SP_df.copy().applymap(experiment.conf_func)   # calculate current confidence of each wire
        inverted_df = 1 - current_confidences_df     # invert to lack of confidence
        total_inverse = inverted_df.sum(axis=1)  # sum row-wise to see what the total non-confidence is per point
        prop_splits_df = inverted_df.divide(total_inverse, axis=0) * allowance  # divide the inverted confidence by the total_by_row, multiply by the TEST_BUDGET
        prop_splits_dict = prop_splits_df.loc[0].to_dict()  # we want a dict here, not a df
        # then probabilistically fix faults and calculate failure probability EMPIRICAL_SAMPLE_SIZE times
        prop_means = experiment.mean_fix_and_prob(prop_splits_dict, fault_counts_df, FAULT_OBSERVABILITY, EMPIRICAL_SAMPLE_SIZE)

        df = pd.DataFrame({
            "Astrahl": astrahl_means,
            "Uniform": uniform_means,
            "Prop": prop_means,
            "Delta_Uniform": astrahl_means - uniform_means,
            "Delta_Prop": astrahl_means - prop_means,
        })

        # check for how many Starting Points competitors are better than Astrahl
        if len(df[df["Delta_Uniform"] > 0]):
            print("FT:", config["FT_index"], "SP:", config["SP_index"], "Allowance:", allowance, " - Note that there are", len(df[df["Delta_Uniform"] > 0]) ,"a SP where Uniform is better than Astrahl")
        if len(df[df["Delta_Prop"] > 0]):
            print("FT:", config["FT_index"], "SP:", config["SP_index"], "Allowance:", allowance, " - Note that there are", len(df[df["Delta_Prop"] > 0]) ,"a SP where Prop is better than Astrahl")

        # Write to File
        outputfile = os.path.join(evaluation_folder, "empirical", f"FT_{config['FT_index']}", f"SP_{config['SP_index']}", "Allowance_{}.json".format(allowance))
        os.makedirs(os.path.dirname(outputfile), exist_ok=True)
        df.to_json(outputfile, indent=2)


if __name__ == "__main__":
    """Run the empirical experiment on a single Fault Tree and Starting Point, but for all the SPs Fault Distributions."""
    # FT_index -> first CLI argument
    # SP_index -> second CLI argument
    config = dict(FT_index=int(sys.argv[1]), SP_index=int(sys.argv[2]), threads=NUM_THREADS,
                  testcost=TESTCOST, emp_allowances=EMPIRICAL_ALLOWANCES)
    experiment_setup(config)
    run_empirical(config)