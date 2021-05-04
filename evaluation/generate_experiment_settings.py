"""
Produces a set of random Fault Trees, Starting Points and Fault Distributions.
All experimental settings should use the same FTs, SPs and FDs.
This file produces them.
"""
import os
import sys
import numpy as np
import pandas as pd
FILE_DIR = os.path.dirname(__file__)

sys.path.append(os.path.join(FILE_DIR, ".."))
from pyASTRAHL.api import *  # for defining the Wires, FT, Confidence functions, etc.

import logging
logging.basicConfig(
    format='[%(asctime)s] %(levelname)-8s %(funcName)-15s : %(message)s',
    datefmt='%H:%M:%S',
    level=logging.WARNING
)
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

# ------------------------------------------------------------------
#              SETTINGS
# ------------------------------------------------------------------
NUM_WIRES = 6  # the size of the fault trees

evaluation_settings_folder = os.path.join(FILE_DIR, "settings")
os.makedirs(evaluation_settings_folder, exist_ok=True)

# generate 200 FTs, the empirical experiments will only use the first 50
FT_count = 200
FT_file = os.path.join(evaluation_settings_folder, "fault_trees.json")

# generate 100 SPs in the interval [100,300] for the theoretical part
SP_theoretical_count = 100
SP_theoretical_file = os.path.join(evaluation_settings_folder, "starting_points_theoretical.json")

# generate 50 SPs in the interval [10,70] for the empirical part
# empirical have lower confidence so we actually produce faults
SP_empirical_count = 50
SP_empirical_file = os.path.join(evaluation_settings_folder, "starting_points_empirical.json")

# produce 50 fault distributions for each empirical SP
# might produce duplicates.
# we do not filter out duplicates, since it would change the error probability created by the starting point
FDist_count = 50
FDist_file = os.path.join(evaluation_settings_folder, "fault_distributions_for_empirical.json")

# ------------------------------------------------------------------

def _generate_random_FT(size, wires=None):
    """
    Creates a fault tree of a certain size, adds its wires to the list.
    This operates recursively.
    """
    if wires is None:
        wires = []

    if size == 1:  # if the tree size is 1, then don't do anything
        wire = Wire("Wire {}".format(len(wires)))
        wires.append(wire)
        return wire

    node_type = random.choice([And, Or])
    if size == 2:
        left_size = 1
        right_size = 1
    else:
        left_size = random.randint(1, size - 1)
        right_size = size - left_size

    return node_type(
        _generate_random_FT(size=left_size, wires=wires),
        _generate_random_FT(size=right_size, wires=wires)
    )


def generate_FTs(FT_count):
    """Generate FT_count FTs, check to not produce duplicates."""
    logger.info(f"Creating {FT_count} fault trees")
    ft_df = pd.read_json(FT_file) if os.path.exists(FT_file) else pd.DataFrame(columns=["FaultTree"])
    logger.debug(f"{len(ft_df)} exist already")

    fts_json = ft_df["FaultTree"].to_list()
    while len(ft_df) < FT_count:
        found_one = False
        idx = 0
        while not found_one and idx < 1000:  # try 1000 times to generate a new one
            idx += 1
            new_ft = _generate_random_FT(NUM_WIRES).to_json()
            if new_ft not in fts_json:
                ft_df = ft_df.append({"FaultTree": new_ft}, ignore_index=True)
                fts_json.append(new_ft)
                found_one = True
            else:
                print("Exists already", idx)
    ft_df.to_json(FT_file)


def generate_theoretical_SPs(SP_count):
    """Generate starting points for the theoretical evaluation as pd.DataFrame"""
    logger.info("Creating %d starting points for empirical", SP_count)

    sp_df = pd.DataFrame(np.random.uniform(100, 300, size=(SP_count, NUM_WIRES)),  # use numpy to create a two-dimensional array of random values in [100,300]
                         columns=[f"Wire {wire_idx}" for wire_idx in range(NUM_WIRES)])
    sp_df.to_json(SP_theoretical_file)


def generate_empirical_SPs(SP_count):
    """Generate starting points for the empirical evaluation as pd.DataFrame"""
    logger.info("Creating %d starting points for empirical", SP_count)

    sp_df = pd.DataFrame(np.random.uniform(10, 70, size=(SP_count, NUM_WIRES)),  # use numpy to create a two-dimensional array of random values in [10,70]
                         columns=["Wire {}".format(wire_idx) for wire_idx in range(NUM_WIRES)])
    sp_df.to_json(SP_empirical_file)


def _create_FaultDist(SP_IDX, STARTING_POINT):
    """
    Create for a given starting point a dictionary of {wire: #faults},
    where #faults is according to a geometric distribution of the confidence.
    We add the starting point and SP_idx for convenience. (we can then filter by it)
    """
    conf_func = 1 - 0.99 ** (Var() + 1)
    fault_dist = {"{} Faults".format(wire): np.random.geometric(1 - conf_func(val)) for wire, val in STARTING_POINT.items()}
    return {"SP_idx":SP_IDX, **STARTING_POINT, **fault_dist}


def generate_FaultDists(SP_IDX, STARTING_POINT, FDist_count):
    # generates a bunch (#FDist_count) FDists for a given starting point
    logger.info("Creating fault distribution for %s", str(STARTING_POINT))
    fd_df = pd.read_json(FDist_file) if os.path.exists(FDist_file) else pd.DataFrame(columns=["Wire {}".format(wire_idx) for wire_idx in range(NUM_WIRES)])

    # filter starting points
    sp_df = fd_df
    for key, value in STARTING_POINT.items():
        sp_df = sp_df[sp_df[key] == value]

    num_missing = FDist_count - len(sp_df)
    logger.debug("%d fault distributions missing for starting point", num_missing)
    for _ in range(num_missing):
        new_fault_dist = _create_FaultDist(SP_IDX, STARTING_POINT)
        fd_df = fd_df.append(new_fault_dist, ignore_index=True)

    fd_df.to_json(FDist_file)


def generate_all_FaultDists(FDist_count):
    logger.info("Creating %d fault distributions for each starting point", FDist_count)
    sp_df = pd.read_json(SP_empirical_file)
    for rowidx, row in sp_df.iterrows():
        generate_FaultDists(rowidx, row.to_dict(), FDist_count)
    fd_df = pd.read_json(FDist_file)


# ------------------------------------------------------------------
#              RUN
# ------------------------------------------------------------------

if __name__ == "__main__":
    generate_FTs(FT_count)
    generate_theoretical_SPs(SP_theoretical_count)
    generate_empirical_SPs(SP_empirical_count)
    generate_all_FaultDists(FDist_count)