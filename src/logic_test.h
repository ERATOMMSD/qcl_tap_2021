/********************************************************************
 * logic_test.h
 *
 * Header of logic_test.c
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_LOGIC_TEST_H__
#define __CCL_LOGIC_TEST_H__

/* logical_operator_test_equal: runs a series of tests to check whether
 * log_equal returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define LO_TEST_EQUAL_BATTERIES_NUMBER 2
// int logical_operator_test_equal();

/* logical_operator_test_copy: runs a series of tests to check whether log_copy
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define LO_TEST_COPY_BATTERIES_NUMBER 2
// int logical_operator_test_copy();

/* logical_operator_test_snprintf: runs a series of tests to check whether
 * log_snprintf returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define LO_TEST_SNPRINTF_BATTERIES_NUMBER 1
// int logical_operator_test_snprintf();

/* logic_test_equal: runs a series of tests to check whether log_equal
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define LOG_TEST_EQUAL_BATTERIES_NUMBER 2
// int logic_test_equal();

/* logic_test_copy: runs a series of tests to check whether log_copy
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define LOG_TEST_COPY_BATTERIES_NUMBER 2
// int logic_test_copy();

/* logic_test_snprintf: runs a series of tests to check whether
 * log_snprintf returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define LOG_TEST_SNPRINTF_BATTERIES_NUMBER 1
// int logic_test_snprintf();

/* logic_test: runs a series of tests to check whether functions on
 * logics return the desired results.
 * inputs: none.
 * output: none.
 * side effect: prints the series of tests.
 */
void logic_test();

#endif // __CCL_LOGIC_TEST_H__
