"""
This script is called from command line on a folder.
It will pair up the splits and algorithm configs and run the
QCL tool in budget-computation mode for each pair
"""
import os
import sys

FILE_DIR = os.path.dirname(__file__)
sys.path.append(os.path.join(FILE_DIR, ".."))
from pyASTRAHL.astrahl import Astrahl

def run_splits_in_folder(folder):
    # get all files and filter the interesting ones
    all_files = os.listdir(folder)
    split_configs = sorted([f for f in all_files if "config.splits." in f])
    alg_configs = sorted([f for f in all_files if "config.alg." in f])

    # we only call the tool on each file pair, the fault trees & confidence functions are already in the files
    # therefore, parameters are (None, None)
    astrahl = Astrahl(None, None)
    # pair up the splits_config and alg_config
    for split, alg in zip(split_configs, alg_configs):
        # to make sure the suffix (created by the `split` tool) match
        assert split.split(".")[-1] == alg.split(".")[-1]
        # call the tool to split the two files
        output_line = astrahl._tool_trigger(astrahl.SPLITS_KEYWORD,
                                            os.path.join(folder, split),
                                            os.path.join(folder, alg)).strip()
        print(output_line)  # re-print the tool-output, so it can be parsed by the caller

if __name__ == "__main__":
    # sys.argv is a folder-name in which we will calculate the splits (for each file-pair...)
    run_splits_in_folder(sys.argv[1])
