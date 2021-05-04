import numpy as np
import pandas as pd

import multiprocessing
from joblib import Parallel, delayed
from tqdm import tqdm

from pyASTRAHL import utils
from pyASTRAHL.api import *
from pyASTRAHL.astrahl import Astrahl


class QCLTheoreticalExperiment(object):
    def __init__(self, FT, CONF_FUNCTIONS):
        """ Defines Theoretical Experiment routines for a given FT and its confidence functions."""
        self.FT = FT
        self.CONF_FUNCTIONS = CONF_FUNCTIONS
        # for convenience (since all are the same in our expoeriments) we store one individuall
        self.conf_func = list(CONF_FUNCTIONS.values())[0]

        # create an Astrahl object so we can trigger the tool
        self.astrahl = Astrahl(self.FT, self.CONF_FUNCTIONS)

    def calculate_uniform(self, SP_df, TEST_BUDGET):
        """
        Split the budget by the number of wires, add it to the starting points
        trigger the confidence function to obtain confidences,
        then calcuate the failure probability.
        """
        increase_per_wire = TEST_BUDGET / len(self.FT.wires)  # calculate split
        EP_df = SP_df + increase_per_wire  # calculate EndPoint
        confidences = EP_df.applymap(self.conf_func)  # convert each cell to a confidence
        return self.astrahl.batch_calculate_probability(confidences)  # calculate the reliability for each row in the df

    def calculate_prop(self, SP_df, TEST_BUDGET):
        """
        Calculate the budget per point, considering proportionate splitting
        each row is a point
        """
        current_confidences_df = SP_df.applymap(self.conf_func)  # calculate current confidence of each wire
        inverted_df = 1 - current_confidences_df     # lack of confidence
        total_inverse = inverted_df.sum(axis=1)  # sum row-wise to see what the total non-confidence is per point
        increases_df = inverted_df.divide(total_inverse, axis=0)  * TEST_BUDGET  # divide the inverted confidence by the total_by_row, multiply by the TEST_BUDGET

        # Check that the sum per row equals test budget
        # check that all increases are positive
        assert not (increases_df < 0).any().any(), "There are no negative values in the increases"
        assert ((increases_df.sum(axis=1) - TEST_BUDGET).abs() < 10 ** -10).all(), "All row-increases sum up to the TEST Budget"

        # calculate the end-point (by cell-wise addition of dataframes)
        EP_df = SP_df + increases_df  # calculate EndPoint
        confidences = EP_df.applymap(self.conf_func)  # convert to confidences
        return self.astrahl.batch_calculate_probability(confidences)  # calculate the reliability for each row in the df

    def calculate_astrahl(self, SP_df, TEST_BUDGET, algorithm_config):
        """
        Use Astrahl to batch-calculate the resource-splitting for a dataframe of starting-points
        """

        # Development-time check. Make sure that batch-calculation matches individual calculations
        # splits_list = []
        # for rowidx, row in SP_df.iterrows():
        #     splits = self.astrahl.calculate_allowances(row, TEST_BUDGET, algorithm_config=algorithm_config, cleanup=True)
        #     splits_list.append(splits)
        #     # print(splits)
        # splits_df2 = pd.DataFrame(splits_list)

        splits_df = self.astrahl.batch_calculate_splits(SP_df, TEST_BUDGET, algorithm_config=algorithm_config)

        assert not (splits_df < 0).any().any(), "There are no negative values in the increases"
        assert ((splits_df.sum(axis=1) - TEST_BUDGET).abs() < (10 ** -10)).all(), "All increases sum up to the TEST Budget"

        EP_df = SP_df + splits_df[SP_df.columns]  # calculate EndPoint
        confidences = EP_df.applymap(self.conf_func)  # convert to confidences

        return self.astrahl.batch_calculate_probability(confidences)  # calculate the reliability for each row in the df


class QCLEmpiricalExperiment(object):
    def __init__(self, FT, CONF_FUNCTIONS):
        self.FT = FT
        self.CONF_FUNCTIONS = CONF_FUNCTIONS
        self.conf_func = list(CONF_FUNCTIONS.values())[0]

        self.astrahl = Astrahl(self.FT, self.CONF_FUNCTIONS)
        self.TESTCOST = 10

    def mean_fix_and_prob(self, resource_allowances, fault_counts_df, FAULT_OBSERVABILITY, samples):
        # trigger self.failure_probabilities_from_fault_counts 100 times
        # do it in parallel, because time is precious
        failure_prob_samples = Parallel(n_jobs=multiprocessing.cpu_count())(
        delayed(self.fix_faults_and_get_fault_probabilities)(resource_allowances, fault_counts_df.copy(), FAULT_OBSERVABILITY) for _ in tqdm(range(samples)))

        # calculate the mean across our 100 runs
        failure_prob_df = pd.DataFrame(failure_prob_samples)  # columns = FaultDists
        means = failure_prob_df.mean()  # calculate the means across experiments
        return means

    def fix_faults_and_get_fault_probabilities(self, splits, fault_counts_df, fault_observability):
        """
        Takes a resource split and a dataframe of #faults for each wire.
        Tries to fix faults probabilistically (simulating testing).
        Then calculates system failure chance afterwards (according to #faults)
        """
        splits_df = pd.DataFrame([splits] * len(fault_counts_df))  # create a df with all equal rows (resource splitting)
        # development time information... allows us to compare the reduction in fault counts
        # failure_probabilities_before = failure_probabilities_from_fault_counts(fault_df, fault_observabilitiy=fault_observability)
        assert splits_df.shape == fault_counts_df.shape

        # the following lines do the following
        # zip(splits_df.values.flat, fault_counts_df.values.flat) creates a list of (allowance, #faults)-tuples
        # we iterate over this list and reduce (#faults) according to (allowance) probabilistically
        # the result is a long list of reduced fault numbers
        new_vals = [
            self.fix_faults_on_wire(wire_allowance, num_faults,
                                     fault_observability=fault_observability)
            for wire_allowance, num_faults in zip(splits_df.values.flat, fault_counts_df.values.flat)]

        # we re-convert our large list into a dataframe using numpy.reshape
        fault_df_after = pd.DataFrame(np.reshape(new_vals, (len(fault_counts_df), len(fault_counts_df.columns))),
                                      index=fault_counts_df.index,
                                      columns=fault_counts_df.columns)

        # now we calculate the row-wise system failure probability (using Astrahl), based on the fault–counts in each row
        return self.failure_probabilities_from_fault_counts(fault_df_after, fault_observabilitiy=fault_observability)

    def failure_probabilities_from_fault_counts(self, fault_df, fault_observabilitiy):
        """
        Calculate system confidence from number of faults per wire.
        fault_df is a dataframe where each column is a wire and each row is fault distribution
        each cell shows how many faults there are per wire for a given fault distribution.

        (rows are different, because fix_faults_on_wire is probabilistic).
        """
        # convert number of faults (in each cell) to a failure probability for each cell
        confidences_df = fault_df.applymap(lambda num_faults: 1 - (1 - fault_observabilitiy) ** num_faults)

        # rename wires to remove "Faults"
        rename_dict = {c: c.replace(" Faults", "") for c in [c for c in fault_df.columns if "Faults" in c]}
        confidences_df.rename(columns=rename_dict, inplace=True)
        # and now make them to Wires
        name_to_wire = {wire.name: wire for wire in self.FT.wires}
        confidences_df.rename(columns=name_to_wire, inplace=True)

        # calculate failure probability
        failure_probabilities = self.astrahl.batch_calculate_probability(confidences_df)
        return failure_probabilities

    def fix_faults_on_wire(self, allowance: float, num_faults: int, fault_observability: float = 0.1):
        """
        Probabilistically removes faults according to allowance.
        """
        allowance = allowance / self.TESTCOST  # how many full test can we run?

        while allowance > 0:  # iteratively remove from allowance, where allowance is the budget we invest
            # factor is how much we invest (for instance, if allowance is 2.4, we do it twice + 1 time with a factor of 0.4)
            factor = 1 if allowance >= 1 else allowance
            allowance -= 1

            # Check each fault if it has been found
            for fault in range(num_faults):
                # generate a new random for each fault
                # since not every test is good for every fault
                # if the fault is hard to observe (ie. small value), random needs to be very small to remove it
                # if the fault is easy to spot (ie. large value),
                fault_removal_value = random.random()
                if fault_removal_value < (1 - (1 - fault_observability) ** factor):
                    num_faults -= 1
                num_faults = max(num_faults, 0)
        return num_faults  # return how many faults remain

