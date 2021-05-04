/********************************************************************
 * sequent_test.h
 *
 * Header of sequent_test.c
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_SEQUENT_TEST_H__
#define __CCL_SEQUENT_TEST_H__

/* sequent_test_equal: runs a series of tests to check whether sqt_equal returns
 * the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define SQT_TEST_EQUAL_BATTERIES_NUMBER 2
// int sequent_test_equal();

/* sequent_test_copy: runs a series of tests to check whether sqt_copy returns
 * the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define SQT_TEST_COPY_BATTERIES_NUMBER 2
// int sequent_test_copy();

/* sequent_test_snprintf: runs a series of tests to check whether sqt_snprintf
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define SQT_TEST_SNPRINTF_BATTERIES_NUMBER 1
// int sequent_test_snprintf();

/* sequent_test: runs a series of tests to check whether functions on
 * sequents return the desired results.
 * inputs: none.
 * output: none.
 * side effect: prints the series of tests.
 */
void sequent_test();

#endif // __CCL_SEQUENT_TEST_H__
