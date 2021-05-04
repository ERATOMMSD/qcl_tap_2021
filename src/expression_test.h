/********************************************************************
 * expression_test.h
 *
 * Header of expression_test.c
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_EXPRESSION_TEST_H__
#define __CCL_EXPRESSION_TEST_H__

/* expression_test_equal: runs a series of tests to check whether exp_equal
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define EXP_TEST_EQUAL_BATTERIES_NUMBER 2
// int expression_test_equal();

/* expression_test_copy: runs a series of tests to check whether exp_copy
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define EXP_TEST_COPY_BATTERIES_NUMBER 2
// int expression_test_copy();

/* expression_test_snprintf: runs a series of tests to check whether
 * exp_snprintf returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define EXP_TEST_SNPRINTF_BATTERIES_NUMBER 1
// int expression_test_snprintf();

/* expression_test_eval: runs a series of tests to check whether exp_eval
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define EXP_TEST_EVAL_BATTERIES_NUMBER 2
// int expression_test_eval();

/* expression_test_simplification: runs a series of tests to check whether
 * exp_simplification returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */

#define EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER 2
// two tests to perform:
// - sometimes reduces to a constant with the same as the evaluated expression
// - sometimes does not reduce
// #define EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER 0
// int expression_test_simplification();

/* expression_test_derivative: runs a series of tests to check whether
 * exp_derivative returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define EXP_TEST_DERIVATIVE_BATTERIES_NUMBER 2
// int expression_test_derivative();

/* expression_test_mul_1_many: runs a series of tests to check whether
 * exp_mul_1_many returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define EXP_TEST_MUL_1_MANY_BATTERIES_NUMBER 1
// int expression_test_mul_1_many();

/* expression_test_mul_array: runs a series of tests to check whether
 * exp_mul_array returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define EXP_TEST_MUL_ARRAY_BATTERIES_NUMBER 1
// int expression_test_mul_array();

/* expression_test: runs a series of tests to check whether functions on
 * expressions return the desired results.
 * inputs: none.
 * output: none.
 * side effect: prints the series of tests.
 */
void expression_test();

#endif // __CCL_EXPRESSION_TEST_H__
