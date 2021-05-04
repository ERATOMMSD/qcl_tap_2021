/********************************************************************
 * sequent_test.c
 *
 * Defines a battery of tests for sequents.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <gc.h>
#include "utils.h"
#include "sequent.h"
#include "sequent_test.h"

#define SQT_CASES 6
// possible cases:
// - different conclusion formulas
// - different number of conlusion confidences
// - different conclusion confidences
// - different number of hypotheses
// - different hypotheses (formulas or confidences).

int sequent_test_equal() {
  int res = 0;
  // testing different numbers of hypotheses
  sequent** sqts = GC_MALLOC( SQT_CASES * sizeof( sequent* ) );
  for ( int i = 0; i < SQT_CASES; ++i )
    sqts[ i ] = GC_MALLOC( 3 * sizeof( sequent ) );
  // using different hypotheses lists and different confidence lists
  formula** hyp_fmls = GC_MALLOC( 3 * sizeof( formula* ) );
  expression** hyp_pos_cfds = GC_MALLOC( 3 * sizeof( expression* ) );
  expression** hyp_neg_cfds = GC_MALLOC( 3 * sizeof( expression* ) );
  expression** ccl_pos_cfds = GC_MALLOC( 3 * sizeof( expression* ) );
  expression** ccl_neg_cfds = GC_MALLOC( 3 * sizeof( expression* ) );
  for ( int i = 0; i < 3; ++i ) {
    hyp_fmls[ i ] = GC_MALLOC( 2 * sizeof( formula ) );
    hyp_fmls[ i ][ 0 ] = fml_pvar( 0, "a" );
    hyp_fmls[ i ][ 1 ] = fml_impl( hyp_fmls[ i ][ 0 ], fml_pvar( 1 + i % 2, "b" ) );
    hyp_pos_cfds[ i ] = GC_MALLOC( 2 * sizeof( expression ) );
    hyp_pos_cfds[ i ][ 0 ] = exp_var( 0, "a" );
    hyp_pos_cfds[ i ][ 1 ] = exp_mul( hyp_pos_cfds[ i ][ 0 ],
                                      exp_var( 1 + i % 2, "a" ) );
    hyp_neg_cfds[ i ] = GC_MALLOC( 2 * sizeof( expression ) );
    hyp_neg_cfds[ i ][ 0 ] = exp_var( 0, "a" );
    hyp_neg_cfds[ i ][ 1 ] = exp_mul( hyp_neg_cfds[ i ][ 0 ],
                                      exp_var( 1 + i % 2, "a" ) );
    ccl_pos_cfds[ i ] = GC_MALLOC( 2 * sizeof( expression ) );
    ccl_pos_cfds[ i ][ 0 ] = exp_var( 1, "b" );
    ccl_pos_cfds[ i ][ 1 ] = exp_mul( ccl_pos_cfds[ i ][ 0 ],
                                      exp_var( 2 + i % 2, "b" ) );
    ccl_neg_cfds[ i ] = GC_MALLOC( 2 * sizeof( expression ) );
    ccl_neg_cfds[ i ][ 0 ] = exp_var( 1, "b" );
    ccl_neg_cfds[ i ][ 1 ] = exp_mul( ccl_neg_cfds[ i ][ 0 ],
                                      exp_var( 2 + i % 2, "b" ) );
  }
  for ( int i = 0; i < 3; ++i ) {
    sqts[ 0 ][ i ] = sqt_new( 0, hyp_fmls[ i ], hyp_pos_cfds[ i ],
                              hyp_neg_cfds[ i ], fml_pvar( 1 + i % 2, "b" ), 0,
                              ccl_pos_cfds[ i ], 0, ccl_neg_cfds[ i ] );
    sqts[ 1 ][ i ] = sqt_new( 0, hyp_fmls[ i ], hyp_pos_cfds[ i ],
                              hyp_neg_cfds[ i ], fml_pvar( 1, "c" ), 1 + i % 2,
                              ccl_pos_cfds[ i ], 1 + i % 2, ccl_neg_cfds[ i ] );
    sqts[ 2 ][ i ] = sqt_new( 0, hyp_fmls[ i ], hyp_pos_cfds[ i ],
                              ccl_pos_cfds[ i ], fml_pvar( 1, "c" ), 2,
                              ccl_pos_cfds[ i ], 2, ccl_neg_cfds[ i ] );
    sqts[ 3 ][ i ] = sqt_new( i % 2, hyp_fmls[ 2 * ( i / 2 ) ],
                              hyp_pos_cfds[ 2 * ( i / 2 ) ],
                              hyp_neg_cfds[ 2 * ( i / 2 ) ], fml_pvar( 1, "c" ),
                              1, ccl_pos_cfds[ i ], 1, ccl_neg_cfds[ i ] );
    sqts[ 4 ][ i ] = sqt_new( 2, hyp_fmls[ i ], hyp_pos_cfds[ 2 * ( i / 2 ) ],
                              hyp_neg_cfds[ 2 * ( i / 2 ) ], fml_pvar( 1, "c" ),
                              1, ccl_pos_cfds[ i ], 1, ccl_neg_cfds[ i ] );
    sqts[ 5 ][ i ] = sqt_new( 2, hyp_fmls[ 2 * ( i / 2 ) ], hyp_pos_cfds[ i ],
                              hyp_neg_cfds[ i ], fml_pvar( 1, "c" ), 1,
                              ccl_pos_cfds[ i ], 1, ccl_neg_cfds[ i ] );
  }
  printf( "Strating sqt_equal tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < SQT_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            SQT_TEST_EQUAL_BATTERIES_NUMBER * SQT_CASES );
    if ( sqt_equal( sqts[ i ][ 0 ], sqts[ i ][ 1 ] ) ) {
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
            SQT_TEST_EQUAL_BATTERIES_NUMBER * SQT_CASES );
    if ( sqt_equal( sqts[ i ][ 0 ], sqts[ i ][ 2 ] ) ) {
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
  if ( res == SQT_TEST_EQUAL_BATTERIES_NUMBER * SQT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, SQT_TEST_EQUAL_BATTERIES_NUMBER * SQT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int sequent_test_copy() {
  int res = 0;
  // testing different numbers of hypotheses
  sequent** sqts = GC_MALLOC( SQT_CASES * sizeof( sequent* ) );
  for ( int i = 0; i < SQT_CASES; ++i )
    sqts[ i ] = GC_MALLOC( 2 * sizeof( sequent ) );
  // using different hypotheses lists and different confidence lists
  formula* hyp_fmls = GC_MALLOC( 2 * sizeof( formula ) );
  expression* hyp_pos_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  expression* hyp_neg_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  expression* ccl_pos_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  expression* ccl_neg_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  hyp_fmls[ 0 ] = fml_pvar( 0, "a" );
  hyp_fmls[ 1 ] = fml_impl( hyp_fmls[ 0 ], fml_pvar( 1, "b" ) );
  hyp_pos_cfds[ 0 ] = exp_var( 0, "a" );
  hyp_pos_cfds[ 1 ] = exp_mul( hyp_pos_cfds[ 0 ], exp_var( 1, "b" ) );
  hyp_neg_cfds[ 0 ] = exp_var( 0, "a" );
  hyp_neg_cfds[ 1 ] = exp_mul( hyp_neg_cfds[ 0 ], exp_var( 1, "b" ) );
  ccl_pos_cfds[ 0 ] = exp_var( 1, "b" );
  ccl_pos_cfds[ 1 ] = exp_mul( ccl_pos_cfds[ 0 ], exp_var( 2, "c" ) );
  ccl_neg_cfds[ 0 ] = exp_var( 1, "b" );
  ccl_neg_cfds[ 1 ] = exp_mul( ccl_neg_cfds[ 0 ], exp_var( 2, "c" ) );
  sqts[ 0 ][ 0 ] = sqt_new( 0, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                            fml_pvar( 1, "b" ), 0, ccl_pos_cfds, 0,
                            ccl_neg_cfds );
  sqts[ 1 ][ 0 ] = sqt_new( 0, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                            fml_pvar( 1, "b" ), 1, ccl_pos_cfds, 1,
                            ccl_neg_cfds );
  sqts[ 2 ][ 0 ] = sqt_new( 0, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                            fml_pvar( 1, "b" ), 2, ccl_pos_cfds, 2,
                            ccl_neg_cfds );
  sqts[ 3 ][ 0 ] = sqt_new( 0, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                            fml_pvar( 1, "b" ), 1, ccl_pos_cfds, 1,
                            ccl_neg_cfds );
  sqts[ 4 ][ 0 ] = sqt_new( 2, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                            fml_pvar( 1, "b" ), 1, ccl_pos_cfds, 1,
                            ccl_neg_cfds );
  sqts[ 5 ][ 0 ] = sqt_new( 2, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                            fml_pvar( 1, "b" ), 1, ccl_pos_cfds, 1,
                            ccl_neg_cfds );
  for ( int i = 0; i < SQT_CASES; ++i )
    sqts[ i ][ 1 ] = sqt_copy( sqts[ i ][ 0 ] );
  printf( "Strating sqt_copy tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < SQT_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            SQT_TEST_COPY_BATTERIES_NUMBER * SQT_CASES );
    if ( sqt_equal( sqts[ i ][ 0 ], sqts[ i ][ 1 ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
    }
    printf_reset();
    printf( "Test %d/%d: ", 2 * i + 2,
            SQT_TEST_COPY_BATTERIES_NUMBER * SQT_CASES );
    if ( sqts[ i ][ 0 ] == sqts[ i ][ 1 ] ) {
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
  if ( res == SQT_TEST_COPY_BATTERIES_NUMBER * SQT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, SQT_TEST_COPY_BATTERIES_NUMBER * SQT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int sequent_test_snprintf() {
  int res = 0;
  char* s;
  // testing each sequent type
  sequent* sqts = GC_MALLOC( SQT_CASES * sizeof( sequent ) );
  // using different hypotheses lists and different confidence lists
  formula* hyp_fmls = GC_MALLOC( 2 * sizeof( formula ) );
  expression* hyp_pos_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  expression* hyp_neg_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  expression* ccl_pos_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  expression* ccl_neg_cfds = GC_MALLOC( 2 * sizeof( expression ) );
  hyp_fmls[ 0 ] = fml_pvar( 0, "a" );
  hyp_fmls[ 1 ] = fml_impl( hyp_fmls[ 0 ], fml_pvar( 1, "b" ) );
  hyp_pos_cfds[ 0 ] = exp_var( 0, "a+" );
  hyp_pos_cfds[ 1 ] = exp_mul( hyp_pos_cfds[ 0 ], exp_var( 1, "b+" ) );
  hyp_neg_cfds[ 0 ] = exp_var( 0, "a-" );
  hyp_neg_cfds[ 1 ] = exp_mul( hyp_neg_cfds[ 0 ], exp_var( 1, "b-" ) );
  ccl_pos_cfds[ 0 ] = exp_var( 2, "c+" );
  ccl_pos_cfds[ 1 ] = exp_mul( ccl_pos_cfds[ 0 ], exp_var( 3, "d+" ) );
  ccl_neg_cfds[ 0 ] = exp_var( 2, "c-" );
  ccl_neg_cfds[ 1 ] = exp_mul( ccl_neg_cfds[ 0 ], exp_var( 3, "d-" ) );
  // array of answers
  char** strings = GC_MALLOC( SQT_CASES * sizeof( char* ) );
  sqts[ 0 ] = sqt_new( 0, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                       fml_pvar( 2, "c" ), 0, ccl_pos_cfds, 0, ccl_neg_cfds );
  // TODO
  // change back when printing of expression is back to normal
  // strings[ 0 ] = " |- c : (  ,  )";
  strings[ 0 ] = " |- c : (  ,  )";
  sqts[ 1 ] = sqt_new( 0, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                       fml_pvar( 2, "c" ), 1, ccl_pos_cfds, 1, ccl_neg_cfds );
  strings[ 1 ] = " |- c : ( x_2 , x_2 )";
  sqts[ 2 ] = sqt_new( 0, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                       fml_pvar( 2, "c" ), 2, ccl_pos_cfds, 2, ccl_neg_cfds );
  strings[ 2 ] = " |- c : ( x_2 | (x_2) * (x_3) , x_2 | (x_2) * (x_3) )";
  sqts[ 3 ] = sqt_new( 1, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                       fml_pvar( 2, "c" ), 0, ccl_pos_cfds, 0, ccl_neg_cfds );
  strings[ 3 ] = "a : ( x_0 , x_0 ) |- c : (  ,  )";
  sqts[ 4 ] = sqt_new( 2, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                       fml_pvar( 2, "c" ), 0, ccl_pos_cfds, 0, ccl_neg_cfds );
  strings[ 4 ] = "a : ( x_0 , x_0 ), (a) => (b) : ( (x_0) * (x_1) , (x_0) * (x_1) ) |- c : (  ,  )";
  sqts[ 5 ] = sqt_new( 2, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                       fml_pvar( 2, "c" ), 2, ccl_pos_cfds, 2, ccl_neg_cfds );
  strings[ 5 ] = "a : ( x_0 , x_0 ), (a) => (b) : ( (x_0) * (x_1) , (x_0) * (x_1) ) |- c : ( x_2 | (x_2) * (x_3) , x_2 | (x_2) * (x_3) )";
  printf( "Strating sqt_snprintf tests.\n" );
  printf( "============================\n" );
  for ( int i = 0; i < SQT_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            SQT_TEST_SNPRINTF_BATTERIES_NUMBER * SQT_CASES );
    int l = sqt_snprintf( NULL, 0, sqts[ i ] );
    s = GC_MALLOC( (l + 1) * sizeof( char ) );
    sqt_snprintf( s, l + 1, sqts[ i ] );
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
  if ( res == SQT_TEST_SNPRINTF_BATTERIES_NUMBER * SQT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, SQT_TEST_SNPRINTF_BATTERIES_NUMBER * SQT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "============================\n" );
  return res;
}

void sequent_test() {
  int batteries = SQT_TEST_EQUAL_BATTERIES_NUMBER +
                  SQT_TEST_COPY_BATTERIES_NUMBER +
                  SQT_TEST_SNPRINTF_BATTERIES_NUMBER;
  int n = sequent_test_equal();
  n += sequent_test_copy();
  n += sequent_test_snprintf();
  printf( "===================\n" );
  printf( "Total tests passed: " );
  if ( n == batteries * SQT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", n, batteries * SQT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "===================\n" );
}
