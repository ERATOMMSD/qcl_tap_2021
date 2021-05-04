/********************************************************************
 * utils.h
 *
 * Header of utils.c.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_UTILS_H__
#define __CCL_UTILS_H__

#include <stdbool.h>

/************
 * Integers *
 ************/

/* int_pow: computes the power of two integers.
 * input:
 * - the base number [base],
 * - the exponent [exp].
 * output: the power.
 */
int int_pow( int base, int exp );

/* int_length: computes the length of a number written in base 10.
 * input: the number [n].
 * output: its length.
 */
int int_length( int n );

/***********
 * Doubles *
 ***********/

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif /* M_PI */

/* dequal: tests whether two doubles are equal, up to some tolerance.
 * inputs:
 * - doubles [x] and [y] to compare,
 * - a [tolerance] factor (the bigger, the more lenient comparison is), 1 should
 *   be okay for a single operation.
 * output: a boolean.
 */
// bool dequal( double x, double y, double tolerance );

/* dcompare: tests whether two doubles are equal, up to some tolerance.
 * inputs:
 * - doubles [x] and [y] to compare,
 * - a [tolerance] factor (the bigger, the more lenient comparison is), 1 should
 *   be okay for a single operation.
 * output: an integer that is:
 * - positive when [x] > [y],
 * - negative when [x] < [y],
 * - 0 when [x] = [y] (up to the given tolerance).
 */
int dcompare( double x, double y, double tolerance );

/*
 * l1_norm: computes the L1 norm of a vector of non-negative coefficients (i.e.,
 * it sums the components) while ensuring that the coefficients are
 * non-negative.
 * inputs:
 * - an integer [n] that represents the dimension of the vector,
 * - an array [v] of doubles representing the vector.
 * output: a double.
 */
double l1_norm( int n, double* v );

/* box_muller: computes the Box-Muller transform of two independent random
 * variables uniformly distributed in [0,1] into two independent random
 * variables following Gaussian distrbiutions of mean 0 and standard deviation
 * 1.
 * inputs:
 * - doubles [x] and [y], representing the two samples,
 * - an array [res] of doubles to store the two transformed samples (needs to
 *   have at least two spaces left).
 * output: nothing (stored in [res]).
 */
void box_muller( double x, double y, double* res );

/*************
 * Arguments *
 *************/

#define TEST_INT            6
#define TEST_EXPRESSION_BIT 0
#define TEST_FORMULA_BIT    1
#define TEST_SEQUENT_BIT    2
#define TEST_PROOF_BIT      3
#define TEST_FAULT_TREE_BIT 4
#define TEST_LOGIC_BIT      5

#define ARGS_MODE_PROPAGATE_STR "propagate"
#define ARGS_MODE_SPLITS_STR "splits"
#define ARGS_MODE_BENCHMARK_STR "benchmark"

typedef struct args_s {
  int test;
  int print_level;
  char* mode;
  char* filename;
  char* filename_optimisation;
} *args;

args args_new();

void args_read( args a, int argc, char** argv );

/************
 * Printing *
 ************/

#define BASIC_PRINT_LEVEL 1

// int verbosity_level = 0;
// 
// /* printf_verbose: only prints if [verbosity_level] is big enough.
//  * inputs:
//  * - the minimum verbosity level [i] that prints something.
//  * output: nothing.
//  * side-effect: prints (or not).
//  */
// void printf_verbose( int i );

/* Printf color modifications. */
void printf_green();
void printf_bold_green();
void printf_bold_red();
void printf_reset();

/***********
 * Vectors *
 ***********/

/* square_norm: computes the square of the norm of a vector.
 * inputs:
 * - an array [v] of doubles representing the coordinates of the
 *   vector,
 * - the size [n] of the vector.
 * output: the squared norm.
 */
double square_norm( int n, double* v );

#endif // __CCL_UTILS_H__
