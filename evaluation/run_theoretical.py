import os
import sys
import time
import multiprocessing

from joblib import Parallel, delayed
from tqdm import tqdm

import pandas as pd
# import numpy as np

FILE_DIR = os.path.dirname(__file__)

sys.path.append(os.path.join(FILE_DIR, ".."))

from pyASTRAHL.api import *  # for defining the Wires, FT, Confidence functions, etc.
from pyASTRAHL import qcl
# from pyASTRAHL.utils import Fault
# from pyASTRAHL.astrahl import Astrahl

# ------------------------------------------------------------------

# Create experiment
evaluation_folder = "./evaluation"
evaluation_settings_folder = os.path.join(evaluation_folder, "settings")

# Hyper Parameters
COMPETITORS = ["Uniform", "Prop_iter_1", "Prop_iter_10"]
THEORETICAL_ALLOWANCES = [1, 10, 50, 100, 250, 500, 1000]
# THEORETICAL_ALLOWANCES = [500, 1000]  # faster, for testing
HYPER_PARAMS = {"max_step": 20000, "p_init": 0.0001, "lambda": 35, "function_constant": 0.99}

EMPIRICAL_ALLOWANCES = [60, 120, 240, 360, 480, 600]
TESTCOST = 10
EMPIRICAL_SAMPLE_SIZE = 100

FT_file = os.path.join(evaluation_settings_folder, "fault_trees.json")
SP_theoretical_file = os.path.join(evaluation_settings_folder, "starting_points_theoretical.json")
SP_empirical_file = os.path.join(evaluation_settings_folder, "starting_points_empirical.json")
FDist_file = os.path.join(evaluation_settings_folder, "fault_distributions_for_empirical.json")


# -------------------- Parsing Data
# print(FT_file)
ft_df = pd.read_json(FT_file)
theo_sp_df = pd.read_json(SP_theoretical_file)
emp_sp_df = pd.read_json(SP_empirical_file)
fd_df = pd.read_json(FDist_file)


def experiment_setup(config):
    # get the correct fault tree (in its serialized string-form) from the file (actually the dataframe)
    ft_json = ft_df.loc[config["FT_index"]]["FaultTree"]

    # parse it... (i.e. JSON-string -> FT-object)
    FT = FaultTree.from_json(ft_json)

    # define the confidence function for each Wire
    CONF_FUNCTIONS = {wire : 1 - 0.99 ** (Var() + 1) for wire in FT.wires}

    # store this information in the config
    config["FT"] = FT
    config["CF"] = CONF_FUNCTIONS


def run_theoretical(config):
    print("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *")
    print("THEORETICAL EXPERIMENT on FT", config["FT_index"])
    print("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *")

    # setup a theoretical experiment object for a given fault tree and confidence functions
    # also save the hyper-params for the QCL tool in that object (because they don't change)
    experiment = qcl.QCLTheoreticalExperiment(config["FT"], config["CF"])
    experiment.algorithm_config = HYPER_PARAMS

    # make sure that the column headers are wires, not strings
    name_to_wire = {wire.name: wire for wire in config["FT"].wires}
    theo_sp_df.rename(columns=name_to_wire, inplace=True)

    # make sure the output data-folder exists
    ft_folder = os.path.join(FILE_DIR, "theoretical", "FT_{}".format(config["FT_index"]))
    os.makedirs(ft_folder, exist_ok=True)

    start = time.time()
    # for each allowance, run the individual resource splitting approaches (defined in QCLTheoreticalExperiment)
    for allowance in reversed(THEORETICAL_ALLOWANCES):
        print("Allowance", allowance)

        # calculate (1 - dataframe) to convert from reliability to system unreliability
        astrahl = 1 - experiment.calculate_astrahl(theo_sp_df.copy(), allowance, algorithm_config=HYPER_PARAMS)
        uniform = 1 - experiment.calculate_uniform(theo_sp_df.copy(), allowance)
        prop = 1 - experiment.calculate_prop(theo_sp_df.copy(), allowance)

        # create a DataFrame (because they have a nicer API)
        # it contains the competitors and the differences
        df = pd.DataFrame({
            "Astrahl": astrahl,
            "Uniform": uniform,
            "Prop": prop,
            "Delta_Uniform": astrahl - uniform,
            "Delta_Prop": astrahl - prop,
        })

        # check for how many Starting Points competitors are better than Astrahl
        if len(df[df["Delta_Uniform"] > 0]):
            print("FT", config["FT_index"], "Allowance", allowance, "Note that there are", len(df[df["Delta_Uniform"] > 0]) ," SPs where Uniform is better than Astrahl")
        if len(df[df["Delta_Prop"] > 0]):
            print("FT", config["FT_index"], "Allowance", allowance, "Note that there are", len(df[df["Delta_Prop"] > 0]) ," SPs where Prop is better than Astrahl")

        # Write to File
        outputfile = os.path.join(ft_folder, "Allowance_{}.json".format(allowance))
        df.to_json(outputfile, indent=2)

    print("Theoretical Experiment took ", time.time() - start, "seconds")


def run_theoretical_for_FT(ft_idx):
    """Operates on an individual FT"""
    config = {"FT_index": int(ft_idx)}

    # fill the config here (i.e. parse the fault tree from file, define the confidence functions, and the starting points)
    experiment_setup(config)
    run_theoretical(config)

if __name__ == "__main__":
    """ Runs all experiments for a single FT (or multiple FTs) and all their Starting Points"""

    # we can pass several FT indices and run experiments on them in parallel
    FTs_to_run = [int(arg) for arg in sys.argv[1:]]
    results = Parallel(n_jobs=4)(
        delayed(run_theoretical_for_FT)(ft) for ft in tqdm(FTs_to_run))  # each result contains (row_index, data)
