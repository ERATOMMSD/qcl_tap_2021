/********************************************************************
 * fault_tree_test.c
 *
 * Defines a battery of tests for fault trees.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <gc.h>
#include "utils.h"
#include "fault_tree.h"
#include "fault_tree_test.h"

int fault_tree_test_equal() {
  int res = 0;
  // testing each basic fault_tree type
  fault_tree** fltts = GC_MALLOC( FLTT_CASES * sizeof( fault_tree* ) );
  for ( int i = 0; i < FLTT_CASES; ++i ) {
    fltts[ i ] = GC_MALLOC( 3 * sizeof( fault_tree ) );
  }
  fltts[ 0 ][ 0 ] = fltt_wire( 3 );
  fltts[ 0 ][ 1 ] = fltt_wire( 5 );
  fltts[ 0 ][ 2 ] = fltt_wire( 3 );
  for ( int i = 0; i < 3; ++i ) {
    fltts[ 1 ][ i ] = fltt_and( fltts[ 0 ][ i ], fltts[ 0 ][ i ] );
    fltts[ 2 ][ i ] = fltt_or( fltts[ 0 ][ i ], fltts[ 1 ][ i ] );
    fltts[ 3 ][ i ] = fltt_pand( fltts[ 1 ][ i ], fltts[ 2 ][ i ] );
  }
  printf( "Strating fltt_equal tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < FLTT_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            FLTT_TEST_EQUAL_BATTERIES_NUMBER * FLTT_CASES );
    if ( fltt_equal( fltts[ i ][ 0 ], fltts[ i ][ 1 ] ) ) {
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
            FLTT_TEST_EQUAL_BATTERIES_NUMBER * FLTT_CASES );
    if ( fltt_equal( fltts[ i ][ 0 ], fltts[ i ][ 2 ] ) ) {
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
  if ( res == FLTT_TEST_EQUAL_BATTERIES_NUMBER * FLTT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FLTT_TEST_EQUAL_BATTERIES_NUMBER * FLTT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int fault_tree_test_copy() {
  int res = 0;
  // testing each basic fault tree type
  fault_tree** fltts = GC_MALLOC( FLTT_CASES * sizeof( fault_tree* ) );
  for ( int i = 0; i < FLTT_CASES; ++i ) {
    fltts[ i ] = GC_MALLOC( 2 * sizeof( fault_tree ) );
  }
  fltts[ 0 ][ 0 ] = fltt_wire( 3 );
  fltts[ 1 ][ 0 ] = fltt_and( fltts[ 0 ][ 0 ], fltts[ 0 ][ 0 ] );
  fltts[ 2 ][ 0 ] = fltt_or( fltts[ 0 ][ 0 ], fltts[ 1 ][ 0 ] );
  fltts[ 3 ][ 0 ] = fltt_pand( fltts[ 1 ][ 0 ], fltts[ 2 ][ 0 ] );
  for ( int i = 0; i < FLTT_CASES; ++i )
    fltts[ i ][ 1 ] = fltt_copy( fltts[ i ][ 0 ] );
  printf( "Strating fltt_copy tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < FLTT_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            FLTT_TEST_COPY_BATTERIES_NUMBER * FLTT_CASES );
    if ( fltt_equal( fltts[ i ][ 0 ], fltts[ i ][ 1 ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
    }
    printf_reset();
    printf( "Test %d/%d: ", 2 * i + 2,
            FLTT_TEST_COPY_BATTERIES_NUMBER * FLTT_CASES );
    if ( fltts[ i ][ 0 ] == fltts[ i ][ 1 ] ) {
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
  if ( res == FLTT_TEST_COPY_BATTERIES_NUMBER * FLTT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FLTT_TEST_COPY_BATTERIES_NUMBER * FLTT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int fault_tree_test_snprintf() {
  int res = 0;
  char* s;
  // testing each basic fault_tree type
  fault_tree* fltts = GC_MALLOC( FLTT_CASES * sizeof( fault_tree ) );
  // array of answers
  char** strings = GC_MALLOC( FLTT_CASES * sizeof( char* ) );
  fltts[ 0 ] = fltt_wire( 3 );
  strings[ 0 ] = "3";
  fltts[ 1 ] = fltt_and( fltts[ 0 ], fltts[ 0 ] );
  strings[ 1 ] = "AND( 3, 3 )";
  fltts[ 2 ] = fltt_or( fltts[ 0 ], fltts[ 1 ] );
  strings[ 2 ] = "OR( 3, AND( 3, 3 ) )";
  fltts[ 3 ] = fltt_pand( fltts[ 1 ], fltts[ 2 ] );
  strings[ 3 ] = "PAND( AND( 3, 3 ), OR( 3, AND( 3, 3 ) ) )";
  printf( "Strating fltt_snprintf tests.\n" );
  printf( "============================\n" );
  for ( int i = 0; i < FLTT_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            FLTT_TEST_SNPRINTF_BATTERIES_NUMBER * FLTT_CASES );
    int l = fltt_snprintf( NULL, 0, fltts[ i ] );
    s = GC_MALLOC( (l + 1) * sizeof( char ) );
    fltt_snprintf( s, l + 1, fltts[ i ] );
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
  if ( res == FLTT_TEST_SNPRINTF_BATTERIES_NUMBER * FLTT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FLTT_TEST_SNPRINTF_BATTERIES_NUMBER * FLTT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "============================\n" );
  return res;
}

int fault_tree_test_propagate() {
  int res = 0;
  bool** sigma = GC_MALLOC( 2 * sizeof( bool* ) );
  for ( int i = 0; i < 2; ++i ) sigma[ i ] = GC_MALLOC( 3 * sizeof( bool ) );
  sigma[ 0 ][ 0 ] = true;
  sigma[ 0 ][ 1 ] = false;
  sigma[ 0 ][ 2 ] = false;
  sigma[ 1 ][ 0 ] = true;
  sigma[ 1 ][ 1 ] = true;
  sigma[ 1 ][ 2 ] = false;
  // testing each basic fault_tree type
  fault_tree* fltts = GC_MALLOC( FLTT_CASES * sizeof( fault_tree ) );
  // array of answers
  bool** propagate = GC_MALLOC( FLTT_CASES * sizeof( bool* ) );
  for ( int i = 0; i < FLTT_CASES; ++i )
    propagate[ i ] = GC_MALLOC( 2 * sizeof( bool ) );
  fltts[ 0 ] = fltt_wire( 0 );
  propagate[ 0 ][ 0 ] = true;
  propagate[ 0 ][ 1 ] = true;
  fltts[ 1 ] = fltt_and( fltts[ 0 ], fltt_wire( 1 ) );
  propagate[ 1 ][ 0 ] = false;
  propagate[ 1 ][ 1 ] = true;
  fltts[ 2 ] = fltt_or( fltts[ 1 ], fltt_wire( 2 ) );
  propagate[ 2 ][ 0 ] = false;
  propagate[ 2 ][ 1 ] = true;
  fltts[ 3 ] = fltt_pand( fltt_wire( 0 ), fltts[ 2 ] );
  propagate[ 3 ][ 0 ] = false;
  propagate[ 3 ][ 1 ] = true;
  printf( "Strating fltt_propagate tests.\n" );
  printf( "========================\n" );
  for ( int i = 0; i < FLTT_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            FLTT_TEST_PROPAGATE_BATTERIES_NUMBER * FLTT_CASES );
    if ( fltt_propagate( fltts[ i ], sigma[ 0 ] ) == propagate[ i ][ 0 ] ) {
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
            FLTT_TEST_PROPAGATE_BATTERIES_NUMBER * FLTT_CASES );
    if ( fltt_propagate( fltts[ i ], sigma[ 1 ] ) == propagate[ i ][ 1 ] ) {
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
  if ( res == FLTT_TEST_PROPAGATE_BATTERIES_NUMBER * FLTT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FLTT_TEST_PROPAGATE_BATTERIES_NUMBER * FLTT_CASES );
  printf_reset();
  printf( ".\n" );
  return res;
}

int fault_tree_test_prf() {
  int res = 0;
  // defining the logic
  enum TRUTH_VALUE neg_table[ TRUTH_VALUE_CASES ] = { TRUE, UNDET, FALSE };
  enum TRUTH_VALUE imp_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { TRUE, TRUE, TRUE, UNDET, UNDET, TRUE, FALSE, UNDET, TRUE };
  enum TRUTH_VALUE disj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, UNDET, TRUE, UNDET, UNDET, TRUE, TRUE, TRUE, TRUE };
  enum TRUTH_VALUE conj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, FALSE, FALSE, FALSE, UNDET, UNDET, FALSE, UNDET, TRUE };
  logical_operator neg  = table_to_lo( 1, neg_table );
  logical_operator imp  = table_to_lo( 2, imp_table );
  logical_operator disj = table_to_lo( 2, disj_table );
  logical_operator conj = table_to_lo( 2, conj_table );
  logic log = los_to_log( neg, imp, disj, conj );
  // testing each basic fault_tree type
  fault_tree* fltts = GC_MALLOC( FLTT_CASES * sizeof( fault_tree ) );
  // array of answers
  proof* prfs = GC_MALLOC( FLTT_CASES * sizeof( proof ) );
  formula* fmls = GC_MALLOC( 3 * sizeof( formula ) );
  fmls[ 0 ] = fml_pvar( 0, "0" );
  fmls[ 1 ] = fml_pvar( 1, "1" );
  fmls[ 2 ] = fml_pvar( 2, "2" );
  expression* pos_cfds = GC_MALLOC( 3 * sizeof( expression ) );
  pos_cfds[ 0 ] = exp_var( 0, "0+" );
  pos_cfds[ 1 ] = exp_var( 2, "1+" );
  pos_cfds[ 2 ] = exp_var( 4, "2+" );
  expression* neg_cfds = GC_MALLOC( 3 * sizeof( expression ) );
  neg_cfds[ 0 ] = exp_var( 1, "0-" );
  neg_cfds[ 1 ] = exp_var( 3, "1-" );
  neg_cfds[ 2 ] = exp_var( 5, "2-" );
  fltts[ 0 ] = fltt_wire( 0 );
  prfs[ 0 ] = prf_axiom( 3, fmls, pos_cfds, neg_cfds, 0 );
  fltts[ 1 ] = fltt_and( fltts[ 0 ], fltt_wire( 1 ) );
  prfs[ 1 ] = prf_disj_i( prfs[ 0 ],
                          prf_axiom( 3, fmls, pos_cfds, neg_cfds, 1 ), log );
  fltts[ 2 ] = fltt_or( fltts[ 1 ], fltt_wire( 2 ) );
  prfs[ 2 ] = prf_conj_i( prfs[ 1 ],
                          prf_axiom( 3, fmls, pos_cfds, neg_cfds, 2 ), log );
  fltts[ 3 ] = fltt_pand( fltt_wire( 0 ), fltts[ 2 ] );
  prfs[ 3 ] = prf_disj_i( prf_axiom( 3, fmls, pos_cfds, neg_cfds, 0 ),
                          prfs[ 2 ], log );
  printf( "Strating fltt_to_prf tests.\n" );
  printf( "===========================\n" );
  for ( int i = 0; i < FLTT_CASES; ++i ) {
    printf( "Test %d/%d: ", i,
            FLTT_TEST_PRF_BATTERIES_NUMBER * FLTT_CASES );
    if ( prf_equal( fltt_to_prf( 3, fltts[ i ], log ), prfs[ i ] ) ) {
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
  if ( res == FLTT_TEST_PRF_BATTERIES_NUMBER * FLTT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, FLTT_TEST_PRF_BATTERIES_NUMBER * FLTT_CASES );
  printf_reset();
  printf( ".\n" );
  return res;
}

void fault_tree_test() {
  int batteries = FLTT_TEST_EQUAL_BATTERIES_NUMBER +
                  FLTT_TEST_COPY_BATTERIES_NUMBER +
                  FLTT_TEST_SNPRINTF_BATTERIES_NUMBER +
                  FLTT_TEST_PROPAGATE_BATTERIES_NUMBER +
                  FLTT_TEST_PRF_BATTERIES_NUMBER;
  int n = fault_tree_test_equal();
  n += fault_tree_test_copy();
  n += fault_tree_test_snprintf();
  n += fault_tree_test_propagate();
  n += fault_tree_test_prf();
  printf( "===================\n" );
  printf( "Total tests passed: " );
  if ( n == batteries * FLTT_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", n, batteries * FLTT_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "===================\n" );
}
