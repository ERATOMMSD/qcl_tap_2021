/********************************************************************
 * fault_tree_test.h
 *
 * Header of fault_tree_test.c
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_FAULT_TREE_TEST_H__
#define __CCL_FAULT_TREE_TEST_H__

/* fault_tree_test_equal: runs a series of tests to check whether fltt_equal
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FLTT_TEST_EQUAL_BATTERIES_NUMBER 2
// int fault_tree_test_equal();

/* fault_tree_test_copy: runs a series of tests to check whether fltt_copy
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FLTT_TEST_COPY_BATTERIES_NUMBER 2
// int fault_tree_test_copy();

/* fault_tree_test_snprintf: runs a series of tests to check whether
 * fltt_snprintf returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FLTT_TEST_SNPRINTF_BATTERIES_NUMBER 1
// int fault_tree_test_snprintf();

/* fault_tree_test_propagate: runs a series of tests to check whether
 * fltt_propagate returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FLTT_TEST_PROPAGATE_BATTERIES_NUMBER 2
// int fault_tree_test_propagate();

/* fault_tree_test_prf: runs a series of tests to check whether fltt_to_prf
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FLTT_TEST_PRF_BATTERIES_NUMBER 1
// int fault_tree_test_prf();

/* fault_tree_test: runs a series of tests to check whether functions on fault_trees
 * return the desired results.
 * inputs: none.
 * output: none.
 * side effect: prints the series of tests.
 */
void fault_tree_test();

#endif // __CCL_FAULT_TREE_TEST_H__
