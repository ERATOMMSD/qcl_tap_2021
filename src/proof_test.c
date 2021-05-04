/********************************************************************
 * proof_test.c
 *
 * Defines a battery of tests for proofs.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <gc.h>
#include "utils.h"
#include "proof.h"
#include "proof_test.h"

// different possible cases for proofs:
// - axiom
// - implication introduction
// - implication elimination
#define PRF_CASES 2
// TODO
// add 

int proof_test_equal() {
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
  // testing each basic proof type
  formula** fmls = GC_MALLOC( 3 * sizeof( formula* ) );
  expression** pos_cfds = GC_MALLOC( 3 * sizeof( expression* ) );
  expression** neg_cfds = GC_MALLOC( 3 * sizeof( expression* ) );
  proof** prfs = GC_MALLOC( PRF_CASES * sizeof( proof* ) );
  for ( int i = 0; i < 3; ++i ) {
    fmls[ i ] = GC_MALLOC( 3 * sizeof( formula ) );
    pos_cfds[ i ] = GC_MALLOC( 3 * sizeof( expression ) );
    neg_cfds[ i ] = GC_MALLOC( 3 * sizeof( expression ) );
  }
  for ( int i = 0; i < PRF_CASES; ++i ) {
    prfs[ i ] = GC_MALLOC( 3 * sizeof( proof ) );
  }
  fmls[ 0 ][ 0 ] = fml_pvar( 0, "a" );
  fmls[ 0 ][ 1 ] = fml_pvar( 2, "c" );
  fmls[ 0 ][ 2 ] = fml_pvar( 0, "a" );
  fmls[ 1 ][ 0 ] = fml_pvar( 1, "b" );
  fmls[ 1 ][ 1 ] = fml_pvar( 3, "d" );
  fmls[ 1 ][ 2 ] = fml_pvar( 1, "b" );
  fmls[ 2 ][ 0 ] = fml_pvar( 0, "a" );
  fmls[ 2 ][ 1 ] = fml_pvar( 2, "c" );
  fmls[ 2 ][ 2 ] = fml_pvar( 0, "a" );
  pos_cfds[ 0 ][ 0 ] = exp_var( 0, "a+" );
  pos_cfds[ 0 ][ 1 ] = exp_var( 2, "c+" );
  pos_cfds[ 0 ][ 2 ] = exp_var( 0, "a+" );
  pos_cfds[ 1 ][ 0 ] = exp_var( 1, "b+" );
  pos_cfds[ 1 ][ 1 ] = exp_var( 3, "d+" );
  pos_cfds[ 1 ][ 2 ] = exp_var( 1, "b+" );
  pos_cfds[ 2 ][ 0 ] = exp_var( 0, "a+" );
  pos_cfds[ 2 ][ 1 ] = exp_var( 2, "c+" );
  pos_cfds[ 2 ][ 2 ] = exp_var( 0, "a+" );
  neg_cfds[ 0 ][ 0 ] = exp_var( 0, "a-" );
  neg_cfds[ 0 ][ 1 ] = exp_var( 2, "c-" );
  neg_cfds[ 0 ][ 2 ] = exp_var( 0, "a-" );
  neg_cfds[ 1 ][ 0 ] = exp_var( 1, "b-" );
  neg_cfds[ 1 ][ 1 ] = exp_var( 3, "d-" );
  neg_cfds[ 1 ][ 2 ] = exp_var( 1, "b-" );
  neg_cfds[ 2 ][ 0 ] = exp_var( 0, "a-" );
  neg_cfds[ 2 ][ 1 ] = exp_var( 2, "c-" );
  neg_cfds[ 2 ][ 2 ] = exp_var( 0, "a-" );
  for ( int i = 0; i < 3; ++i ) {
    prfs[ 0 ][ i ] = prf_axiom( 3, fmls[ i ], pos_cfds[ i ], neg_cfds[ i ], 0 );
    prfs[ 1 ][ i ] = prf_impl_i( prfs[ 0 ][ i ], log, 2 );
    // prfs[ 2 ][ i ] = prf_impl_e( prfs[ 1 ][ i ],
    //                              prf_axiom( 2, fmls[ i ], pos_cfds[ i ],
    //                                         neg_cfds[ i ], 2 ) ); // TODO
  }
  printf( "Strating prf_equal tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < PRF_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            PRF_TEST_EQUAL_BATTERIES_NUMBER * PRF_CASES );
    if ( prf_equal( prfs[ i ][ 0 ], prfs[ i ][ 1 ] ) ) {
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
            PRF_TEST_EQUAL_BATTERIES_NUMBER * PRF_CASES );
    if ( prf_equal( prfs[ i ][ 0 ], prfs[ i ][ 2 ] ) ) {
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
  if ( res == PRF_TEST_EQUAL_BATTERIES_NUMBER * PRF_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, PRF_TEST_EQUAL_BATTERIES_NUMBER * PRF_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int proof_test_copy() {
  int res = 0;
  // defining the logic
  enum TRUTH_VALUE neg_table[ TRUTH_VALUE_CASES ] = { TRUE, UNDET, FALSE };
  enum TRUTH_VALUE imp_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { TRUE, TRUE, TRUE, UNDET, UNDET, TRUE,  FALSE, UNDET, TRUE };
  enum TRUTH_VALUE disj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, UNDET, TRUE, UNDET, UNDET, TRUE, TRUE, TRUE, TRUE };
  enum TRUTH_VALUE conj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, FALSE, FALSE, FALSE, UNDET, UNDET, FALSE, UNDET, TRUE };
  logical_operator neg  = table_to_lo( 1, neg_table );
  logical_operator imp  = table_to_lo( 2, imp_table );
  logical_operator disj = table_to_lo( 2, disj_table );
  logical_operator conj = table_to_lo( 2, conj_table );
  logic log = los_to_log( neg, imp, disj, conj );
  // testing each proof type
  formula* fmls = GC_MALLOC( 3 * sizeof( formula ) );
  expression* pos_cfds = GC_MALLOC( 3 * sizeof( expression ) );
  expression* neg_cfds = GC_MALLOC( 3 * sizeof( expression ) );
  proof** prfs = GC_MALLOC( PRF_CASES * sizeof( proof* ) );
  fmls[ 0 ] = fml_pvar( 0, "a" );
  fmls[ 1 ] = fml_pvar( 2, "b" );
  fmls[ 2 ] = fml_pvar( 0, "a" );
  pos_cfds[ 0 ] = exp_var( 0, "a+" );
  pos_cfds[ 1 ] = exp_var( 2, "b+" );
  pos_cfds[ 2 ] = exp_var( 0, "a+" );
  neg_cfds[ 0 ] = exp_var( 0, "a-" );
  neg_cfds[ 1 ] = exp_var( 2, "b-" );
  neg_cfds[ 2 ] = exp_var( 0, "a-" );
  for ( int i = 0; i < PRF_CASES; ++i )
    prfs[ i ] = GC_MALLOC( 2 * sizeof( proof ) );
  prfs[ 0 ][ 0 ] = prf_axiom( 3, fmls, pos_cfds, neg_cfds, 1 );
  prfs[ 1 ][ 0 ] = prf_impl_i( prfs[ 0 ][ 0 ], log, 2 );
  // prfs[ 2 ][ 0 ] = prf_impl_e( prfs[ 1 ][ 0 ], prf_axiom( 2, fmls, cfds, 0 ) ); // TODO
  for ( int i = 0; i < PRF_CASES; ++i )
    prfs[ i ][ 1 ] = prf_copy( prfs[ i ][ 0 ] );
  printf( "Strating prf_copy tests.\n" );
  printf( "========================\n" );
  for ( int i = 0; i < PRF_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            PRF_TEST_COPY_BATTERIES_NUMBER * PRF_CASES );
    if ( prf_equal( prfs[ i ][ 0 ], prfs[ i ][ 1 ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
    }
    printf_reset();
    printf( "Test %d/%d: ", 2 * i + 2,
            PRF_TEST_COPY_BATTERIES_NUMBER * PRF_CASES );
    if ( prfs[ i ][ 0 ] == prfs[ i ][ 1 ] ) {
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
  if ( res == PRF_TEST_COPY_BATTERIES_NUMBER * PRF_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, PRF_TEST_COPY_BATTERIES_NUMBER * PRF_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "========================\n" );
  return res;
}

int proof_test_snprintf() {
  int res = 0;
  char* s;
  // defining the logic
  enum TRUTH_VALUE neg_table[ TRUTH_VALUE_CASES ] = { TRUE, UNDET, FALSE };
  enum TRUTH_VALUE imp_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { TRUE, TRUE, TRUE, UNDET, UNDET, TRUE,  FALSE, UNDET, TRUE };
  enum TRUTH_VALUE disj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, UNDET, TRUE, UNDET, UNDET, TRUE, TRUE, TRUE, TRUE };
  enum TRUTH_VALUE conj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, FALSE, FALSE, FALSE, UNDET, UNDET, FALSE, UNDET, TRUE };
  logical_operator neg  = table_to_lo( 1, neg_table );
  logical_operator imp  = table_to_lo( 2, imp_table );
  logical_operator disj = table_to_lo( 2, disj_table );
  logical_operator conj = table_to_lo( 2, conj_table );
  logic log = los_to_log( neg, imp, disj, conj );
  // testing each basic proof type
  formula* fmls = GC_MALLOC( 3 * sizeof( formula ) );
  expression* pos_cfds = GC_MALLOC( 3 * sizeof( expression ) );
  expression* neg_cfds = GC_MALLOC( 3 * sizeof( expression ) );
  proof* prfs = GC_MALLOC( PRF_CASES * sizeof( proof ) );
  fmls[ 0 ] = fml_pvar( 0, "a" );
  fmls[ 1 ] = fml_pvar( 2, "b" );
  fmls[ 2 ] = fml_pvar( 0, "a" );
  pos_cfds[ 0 ] = exp_var( 0, "a+" );
  pos_cfds[ 1 ] = exp_var( 2, "b+" );
  pos_cfds[ 2 ] = exp_var( 0, "a+" );
  neg_cfds[ 0 ] = exp_var( 0, "a-" );
  neg_cfds[ 1 ] = exp_var( 2, "b-" );
  neg_cfds[ 2 ] = exp_var( 0, "a-" );
  // array of answers
  char** strings = GC_MALLOC( PRF_CASES * sizeof( char* ) );
  prfs[ 0 ] = prf_axiom( 3, fmls, pos_cfds, neg_cfds, 1 );
  // TODO
  // change back when printing of expression is back to normal
  // strings[ 0 ] = "a : ( a+ , a- ), b : ( b+ , b- ), a : ( a+ , a- ) |- b : ( b+ , b- )";
  strings[ 0 ] = "a : ( x_0 , x_0 ), b : ( x_2 , x_2 ), a : ( x_0 , x_0 ) |- b : ( x_2 , x_2 )";
  prfs[ 1 ] = prf_impl_i( prfs[ 0 ], log, 2 );
  strings[ 1 ] = "[ a : ( x_0 , x_0 ), b : ( x_2 , x_2 ), a : ( x_0 , x_0 ) |- b : ( x_2 , x_2 ) ] |= a : ( x_0 , x_0 ), b : ( x_2 , x_2 ) |- (a) => (b) : ( (((((0.000000) + ((x_0) * (x_2))) + ((x_0) * ((1.000000) - ((x_2) + (x_2))))) + ((x_0) * (x_2))) + (((1.000000) - ((x_0) + (x_0))) * (x_2))) + ((x_0) * (x_2)) , (0.000000) + ((x_0) * (x_2)) )";
  // prfs[ 2 ] = prf_impl_e( prfs[ 1 ], prf_axiom( 2, fmls, cfds, 0 ) ); // TODO
  // strings[ 2 ] = "[ [ a' : a, b' : b, a' : a |- b' : b ] |= a' : a, b' : b |- (a') => (b') : (a) * (b); a' : a, b' : b |- a' : a ] |= a' : a, b' : b |- b' : ((a) * (b)) * (a)";
  printf( "Strating prf_snprintf tests.\n" );
  printf( "============================\n" );
  for ( int i = 0; i < PRF_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            PRF_TEST_SNPRINTF_BATTERIES_NUMBER * PRF_CASES );
    int l = prf_snprintf( NULL, 0, prfs[ i ] );
    s = GC_MALLOC( (l + 1) * sizeof( char ) );
    prf_snprintf( s, l + 1, prfs[ i ] );
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
  if ( res == PRF_TEST_SNPRINTF_BATTERIES_NUMBER * PRF_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, PRF_TEST_SNPRINTF_BATTERIES_NUMBER * PRF_CASES );
  printf_reset();
  printf( ".\n" );
  return res;
}

void proof_test() {
  int batteries = PRF_TEST_EQUAL_BATTERIES_NUMBER +
                  PRF_TEST_COPY_BATTERIES_NUMBER +
                  PRF_TEST_SNPRINTF_BATTERIES_NUMBER;
  int n = 0;
  n += proof_test_equal();
  n += proof_test_copy();
  n += proof_test_snprintf();
  printf( "===================\n" );
  printf( "Total tests passed: " );
  if ( n == batteries * PRF_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", n, batteries * PRF_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "===================\n" );
}
