/********************************************************************
 * expression_test.c
 *
 * Defines a battery of tests for expressions.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gc.h>
#include "utils.h"
#include "expression.h"
#include "expression_test.h"

int expression_test_equal() {
  int res = 0;
  // testing each basic expression type
  expression** exps = GC_MALLOC( EXP_CASES * sizeof( expression* ) );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    exps[ i ] = GC_MALLOC( 3 * sizeof( expression ) );
  }
  exps[ 0 ][ 0 ] = exp_var( 3, "a" );
  exps[ 0 ][ 1 ] = exp_var( 5, "b" );
  exps[ 0 ][ 2 ] = exp_var( 3, "a" );
  exps[ 1 ][ 0 ] = exp_const( 1.57 );
  exps[ 1 ][ 1 ] = exp_const( -0.6 );
  exps[ 1 ][ 2 ] = exp_const( 1.57 );
  for ( int i = 0; i < 3; ++i ) {
    exps[ 2 ][ i ] = exp_opp( exps[ 0 ][ i ] );
    exps[ 3 ][ i ] = exp_add( exps[ 1 ][ i ], exps[ 2 ][ i ] );
    exps[ 4 ][ i ] = exp_sub( exps[ 2 ][ i ], exps[ 3 ][ i ] );
    exps[ 5 ][ i ] = exp_mul( exps[ 3 ][ i ], exps[ 4 ][ i ] );
    exps[ 6 ][ i ] = exp_div( exps[ 4 ][ i ], exps[ 5 ][ i ] );
    exps[ 7 ][ i ] = exp_pow( exps[ 5 ][ i ], exps[ 6 ][ i ] );
    exps[ 8 ][ i ] = exp_log( exps[ 7 ][ i ] );
  }
  printf( "Strating exp_equal tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            EXP_TEST_EQUAL_BATTERIES_NUMBER * EXP_CASES );
    if ( exp_equal( exps[ i ][ 0 ], exps[ i ][ 1 ] ) ) {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    } else {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    }
    printf( "Test %d/%d: ", 2 * i + 2,
            EXP_TEST_EQUAL_BATTERIES_NUMBER * EXP_CASES );
    if ( exp_equal( exps[ i ][ 0 ], exps[ i ][ 2 ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
  }
  printf( "=========================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_EQUAL_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_EQUAL_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int expression_test_copy() {
  int res = 0;
  // testing each basic expression type
  expression** exps = GC_MALLOC( EXP_CASES * sizeof( expression* ) );
  for ( int i = 0; i < EXP_CASES; ++i )
    exps[ i ] = GC_MALLOC( 2 * sizeof( expression ) );
  exps[ 0 ][ 0 ] = exp_var( 3, "a" );
  exps[ 1 ][ 0 ] = exp_const( 1.57 );
  exps[ 2 ][ 0 ] = exp_opp( exps[ 0 ][ 0 ] );
  exps[ 3 ][ 0 ] = exp_add( exps[ 1 ][ 0 ], exps[ 2 ][ 0 ] );
  exps[ 4 ][ 0 ] = exp_sub( exps[ 2 ][ 0 ], exps[ 3 ][ 0 ] );
  exps[ 5 ][ 0 ] = exp_mul( exps[ 3 ][ 0 ], exps[ 4 ][ 0 ] );
  exps[ 6 ][ 0 ] = exp_div( exps[ 4 ][ 0 ], exps[ 5 ][ 0 ] );
  exps[ 7 ][ 0 ] = exp_pow( exps[ 5 ][ 0 ], exps[ 6 ][ 0 ] );
  exps[ 8 ][ 0 ] = exp_log( exps[ 7 ][ 0 ] );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    exps[ i ][ 1 ] = exp_copy( exps[ i ][ 0 ] );
  }
  printf( "Strating exp_copy tests.\n" );
  printf( "========================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            EXP_TEST_COPY_BATTERIES_NUMBER * EXP_CASES );
    if ( exp_equal( exps[ i ][ 0 ], exps[ i ][ 1 ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
    }
    printf_reset();
    printf( "Test %d/%d: ", 2 * i + 2,
            EXP_TEST_COPY_BATTERIES_NUMBER * EXP_CASES );
    if ( exps[ i ][ 0 ] == exps[ i ][ 1 ] ) {
      printf_bold_red();
      printf( "FAILED!\n" );
    } else {
      ++res;
      printf_green();
      printf( "passed.\n" );
    }
    printf_reset();
  }
  printf( "========================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_COPY_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_COPY_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "========================\n" );
  return res;
}

int expression_test_snprintf() {
  int res = 0;
  char* s;
  // testing each basic expression type
  expression* exps = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  // array of answers
  char** strings = GC_MALLOC( EXP_CASES * sizeof( char* ) );
  exps[ 0 ] = exp_var( 3, "a" );
  // TODO
  // change back when printing of expression is back to normal
  // strings[ 0 ] = "a";
  strings[ 0 ] = "x_3";
  exps[ 1 ] = exp_const( 1.57 );
  strings[ 1 ] = "1.570000";
  exps[ 2 ] = exp_opp( exps[ 0 ] );
  strings[ 2 ] = "- (x_3)";
  exps[ 3 ] = exp_add( exps[ 1 ], exps[ 2 ] );
  strings[ 3 ] = "(1.570000) + (- (x_3))";
  exps[ 4 ] = exp_sub( exps[ 2 ], exps[ 3 ] );
  strings[ 4 ] = "(- (x_3)) - ((1.570000) + (- (x_3)))";
  exps[ 5 ] = exp_mul( exps[ 3 ], exps[ 4 ] );
  strings[ 5 ] = "((1.570000) + (- (x_3))) * ((- (x_3)) - ((1.570000) + (- (x_3))))";
  exps[ 6 ] = exp_div( exps[ 4 ], exps[ 5 ] );
  strings[ 6 ] = "((- (x_3)) - ((1.570000) + (- (x_3)))) / (((1.570000) + (- (x_3))) * ((- (x_3)) - ((1.570000) + (- (x_3)))))";
  exps[ 7 ] = exp_pow( exps[ 5 ], exps[ 6 ] );
  strings[ 7 ] = "(((1.570000) + (- (x_3))) * ((- (x_3)) - ((1.570000) + (- (x_3))))) ^ (((- (x_3)) - ((1.570000) + (- (x_3)))) / (((1.570000) + (- (x_3))) * ((- (x_3)) - ((1.570000) + (- (x_3))))))";
  exps[ 8 ] = exp_log( exps[ 7 ] );
  strings[ 8 ] = "log ((((1.570000) + (- (x_3))) * ((- (x_3)) - ((1.570000) + (- (x_3))))) ^ (((- (x_3)) - ((1.570000) + (- (x_3)))) / (((1.570000) + (- (x_3))) * ((- (x_3)) - ((1.570000) + (- (x_3)))))))";
  printf( "Strating exp_snprintf tests.\n" );
  printf( "============================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            EXP_TEST_SNPRINTF_BATTERIES_NUMBER * EXP_CASES );
    int l = exp_snprintf( NULL, 0, exps[ i ] );
    s = GC_MALLOC( (l + 1) * sizeof( char ) );
    exp_snprintf( s, l + 1, exps[ i ] );
    if ( strcmp( s, strings[ i ] ) == 0 ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
  }
  printf( "============================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_SNPRINTF_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_SNPRINTF_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "============================\n" );
  return res;
}

int expression_test_eval() {
  double** sigma = GC_MALLOC( 2 * sizeof( double* ) );
  for ( int i = 0; i < 2; ++i ) sigma[ i ] = GC_MALLOC( 3 * sizeof( double ) );
  sigma[ 0 ][ 0 ] = 0;
  sigma[ 0 ][ 1 ] = 1;
  sigma[ 0 ][ 2 ] = 2.7;
  sigma[ 1 ][ 0 ] = 1.9;
  sigma[ 1 ][ 1 ] = -1;
  sigma[ 1 ][ 2 ] = -3.3;
  int res = 0;
  // testing each basic expression type
  expression* exps = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  // array of answers
  double** eval = GC_MALLOC( EXP_CASES * sizeof( double* ) );
  for ( int i = 0; i < EXP_CASES; ++i )
    eval[ i ] = GC_MALLOC( 2 * sizeof( double ) );
  exps[ 0 ] = exp_var( 0, "a" );
  eval[ 0 ][ 0 ] = 0;
  eval[ 0 ][ 1 ] = 1.9;
  exps[ 1 ] = exp_const( 1.57 );
  eval[ 1 ][ 0 ] = 1.57;
  eval[ 1 ][ 1 ] = 1.57;
  exps[ 2 ] = exp_opp( exps[ 0 ] );
  eval[ 2 ][ 0 ] = 0;
  eval[ 2 ][ 1 ] = -1.9;
  exps[ 3 ] = exp_add( exp_var( 1, "b" ), exps[ 2 ] );
  eval[ 3 ][ 0 ] = 1;
  eval[ 3 ][ 1 ] = -2.9;
  exps[ 4 ] = exp_sub( exp_var( 2, "c" ), exps[ 3 ] );
  eval[ 4 ][ 0 ] = 1.7;
  eval[ 4 ][ 1 ] = -0.4;
  exps[ 5 ] = exp_mul( exps[ 3 ], exps[ 4 ] );
  eval[ 5 ][ 0 ] = 1.7;
  eval[ 5 ][ 1 ] = 1.16;
  exps[ 6 ] = exp_div( exps[ 4 ], exps[ 5 ] );
  eval[ 6 ][ 0 ] = 1;
  eval[ 6 ][ 1 ] = -0.3448275862068966;
  exps[ 7 ] = exp_pow( exps[ 5 ], exps[ 6 ] );
  eval[ 7 ][ 0 ] = 1.7;
  eval[ 7 ][ 1 ] = 0.9501082893353264;
  exps[ 8 ] = exp_log( exps[ 5 ] );
  eval[ 8 ][ 0 ] = 0.5306282510621704;
  eval[ 8 ][ 1 ] = 0.14842000511827322;
  printf( "Strating exp_eval tests.\n" );
  printf( "==========================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            EXP_TEST_EVAL_BATTERIES_NUMBER * EXP_CASES );
    if ( dcompare( exp_eval( exps[ i ], sigma[ 0 ] ), eval[ i ][ 0 ], 10 ) == 0 ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
    printf( "Test %d/%d: ", 2 * i + 2,
            EXP_TEST_EVAL_BATTERIES_NUMBER * EXP_CASES );
    if ( dcompare( exp_eval( exps[ i ], sigma[ 1 ] ), eval[ i ][ 1 ], 10 ) == 0 ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
  }
  printf( "==========================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_EVAL_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_EVAL_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "==========================\n" );
  return res;
}

int expression_test_simplification() {
  int res = 0;
  // testing each basic expression type as many times as there are possible
  // cases
  expression** exps = GC_MALLOC( EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER *
                                 sizeof( expression* ) );
  for ( int i = 0; i < EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER; ++i )
    exps[ i ] = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  // array of answers
  expression** simpls = GC_MALLOC( EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER *
                                   sizeof( expression* ) );
  for ( int i = 0; i < EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER; ++i )
    simpls[ i ] = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  exps[ 0 ][ 0 ] = exp_const( 1 );
  exps[ 0 ][ 1 ] = exp_var( 0, "a" );
  exps[ 1 ][ 0 ] = exp_var( 0, "a" );
  exps[ 1 ][ 1 ] = exp_const( 1 );
  for ( int i = 0; i < EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER; ++i ) {
    exps[ i ][ 2 ] = exp_opp( exps[ i ][ 0 ] );
    exps[ i ][ 3 ] = exp_add( exps[ i ][ 0 ], exps[ i ][ 2 ] );
    exps[ i ][ 4 ] = exp_sub( exps[ i ][ 3 ], exps[ i ][ 2 ] );
    exps[ i ][ 5 ] = exp_mul( exps[ i ][ 4 ], exps[ i ][ 0 ] );
    exps[ i ][ 6 ] = exp_div( exps[ i ][ 5 ], exps[ i ][ 0 ] );
    exps[ i ][ 7 ] = exp_pow( exps[ i ][ 6 ], exps[ i ][ 0 ] );
    exps[ i ][ 8 ] = exp_log( exps[ i ][ 7 ] );
  }
  simpls[ 0 ][ 0 ] = exp_const( 1 );
  simpls[ 0 ][ 1 ] = exp_const( 2 ); // this value does not matter
  simpls[ 0 ][ 2 ] = exp_const( -1 );
  simpls[ 0 ][ 3 ] = exp_const( 0 );
  simpls[ 0 ][ 4 ] = exp_const( 1 );
  simpls[ 0 ][ 5 ] = exp_const( 1 );
  simpls[ 0 ][ 6 ] = exp_const( 1 );
  simpls[ 0 ][ 7 ] = exp_const( 1 );
  simpls[ 0 ][ 8 ] = exp_const( 0 );
  for ( int j = 0; j < EXP_CASES; ++j )
    simpls[ 1 ][ j ] = exps[ 1 ][ j ];
  // only used to evaluate the expression
  double sigma[1] = { 1.13 };
  printf( "Strating exp_snprintf tests.\n" );
  printf( "============================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER * EXP_CASES );
    if ( i == 1 || // special case for variable
         ( exp_simplification( exps[ 0 ][ i ] )->expression_type == CONST &&
           dcompare( exp_eval( exps[ 0 ][ i ], sigma ),
                     exp_eval( simpls[ 0 ][ i ], NULL ),
                     10 )
            == 0 ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
    printf( "Test %d/%d: ", 2 * i + 2,
            EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER * EXP_CASES );
    if ( exp_equal( exp_simplification( exps[ 1 ][ i ] ), simpls[ 1 ][ i ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
  }
  printf( "============================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "============================\n" );
  return res;
}

int expression_test_derivative() {
  double sigma[ 3 ] = { 0, 1, 2.7 };
  int res = 0;
  // testing each basic expression type
  expression* exps = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  // array of answers
  double** eval = GC_MALLOC( EXP_CASES * sizeof( double* ) );
  for ( int i = 0; i < EXP_CASES; ++i )
    eval[ i ] = GC_MALLOC( 2 * sizeof( double ) );
  exps[ 0 ] = exp_var( 0, "a" );
  eval[ 0 ][ 0 ] = 1;
  eval[ 0 ][ 1 ] = 0;
  exps[ 1 ] = exp_const( 1.57 );
  eval[ 1 ][ 0 ] = 0;
  eval[ 1 ][ 1 ] = 0;
  exps[ 2 ] = exp_opp( exps[ 0 ] );
  eval[ 2 ][ 0 ] = -1;
  eval[ 2 ][ 1 ] = 0;
  exps[ 3 ] = exp_add( exp_var( 1, "b" ), exps[ 2 ] );
  eval[ 3 ][ 0 ] = -1;
  eval[ 3 ][ 1 ] = 0;
  exps[ 4 ] = exp_sub( exp_var( 2, "c" ), exps[ 3 ] );
  eval[ 4 ][ 0 ] = 1;
  eval[ 4 ][ 1 ] = 1;
  exps[ 5 ] = exp_mul( exps[ 3 ], exps[ 4 ] );
  eval[ 5 ][ 0 ] = -0.7;
  eval[ 5 ][ 1 ] = 1;
  exps[ 6 ] = exp_div( exps[ 4 ], exps[ 5 ] );
  eval[ 6 ][ 0 ] = 1;
  eval[ 6 ][ 1 ] = 0;
  exps[ 7 ] = exp_pow( exps[ 5 ], exps[ 6 ] );
  eval[ 7 ][ 0 ] = 0.2020680268056897;
  eval[ 7 ][ 1 ] = 1;
  exps[ 8 ] = exp_log( exps[ 5 ] );
  eval[ 8 ][ 0 ] = -0.4117647058823529;
  eval[ 8 ][ 1 ] = 0.5882352941176471;
  printf( "Strating exp_derivative tests.\n" );
  printf( "==============================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            EXP_TEST_DERIVATIVE_BATTERIES_NUMBER * EXP_CASES );
    if ( dcompare( exp_eval( exp_derivative( exps[ i ], 0 ), sigma ), eval[ i ][ 0 ], 10 ) == 0 ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
    printf( "Test %d/%d: ", 2 * i + 1,
            EXP_TEST_DERIVATIVE_BATTERIES_NUMBER * EXP_CASES );
    if ( dcompare( exp_eval( exp_derivative( exps[ i ], 2 ), sigma ), eval[ i ][ 1 ], 10 ) == 0 ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
  }
  printf( "==========================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_DERIVATIVE_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_DERIVATIVE_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "==========================\n" );
  return res;
}

int expression_test_mul_1_many() {
  int res = 0;
  // testing one expression multiplied by an array of all expression types
  expression exp = exp_mul( exp_const( 2.3 ), exp_var( 1, "b" ) );
  expression* exps = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  // array of answers
  expression* ans = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  exps[ 0 ] = exp_var( 0, "a" );
  exps[ 1 ] = exp_const( 1.57 );
  exps[ 2 ] = exp_opp( exps[ 0 ] );
  exps[ 3 ] = exp_add( exp_var( 1, "b" ), exps[ 2 ] );
  exps[ 4 ] = exp_sub( exp_var( 2, "c" ), exps[ 3 ] );
  exps[ 5 ] = exp_mul( exps[ 3 ], exps[ 4 ] );
  exps[ 6 ] = exp_div( exps[ 4 ], exps[ 5 ] );
  exps[ 7 ] = exp_pow( exps[ 5 ], exps[ 6 ] );
  exps[ 8 ] = exp_log( exps[ 7 ] );
  for ( int i = 0; i < EXP_CASES; ++i )
    ans[ i ] = exp_mul( exp, exps[ i ] );
  expression* mul = exp_mul_1_many( exp, EXP_CASES, exps );
  printf( "Strating exp_mul_1_many tests.\n" );
  printf( "==============================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            EXP_TEST_MUL_1_MANY_BATTERIES_NUMBER * EXP_CASES );
    if ( exp_equal( mul[ i ], ans[ i ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
  }
  printf( "==========================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_MUL_1_MANY_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_MUL_1_MANY_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "==========================\n" );
  return res;
}

int expression_test_mul_array() {
  int res = 0;
  // testing multiplication of two arrays of expressions with possibly different
  // lengths
  expression* exps1 = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  expression* exps2 = GC_MALLOC( EXP_CASES * sizeof( expression ) );
  // array of answers
  expression** ans = GC_MALLOC( EXP_CASES * sizeof( expression* ) );
  for ( int i = 0; i < EXP_CASES; ++i )
    ans[ i ] = GC_MALLOC( ( i + 1 ) * sizeof( expression ) );
  exps1[ 0 ] = exp_var( 0, "a" );
  exps1[ 1 ] = exp_const( 1.57 );
  exps1[ 2 ] = exp_opp( exps1[ 0 ] );
  exps1[ 3 ] = exp_add( exp_var( 1, "b" ), exps1[ 2 ] );
  exps1[ 4 ] = exp_sub( exp_var( 2, "c" ), exps1[ 3 ] );
  exps1[ 5 ] = exp_mul( exps1[ 3 ], exps1[ 4 ] );
  exps1[ 6 ] = exp_div( exps1[ 4 ], exps1[ 5 ] );
  exps1[ 7 ] = exp_pow( exps1[ 5 ], exps1[ 6 ] );
  exps1[ 8 ] = exp_log( exps1[ 7 ] );
  for ( int i = 0; i < EXP_CASES; ++i )
    exps2[ i ] = exp_copy( exps1[ i ] );
  for ( int i = 0; i < EXP_CASES; ++i )
    for ( int j = 0; j < i + 1; ++j )
      ans[ i ][ j ] = exp_mul( exps1[ i ], exps2[ j ] );
  expression* muls = GC_MALLOC( EXP_CASES * ( EXP_CASES + 1 ) / 2 *
                                sizeof( expression ) );
  for ( int i = 0; i < EXP_CASES; ++i )
    for ( int j = 0; j < EXP_CASES; ++j )
      muls[ i * ( i + 1 ) / 2 + j ] = exp_mul( exps1[ i ], exps2[ j ] );
  printf( "Strating exp_mul_array tests.\n" );
  printf( "==============================\n" );
  for ( int i = 0; i < EXP_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            EXP_TEST_MUL_ARRAY_BATTERIES_NUMBER * EXP_CASES );
    bool eq = true;
    for ( int j = 0; j < i + 1; ++j )
      eq = eq && exp_equal( muls[ i * ( i + 1 ) / 2 + j ], ans[ i ][ j ] );
    if ( eq ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
      printf_reset();
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
      printf_reset();
    }
  }
  printf( "==========================\n" );
  printf( "Passed tests: " );
  if ( res == EXP_TEST_MUL_ARRAY_BATTERIES_NUMBER * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, EXP_TEST_MUL_ARRAY_BATTERIES_NUMBER * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "==========================\n" );
  return res;
}

void expression_test() {
  int batteries = EXP_TEST_EQUAL_BATTERIES_NUMBER +
                  EXP_TEST_COPY_BATTERIES_NUMBER +
                  EXP_TEST_SNPRINTF_BATTERIES_NUMBER +
                  EXP_TEST_EVAL_BATTERIES_NUMBER +
                  EXP_TEST_SIMPLIFICATION_BATTERIES_NUMBER +
                  EXP_TEST_DERIVATIVE_BATTERIES_NUMBER +
                  EXP_TEST_MUL_1_MANY_BATTERIES_NUMBER +
                  EXP_TEST_MUL_ARRAY_BATTERIES_NUMBER;
  int n = expression_test_equal();
  n += expression_test_copy();
  n += expression_test_snprintf();
  n += expression_test_eval();
  n += expression_test_simplification();
  n += expression_test_derivative();
  n += expression_test_mul_1_many();
  n += expression_test_mul_array();
  printf( "===================\n" );
  printf( "Total tests passed: " );
  if ( n == batteries * EXP_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", n, batteries * EXP_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "===================\n" );
}
