import pprint
import random 
from functools import reduce
from .api import *
import numpy as np


import logging
logger = logging.getLogger(__name__)

#-----------------------
# ALLOWANCE STRATEGIES
#----------------------

def calculate_random_wire_allowances(current_confidences, allowance, algorithm_config=None):
    """Note, algorithm_config is completely ignored, but we need it to match the signature of astrahl's function"""
    wires = current_confidences.keys()
    randoms = {wire: random.random() for wire in wires}
    sum_random = sum(randoms.values())
    return {wire: val / sum_random * allowance for wire, val in randoms.items()}

def calculate_uniform_wire_allowances(current_confidences, allowance, algorithm_config=None):
    """Note, algorithm_config is completely ignored, but we need it to match the signature of astrahl's function"""
    wires = current_confidences.keys()
    return {wire: allowance / len(wires) for wire in wires}

def calculate_proportional_wire_allowances(current_confidences, allowance, algorithm_config=None):
    """Note, algorithm_config is completely ignored, but we need it to match the signature of astrahl's function"""
    wires = current_confidences.keys()
    inverse_values = {wire : 1 - current_confidences[wire] for wire in wires}
    total_inverse = sum(inverse_values.values())
    return {wire: inverse / total_inverse * allowance for wire, inverse in inverse_values.items()}


#-----------------------
# Empirical Evaluation 
#----------------------

class Fault(object):
    def __init__(self, observability=None):
        """ High observability means it's easy to fix and find."""
        self.observability = observability or np.random.beta(0.09, 8.9)   
        # https://www.wolframalpha.com/input/?assumption=%7B%22C%22%2C+%22beta%22%7D+-%3E+%7B%22ProbabilityDistribution%22%7D&i=beta%280.09%2C8.9%29

        # (1.15, 10)
        # https://www.wolframalpha.com/input/?assumption=%7B%22C%22%2C+%22beta%22%7D+-%3E+%7B%22ProbabilityDistribution%22%7D&i=beta%281.15%2C10%29
        
    def __str__(self):
        return repr(self)
    
    def __repr__(self):
        return "Fault({:.4f})".format(self.observability)

def create_fault_distribution(wire_confidences, num_faults=20, fault_drop_limit=1):
    """ Tries to allocate faults to the individual wires, given a starting point and confidence functions.
    
    Iteratively tries to assign a parameterizable number of faults to the wires. 
    For every i in range(20), the function will create a fault with a random 'observability' in the range [0, 0.125). 
    Observability is an indicator of how hard it is to encounter the fault. Our __assumption__ here is that a fault that is
    difficult to encounter and observe is more severe. Its observability will therefore be lower.
    
    The algorithm works as follows: 
        1. create a fault and assign a random observability in [0, 0.125)
        2. Assign the fault to a wire (or drop it)
            a) create a random assignment_chance value
            b) randomly choose a wire or None (None indicates that we will drop the fault and not assign it to a wire)
            c) if None: drop the fault and go to 1.  (actually, the fault_drop_limit parameter can be configured so that we have to hit None more than once)
            d) otherwise:
                d1) multiply the fault's observability with the wire's confidence
                d2) if assignment_chance is larger than the value from d1), then add the fault to the wire and move to 1.
                d3) otherwise, step back to 2.b) and choose another wire randomly (potentially even the same wire)
                
    This procedure runs until all 'num_faults' have been either assigned to wires or dropped.
    
    Note, that our logic behind step d1) is as follows:
    a fault assignment is a probabilistic, but also dependent on two factors: 
    1. it's observability (harder to find faults have lower observability), 
    and 2. the wire's confidence point (if we're confident in the wire, we it should be less likely that there is a fault)
    Hence, the more confident we are in a component (ie larger value) and the "easier to spot" a fault is (ie. the larger the value), then the more likely we will accept the fault for the wire.
    """
    ft_wires = wire_confidences.keys()
    fault_distribution = {wire : [] for wire in ft_wires}  # empty list of wires

    wires_and_none = list(ft_wires) + [None]  # we add None, because this means we won't assign the fault
    
    for i in range(num_faults):
        logger.debug("****** Fault #{}".format(i))

        fault = Fault()
        fault_drop_count = 0

        logger.debug("Created fault with observability {}".format(fault.observability))
        continue_loop = True

        while continue_loop:
            wire = random.choice(wires_and_none)  # None means we don't assign this fault
            logger.debug("Randomly chose wire {} --- Point confidence: {} ".format(wire, wire_confidences[wire] if wire else None))
            if not wire:
                if fault_drop_count >= (fault_drop_limit - 1):
                    logger.debug("Selected {} {} times. This means we're dropping this fault.".format(wire, fault_drop_limit))
                    continue_loop = False
                    break
                else:
                    fault_drop_count += 1
                    logger.debug("Selected {}. This means we won't assign this fault this time, increasing counter to {}.".format(wire, fault_drop_count))
                    continue

            assignment_chance = random.random() # to make a probability with which we assign the fault
            log_text = "because assignment chance was {}; observability={:.4f} * conf={:.4f} = {:.4f}".format(assignment_chance, fault.observability, wire_confidences[wire], (fault.observability) * (wire_confidences[wire]))
            if assignment_chance * (1 - fault.observability) > wire_confidences[wire]: 
                logger.debug("Assigned fault to wire {} {}".format(wire, log_text))
                fault_distribution[wire].append(fault)
                continue_loop = False
                break
            else:
                logger.debug("Didn't assign fault to wire {} {}".format(wire, log_text))


    assigned = sum([len(l) for w, l in fault_distribution.items()])
    logger.info("Total faults assigned: {} dropped: {}".format(assigned, num_faults - assigned))
    logger.info("Final fault distribution:")
    for wire in ft_wires:
        logger.info("Wire {} (conf={:.4f}): {} faults; {}".format(wire,wire_confidences[wire], len(fault_distribution[wire]),fault_distribution[wire]))
        
    return fault_distribution

def fix_faults(wire_allowances, fault_distribution, testcost=1):
    # create a copy of the distribution
    test_fault_distribution = {wire: list(faults) for wire, faults in fault_distribution.items()}

    for wire, allowance in wire_allowances.items():
#         print("Allowance ", allowance, " Incl Testcost", (allowance / testcost), "Testcost", testcost)
        allowance = allowance / testcost
        
        logger.debug("Testing wire {}: #allowance = {} ({} faults): {}".format(wire, allowance, len(test_fault_distribution[wire]), test_fault_distribution[wire]))
        while allowance > 0: # iteratively remove allownace, where allowance is the time we invest
            # factor is how much we invest (for instance, if allowance is 2.4, we do it twice + 1 time with a factor of 0.4)
            factor = 1 if allowance >= 1 else allowance
            allowance -= 1

            # Check each fault if it has been found
            for fault in test_fault_distribution[wire]:
                # generate a new random for each fault
                # since not every test is good for every fault
                # if the fault is hard to observe (ie. small value), random needs to be very small to remove it
                # if the fault is easy to spot (ie. large value), 
                fault_removal_value = random.random()
                if fault_removal_value < (1 - (1 - fault.observability) ** factor): 
                    logger.debug("Random value was {}, fault observability was {}: Removing".format(fault_removal_value,fault.observability))
                    test_fault_distribution[wire].remove(fault)
        logger.debug("After testing: {} faults; {}".format(len(test_fault_distribution[wire]), test_fault_distribution[wire]))

    logger.info("Testing effect:")
    for wire in wire_allowances.keys():
        logger.info("Wire {}: allowance {}; faults after testing {} of {} (~ {:.1f} %) faults.".format(wire, wire_allowances[wire], len(test_fault_distribution[wire]), len(fault_distribution[wire]), len(test_fault_distribution[wire]) / len(fault_distribution[wire]) * 100 if len(fault_distribution[wire]) > 0 else 0   ))  
        
    return test_fault_distribution




def calculate_system_fault_probability(tool_api, test_fault_distribution):
    chance_of_wire_faults = {}
    for wire, faults in test_fault_distribution.items():
        if len(faults) == 0:
            chance_of_wire_faults[wire] = 0
        else:
            chance_of_wire_faults[wire] = 1 - reduce(lambda x, y: x * y, [ 1 - fault.observability for fault in faults])
    
    logger.debug("Wire fault chances")
    for wire in test_fault_distribution.keys():
        logger.debug("Wire {}: {}; faults after testing {}: {}".format(wire, chance_of_wire_faults[wire], len(test_fault_distribution[wire]),test_fault_distribution[wire]))
    
    print(chance_of_wire_faults)
    result = tool_api.calculate_probability(chance_of_wire_faults)
    logger.info("System fault chance: {}".format(result))
    return result

def batch_calculte_system_fault_probability(tool_api, test_fault_distributions):
    fault_chances = []
    for test_fault_distribution in test_fault_distributions:
        chance_of_wire_faults = {}
        for wire, faults in test_fault_distribution.items():
            if len(faults) == 0:
                chance_of_wire_faults[wire] = 0
            else:
                chance_of_wire_faults[wire] = 1 - reduce(lambda x, y: x * y, [ 1 - fault.observability for fault in faults])

#         chance_of_wire_faults = {wire: 1 - (reduce(lambda x, y: x * y, [ 1 - fault.observability for fault in faults]) if len(faults) > 0 else 0) for wire, faults in test_fault_distribution.items()}
        fault_chances.append(chance_of_wire_faults)
    
    results = tool_api.batch_calculate_probability(fault_chances)
    return results