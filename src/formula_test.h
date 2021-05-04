/********************************************************************
 * formula_test.h
 *
 * Header of formula_test.c
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_FORMULA_TEST_H__
#define __CCL_FORMULA_TEST_H__

/* formula_test_equal: runs a series of tests to check whether fml_equal
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FML_TEST_EQUAL_BATTERIES_NUMBER 2
// int formula_test_equal();

/* formula_test_copy: runs a series of tests to check whether fml_copy returns
 * the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FML_TEST_COPY_BATTERIES_NUMBER 2
// int formula_test_copy();

/* formula_test_snprintf: runs a series of tests to check whether
 * fml_snprintf returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FML_TEST_SNPRINTF_BATTERIES_NUMBER 1
// int formula_test_snprintf();

/* formula_test_eval: runs a series of tests to check whether fml_eval
 * returns the desired result.
 * inputs: none.
 * output: number of passed tests.
 * side effect: prints the series of tests.
 */
#define FML_TEST_EVAL_BATTERIES_NUMBER 2
// int formula_test_eval();

/* formula_test: runs a series of tests to check whether functions on formulas
 * return the desired results.
 * inputs: none.
 * output: none.
 * side effect: prints the series of tests.
 */
void formula_test();

#endif // __CCL_FORMULA_TEST_H__
