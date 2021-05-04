/********************************************************************
 * formula_test.c
 *
 * Defines a battery of tests for formulas.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <gc.h>
#include "utils.h"
#include "formula.h"
#include "formula_test.h"

int formula_test_equal() {
  int res = 0;
  // testing each basic formula type
  formula** fmls = GC_MALLOC( FML_CASES * sizeof( formula* ) );
  for ( int i = 0; i < FML_CASES; ++i ) {
    fmls[ i ] = GC_MALLOC( 3 * sizeof( formula ) );
  }
  fmls[ 0 ][ 0 ] = fml_pvar( 3, "a" );
  fmls[ 0 ][ 1 ] = fml_pvar( 5, "b" );
  fmls[ 0 ][ 2 ] = fml_pvar( 3, "a" );
  for ( int i = 0; i < 3; ++i ) {
    fmls[ 1 ][ i ] = fml_conj( fmls[ 0 ][ i ], fmls[ 0 ][ i ] );
    fmls[ 2 ][ i ] = fml_disj( fmls[ 0 ][ i ], fmls[ 1 ][ i ] );
    fmls[ 3 ][ i ] = fml_impl( fmls[ 1 ][ i ], fmls[ 2 ][ i ] );
  }
  printf( "Strating fml_equal tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < FML_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            FML_TEST_EQUAL_BATTERIES_NUMBER * FML_CASES );
    if ( fml_equal( fmls[ i ][ 0 ], fmls[ i ][ 1 ] ) ) {
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
            FML_TEST_EQUAL_BATTERIES_NUMBER * FML_CASES );
    if ( fml_equal( fmls[ i ][ 0 ], fmls[ i ][ 2 ] ) ) {
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
  if ( res == FML_TEST_EQUAL_BATTERIES_NUMBER * FML_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FML_TEST_EQUAL_BATTERIES_NUMBER * FML_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int formula_test_copy() {
  int res = 0;
  // testing each basic formula type
  formula** fmls = GC_MALLOC( FML_CASES * sizeof( formula* ) );
  for ( int i = 0; i < FML_CASES; ++i ) {
    fmls[ i ] = GC_MALLOC( 2 * sizeof( formula ) );
  }
  fmls[ 0 ][ 0 ] = fml_pvar( 3, "a" );
  fmls[ 1 ][ 0 ] = fml_conj( fmls[ 0 ][ 0 ], fmls[ 0 ][ 0 ] );
  fmls[ 2 ][ 0 ] = fml_disj( fmls[ 0 ][ 0 ], fmls[ 1 ][ 0 ] );
  fmls[ 3 ][ 0 ] = fml_impl( fmls[ 1 ][ 0 ], fmls[ 2 ][ 0 ] );
  for ( int i = 0; i < FML_CASES; ++i )
    fmls[ i ][ 1 ] = fml_copy( fmls[ i ][ 0 ] );
  printf( "Strating fml_copy tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < FML_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            FML_TEST_COPY_BATTERIES_NUMBER * FML_CASES );
    if ( fml_equal( fmls[ i ][ 0 ], fmls[ i ][ 1 ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
    }
    printf_reset();
    printf( "Test %d/%d: ", 2 * i + 2,
            FML_TEST_COPY_BATTERIES_NUMBER * FML_CASES );
    if ( fmls[ i ][ 0 ] == fmls[ i ][ 1 ] ) {
      printf_bold_red();
      printf( "FAILED!\n" );
    } else {
      ++res;
      printf_green();
      printf( "passed.\n" );
    }
    printf_reset();
  }
  printf( "=========================\n" );
  printf( "Passed tests: " );
  if ( res == FML_TEST_COPY_BATTERIES_NUMBER * FML_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FML_TEST_COPY_BATTERIES_NUMBER * FML_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int formula_test_snprintf() {
  int res = 0;
  char* s;
  // testing each basic formula type
  formula* fmls = GC_MALLOC( FML_CASES * sizeof( formula ) );
  // array of answers
  char** strings = GC_MALLOC( FML_CASES * sizeof( char* ) );
  fmls[ 0 ] = fml_pvar( 3, "a" );
  strings[ 0 ] = "a";
  fmls[ 1 ] = fml_conj( fmls[ 0 ], fmls[ 0 ] );
  strings[ 1 ] = "(a) /\\ (a)";
  fmls[ 2 ] = fml_disj( fmls[ 0 ], fmls[ 1 ] );
  strings[ 2 ] = "(a) \\/ ((a) /\\ (a))";
  fmls[ 3 ] = fml_impl( fmls[ 1 ], fmls[ 2 ] );
  strings[ 3 ] = "((a) /\\ (a)) => ((a) \\/ ((a) /\\ (a)))";
  printf( "Strating fml_snprintf tests.\n" );
  printf( "============================\n" );
  for ( int i = 0; i < FML_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            FML_TEST_SNPRINTF_BATTERIES_NUMBER * FML_CASES );
    int l = fml_snprintf( NULL, 0, fmls[ i ] );
    s = GC_MALLOC( (l + 1) * sizeof( char ) );
    fml_snprintf( s, l + 1, fmls[ i ] );
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
  if ( res == FML_TEST_SNPRINTF_BATTERIES_NUMBER * FML_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FML_TEST_SNPRINTF_BATTERIES_NUMBER * FML_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "============================\n" );
  return res;
}

int formula_test_eval() {
  bool** sigma = GC_MALLOC( 2 * sizeof( bool* ) );
  for ( int i = 0; i < 2; ++i ) sigma[ i ] = GC_MALLOC( 3 * sizeof( bool ) );
  sigma[ 0 ][ 0 ] = true;
  sigma[ 0 ][ 1 ] = false;
  sigma[ 0 ][ 2 ] = false;
  sigma[ 1 ][ 0 ] = true;
  sigma[ 1 ][ 1 ] = true;
  sigma[ 1 ][ 2 ] = false;
  int res = 0;
  // testing each basic formula type
  formula* fmls = GC_MALLOC( FML_CASES * sizeof( formula ) );
  // array of answers
  bool** eval = GC_MALLOC( FML_CASES * sizeof( bool* ) );
  for ( int i = 0; i < FML_CASES; ++i )
    eval[ i ] = GC_MALLOC( 2 * sizeof( bool ) );
  fmls[ 0 ] = fml_pvar( 0, "a" );
  eval[ 0 ][ 0 ] = true;
  eval[ 0 ][ 1 ] = true;
  fmls[ 1 ] = fml_conj( fmls[ 0 ], fml_pvar( 1, "b" ) );
  eval[ 1 ][ 0 ] = false;
  eval[ 1 ][ 1 ] = true;
  fmls[ 2 ] = fml_disj( fmls[ 1 ], fml_pvar( 2, "c" ) );
  eval[ 2 ][ 0 ] = false;
  eval[ 2 ][ 1 ] = true;
  fmls[ 3 ] = fml_impl( fml_pvar( 0, "a" ), fmls[ 2 ] );
  eval[ 3 ][ 0 ] = false;
  eval[ 3 ][ 1 ] = true;
  printf( "Strating fml_eval tests.\n" );
  printf( "========================\n" );
  for ( int i = 0; i < FML_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            FML_TEST_EVAL_BATTERIES_NUMBER * FML_CASES );
    if ( fml_eval( fmls[ i ], sigma[ 0 ] ) == eval[ i ][ 0 ] ) {
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
            FML_TEST_EVAL_BATTERIES_NUMBER * FML_CASES );
    if ( fml_eval( fmls[ i ], sigma[ 1 ] ) == eval[ i ][ 1 ] ) {
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
  if ( res == FML_TEST_EVAL_BATTERIES_NUMBER * FML_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FML_TEST_EVAL_BATTERIES_NUMBER * FML_CASES );
  printf_reset();
  printf( ".\n" );
  return res;
}

void formula_test() {
  int batteries = FML_TEST_EQUAL_BATTERIES_NUMBER +
                  FML_TEST_COPY_BATTERIES_NUMBER +
                  FML_TEST_SNPRINTF_BATTERIES_NUMBER +
                  FML_TEST_EVAL_BATTERIES_NUMBER;
  int n = formula_test_equal();
  n += formula_test_copy();
  n += formula_test_snprintf();
  n += formula_test_eval();
  printf( "===================\n" );
  printf( "Total tests passed: " );
  if ( n == batteries * FML_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", n, batteries * FML_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "===================\n" );
}
