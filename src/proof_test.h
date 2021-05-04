/********************************************************************
 * proof_test.h
 *
 * Header of proof_test.c
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_PROOF_TEST_H__
#define __CCL_PROOF_TEST_H__

/* proof_test_equal: runs a series of tests to check whether prf_equal returns
 * the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define PRF_TEST_EQUAL_BATTERIES_NUMBER 2
// int proof_test_equal();

/* proof_test_copy: runs a series of tests to check whether prf_copy returns the
 * desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define PRF_TEST_COPY_BATTERIES_NUMBER 2
// int proof_test_copy();

// /* proof_test_snprintf: runs a series of tests to check whether prf_snprintf
//  * returns the desired result.
//  * inputs: none.
//  * output: number of passed tests.
//  * side effect: prints the series of tests.
//  */
#define PRF_TEST_SNPRINTF_BATTERIES_NUMBER 1
// // int proof_test_snprintf();

/* proof_test: runs a series of tests to check whether functions on proofs
 * return the desired results.
 * inputs: none.
 * output: none.
 * side effect: prints the series of tests.
 */
void proof_test();

#endif // __CCL_EXPRESSION_TEST_H__
