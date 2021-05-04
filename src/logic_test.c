/********************************************************************
 * logic_test.c
 *
 * Defines a battery of tests for logics.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdio.h>
#include <string.h>
#include <gc.h>
#include "utils.h"
#include "logic.h"
#include "logic_test.h"

/*
 * Logical operators
 */

int logical_operator_test_equal() {
  int res = 0;
  // testing each logical operator type
  enum TRUTH_VALUE*** tables = GC_MALLOC( LO_CASES *
                                          sizeof( enum TRUTH_VALUE** ) );
  logical_operator** los = GC_MALLOC( LO_CASES * sizeof( logical_operator* ) );
  int n = 1;
  for ( int i = 0; i < LO_CASES; ++i ) {
    tables[ i ] = GC_MALLOC( 3 * sizeof( enum TRUTH_VALUE* ) );
    los[ i ] = GC_MALLOC( 3 * sizeof( logical_operator ) );
    n = int_pow( TRUTH_VALUE_CASES, i );
    for ( int j = 0; j < 3; ++j ) {
      tables[ i ][ j ] = GC_MALLOC( n * sizeof( enum TRUTH_VALUE ) );
      for ( int k = 0; k < n; ++k )
        tables[ i ][ j ][ k ] = int_to_truth_value( k % TRUTH_VALUE_CASES );
      if ( j == 1 )
        tables[ i ][ j ][ n - 1 ] = int_to_truth_value( n % TRUTH_VALUE_CASES );
      los[ i ][ j ] = table_to_lo( i, tables[ i ][ j ] );
    }
  }
  printf( "Strating lo_equal tests.\n" );
  printf( "=========================\n" );
  for ( int i = 0; i < LO_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            LO_TEST_EQUAL_BATTERIES_NUMBER * LO_CASES );
    if ( lo_equal( los[ i ][ 0 ], los[ i ][ 1 ] ) ) {
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
            LO_TEST_EQUAL_BATTERIES_NUMBER * LO_CASES );
    if ( lo_equal( los[ i ][ 0 ], los[ i ][ 2 ] ) ) {
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
  if ( res == LO_TEST_EQUAL_BATTERIES_NUMBER * LO_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, LO_TEST_EQUAL_BATTERIES_NUMBER * LO_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int logical_operator_test_copy() {
  int res = 0;
  // testing each logical operator type
  int n = int_pow( TRUTH_VALUE_CASES, LO_CASES - 1 );
  enum TRUTH_VALUE* table = GC_MALLOC( n * sizeof( enum TRUTH_VALUE ) );
  logical_operator** los = GC_MALLOC( LO_CASES * sizeof( logical_operator* ) );
  for ( int i = 0; i < n; ++i )
    table[ i ] = int_to_truth_value( i % TRUTH_VALUE_CASES );
  for ( int i = 0; i < LO_CASES; ++i ) {
    los[ i ] = GC_MALLOC( 2 * sizeof( logical_operator ) );
    los[ i ][ 0 ] = table_to_lo( i, table );
    los[ i ][ 1 ] = lo_copy( los[ i ][ 0 ] );
  }
  printf( "Strating lo_copy tests.\n" );
  printf( "========================\n" );
  for ( int i = 0; i < LO_CASES; ++i ) {
    printf( "Test %d/%d: ", 2 * i + 1,
            LO_TEST_COPY_BATTERIES_NUMBER * LO_CASES );
    if ( lo_equal( los[ i ][ 0 ], los[ i ][ 1 ] ) ) {
      ++res;
      printf_green();
      printf( "passed.\n" );
    } else {
      printf_bold_red();
      printf( "FAILED!\n" );
    }
    printf_reset();
    printf( "Test %d/%d: ", 2 * i + 2,
            LO_TEST_COPY_BATTERIES_NUMBER * LO_CASES );
    if ( los[ i ][ 0 ] == los[ i ][ 1 ] ) {
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
  if ( res == LO_TEST_COPY_BATTERIES_NUMBER * LO_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, LO_TEST_COPY_BATTERIES_NUMBER * LO_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "========================\n" );
  return res;
}

int logical_operator_test_snprintf() {
  int res = 0;
  char* s;
  // testing each logical operator type
  enum TRUTH_VALUE* table = GC_MALLOC( LO_CASES * sizeof( enum TRUTH_VALUE ) );
  logical_operator* los = GC_MALLOC( LO_CASES * sizeof( logical_operator* ) );
  char** strings = GC_MALLOC( LO_CASES * sizeof( char* ) );
  int n = int_pow( TRUTH_VALUE_CASES, LO_CASES - 1 );
  for ( int i = 0; i < n; ++i ) {
    table[ i ] = int_to_truth_value( i % TRUTH_VALUE_CASES );
  }
  for ( int i = 0; i < LO_CASES; ++i )
    los[ i ] = table_to_lo( i, table );
  strings[ 0 ] = "F";
  strings[ 1 ] = "F |-> F\n"
                 "U |-> U\n"
                 "T |-> T";
  strings[ 2 ] = "  F U T\n"
                 "F F U T\n"
                 "U F U T\n"
                 "T F U T";
  strings[ 3 ] = "F, F, F |-> F\n"
                 "F, F, U |-> U\n"
                 "F, F, T |-> T\n"
                 "F, U, F |-> F\n"
                 "F, U, U |-> U\n"
                 "F, U, T |-> T\n"
                 "F, T, F |-> F\n"
                 "F, T, U |-> U\n"
                 "F, T, T |-> T\n"
                 "U, F, F |-> F\n"
                 "U, F, U |-> U\n"
                 "U, F, T |-> T\n"
                 "U, U, F |-> F\n"
                 "U, U, U |-> U\n"
                 "U, U, T |-> T\n"
                 "U, T, F |-> F\n"
                 "U, T, U |-> U\n"
                 "U, T, T |-> T\n"
                 "T, F, F |-> F\n"
                 "T, F, U |-> U\n"
                 "T, F, T |-> T\n"
                 "T, U, F |-> F\n"
                 "T, U, U |-> U\n"
                 "T, U, T |-> T\n"
                 "T, T, F |-> F\n"
                 "T, T, U |-> U\n"
                 "T, T, T |-> T";
  printf( "Strating lo_snprintf tests.\n" );
  printf( "============================\n" );
  for ( int i = 0; i < LO_CASES; ++i ) {
    printf( "Test %d/%d: ", i + 1,
            LO_TEST_SNPRINTF_BATTERIES_NUMBER * LO_CASES );
    int l = lo_snprintf( NULL, 0, los[ i ] );
    s = GC_MALLOC( (l + 1) * sizeof( char ) );
    lo_snprintf( s, l + 1, los[ i ] );
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
  if ( res == LO_TEST_SNPRINTF_BATTERIES_NUMBER * LO_CASES )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, LO_TEST_SNPRINTF_BATTERIES_NUMBER * LO_CASES );
  printf_reset();
  printf( ".\n" );
  printf( "============================\n" );
  return res;
}

/*
 * Logics
 */

int logic_test_equal() {
  int res = 0;
  enum TRUTH_VALUE neg_table1[ TRUTH_VALUE_CASES ] = { TRUE, UNDET, FALSE };
  enum TRUTH_VALUE neg_table2[ TRUTH_VALUE_CASES ] = { TRUE, FALSE, FALSE };
  enum TRUTH_VALUE imp_table1[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { TRUE, TRUE, TRUE, UNDET, UNDET, TRUE,  FALSE, UNDET, TRUE };
  enum TRUTH_VALUE imp_table2[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { TRUE, TRUE, TRUE, UNDET, UNDET, UNDET, FALSE, UNDET, TRUE };
  enum TRUTH_VALUE disj_table1[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, UNDET, TRUE, UNDET, UNDET, TRUE, TRUE, TRUE, TRUE };
  enum TRUTH_VALUE disj_table2[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, UNDET, TRUE, UNDET, TRUE,  TRUE, TRUE, TRUE, TRUE };
  enum TRUTH_VALUE conj_table1[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, FALSE, FALSE, FALSE, UNDET, UNDET, FALSE, UNDET, TRUE };
  enum TRUTH_VALUE conj_table2[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, FALSE, FALSE, TRUE,  UNDET, UNDET, FALSE, UNDET, TRUE };
  enum TRUTH_VALUE** neg_tables  = GC_MALLOC( 3 * sizeof( enum TRUTH_VALUE* ) );
  enum TRUTH_VALUE** imp_tables  = GC_MALLOC( 3 * sizeof( enum TRUTH_VALUE* ) );
  enum TRUTH_VALUE** disj_tables = GC_MALLOC( 3 * sizeof( enum TRUTH_VALUE* ) );
  enum TRUTH_VALUE** conj_tables = GC_MALLOC( 3 * sizeof( enum TRUTH_VALUE* ) );
  for ( int i = 0; i < 3; ++i ) {
    neg_tables[ i ] = GC_MALLOC( TRUTH_VALUE_CASES *
                                 sizeof( enum TRUTH_VALUE ) );
    imp_tables[ i ] = GC_MALLOC( TRUTH_VALUE_CASES * TRUTH_VALUE_CASES *
                                 sizeof( enum TRUTH_VALUE ) );
    disj_tables[ i ] = GC_MALLOC( TRUTH_VALUE_CASES * TRUTH_VALUE_CASES *
                                 sizeof( enum TRUTH_VALUE ) );
    conj_tables[ i ] = GC_MALLOC( TRUTH_VALUE_CASES * TRUTH_VALUE_CASES *
                                 sizeof( enum TRUTH_VALUE ) );
  }
  logical_operator* negs  = GC_MALLOC( 3 * sizeof( logical_operator ) );
  logical_operator* imps  = GC_MALLOC( 3 * sizeof( logical_operator ) );
  logical_operator* disjs = GC_MALLOC( 3 * sizeof( logical_operator ) );
  logical_operator* conjs = GC_MALLOC( 3 * sizeof( logical_operator ) );
  logic* logs = GC_MALLOC( 3 * sizeof( logic ) );
  for ( int i = 0; i < TRUTH_VALUE_CASES; ++i ) {
    neg_tables[ 0 ][ i ] = neg_table1[ i ];
    neg_tables[ 1 ][ i ] = neg_table2[ i ];
    neg_tables[ 2 ][ i ] = neg_table1[ i ];
  }
  for ( int i = 0; i < TRUTH_VALUE_CASES * TRUTH_VALUE_CASES; ++i ) {
    imp_tables[ 0 ][ i ]  = imp_table1[ i ];
    imp_tables[ 1 ][ i ]  = imp_table2[ i ];
    imp_tables[ 2 ][ i ]  = imp_table1[ i ];
    disj_tables[ 0 ][ i ] = disj_table1[ i ];
    disj_tables[ 1 ][ i ] = disj_table2[ i ];
    disj_tables[ 2 ][ i ] = disj_table1[ i ];
    conj_tables[ 0 ][ i ] = conj_table1[ i ];
    conj_tables[ 1 ][ i ] = conj_table2[ i ];
    conj_tables[ 2 ][ i ] = conj_table1[ i ];
  }
  for ( int i = 0; i < 3; ++i ) {
    negs[ i ]  = table_to_lo( 1, neg_tables[ i ] );
    imps[ i ]  = table_to_lo( 2, imp_tables[ i ] );
    disjs[ i ] = table_to_lo( 2, disj_tables[ i ] );
    conjs[ i ] = table_to_lo( 2, conj_tables[ i ] );
    logs[ i ] = los_to_log( negs[ i ], imps[ i ], disjs[ i ], conjs[ i ] );
  }
  printf( "Strating log_equal tests.\n" );
  printf( "=========================\n" );
  printf( "Test %d/%d: ", 2 * 0 + 1, LOG_TEST_EQUAL_BATTERIES_NUMBER );
  if ( log_equal( logs[ 0 ], logs[ 1 ] ) ) {
    printf_bold_red();
    printf( "FAILED!\n" );
    printf_reset();
  } else {
    ++res;
    printf_green();
    printf( "passed.\n" );
    printf_reset();
  }
  printf( "Test %d/%d: ", 2 * 0 + 2, LOG_TEST_EQUAL_BATTERIES_NUMBER );
  if ( log_equal( logs[ 0 ], logs[ 2 ] ) ) {
    ++res;
    printf_green();
    printf( "passed.\n" );
    printf_reset();
  } else {
    printf_bold_red();
    printf( "FAILED!\n" );
    printf_reset();
  }
  printf( "=========================\n" );
  printf( "Passed tests: " );
  if ( res == LO_TEST_EQUAL_BATTERIES_NUMBER )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, LO_TEST_EQUAL_BATTERIES_NUMBER );
  printf_reset();
  printf( ".\n" );
  printf( "=========================\n" );
  return res;
}

int logic_test_copy() {
  int res = 0;
  enum TRUTH_VALUE neg_table[ TRUTH_VALUE_CASES ] = { TRUE, UNDET, FALSE };
  logical_operator neg = table_to_lo( 1, neg_table );
  enum TRUTH_VALUE imp_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { TRUE, TRUE, TRUE, UNDET, UNDET, TRUE, FALSE, UNDET, TRUE };
  logical_operator imp = table_to_lo( 2, imp_table );
  enum TRUTH_VALUE disj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, UNDET, TRUE, UNDET, UNDET, TRUE, TRUE, TRUE, TRUE };
  logical_operator disj = table_to_lo( 2, disj_table );
  enum TRUTH_VALUE conj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, FALSE, FALSE, FALSE, UNDET, UNDET, FALSE, UNDET, TRUE };
  logical_operator conj = table_to_lo( 2, conj_table );
  logic* logs = GC_MALLOC( 2 * sizeof( logic ) );
  logs[ 0 ] = los_to_log( neg, imp, disj, conj );
  logs[ 1 ] = log_copy( logs[ 0 ] );
  printf( "Strating log_copy tests.\n" );
  printf( "========================\n" );
  printf( "Test %d/%d: ", 2 * 0 + 1, LOG_TEST_COPY_BATTERIES_NUMBER );
  if ( log_equal( logs[ 0 ], logs[ 1 ] ) ) {
    ++res;
    printf_green();
    printf( "passed.\n" );
  } else {
    printf_bold_red();
    printf( "FAILED!\n" );
  }
  printf_reset();
  printf( "Test %d/%d: ", 2 * 0 + 2, LOG_TEST_COPY_BATTERIES_NUMBER );
  if ( logs[ 0 ] == logs[ 1 ] ) {
    printf_bold_red();
    printf( "FAILED!\n" );
  } else {
    ++res;
    printf_green();
    printf( "passed.\n" );
  }
  printf_reset();
  printf( "========================\n" );
  printf( "Passed tests: " );
  if ( res == LOG_TEST_COPY_BATTERIES_NUMBER )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, LOG_TEST_COPY_BATTERIES_NUMBER );
  printf_reset();
  printf( ".\n" );
  printf( "========================\n" );
  return res;
}

int logic_test_snprintf() {
  int res = 0;
  char* s;
  enum TRUTH_VALUE neg_table[ TRUTH_VALUE_CASES ] = { TRUE, UNDET, FALSE };
  logical_operator neg = table_to_lo( 1, neg_table );
  enum TRUTH_VALUE imp_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { TRUE, TRUE, TRUE, UNDET, UNDET, TRUE, FALSE, UNDET, TRUE };
  logical_operator imp = table_to_lo( 2, imp_table );
  enum TRUTH_VALUE disj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, UNDET, TRUE, UNDET, UNDET, TRUE, TRUE, TRUE, TRUE };
  logical_operator disj = table_to_lo( 2, disj_table );
  enum TRUTH_VALUE conj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
    { FALSE, FALSE, FALSE, FALSE, UNDET, UNDET, FALSE, UNDET, TRUE };
  logical_operator conj = table_to_lo( 2, conj_table );
  logic log = los_to_log( neg, imp, disj, conj );
  char* string = "neg:\n"
                 "F |-> T\n"
                 "U |-> U\n"
                 "T |-> F\n"
                 "imp:\n"
                 "  F U T\n"
                 "F T T T\n"
                 "U U U T\n"
                 "T F U T\n"
                 "disj:\n"
                 "  F U T\n"
                 "F F U T\n"
                 "U U U T\n"
                 "T T T T\n"
                 "conj:\n"
                 "  F U T\n"
                 "F F F F\n"
                 "U F U U\n"
                 "T F U T";
  printf( "Strating log_snprintf tests.\n" );
  printf( "============================\n" );
  printf( "Test %d/%d: ", 1, LOG_TEST_SNPRINTF_BATTERIES_NUMBER );
  int l = log_snprintf( NULL, 0, log );
  s = GC_MALLOC( (l + 1) * sizeof( char ) );
  log_snprintf( s, l + 1, log );
  if ( strcmp( s, string ) == 0 ) {
    ++res;
    printf_green();
    printf( "passed.\n" );
    printf_reset();
  } else {
    printf_bold_red();
    printf( "FAILED!\n" );
    printf_reset();
  }
  printf( "============================\n" );
  printf( "Passed tests: " );
  if ( res == LOG_TEST_SNPRINTF_BATTERIES_NUMBER )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", res, LOG_TEST_SNPRINTF_BATTERIES_NUMBER );
  printf_reset();
  printf( ".\n" );
  printf( "============================\n" );
  return res;
}

void logic_test() {
  int batteries = LO_CASES * LO_TEST_EQUAL_BATTERIES_NUMBER +
                  LO_CASES * LO_TEST_COPY_BATTERIES_NUMBER +
                  LO_CASES * LO_TEST_SNPRINTF_BATTERIES_NUMBER +
                  LOG_TEST_EQUAL_BATTERIES_NUMBER +
                  LOG_TEST_COPY_BATTERIES_NUMBER +
                  LOG_TEST_SNPRINTF_BATTERIES_NUMBER;
  int n = logical_operator_test_equal();
  n += logical_operator_test_copy();
  n += logical_operator_test_snprintf();
  n += logic_test_equal();
  n += logic_test_copy();
  n += logic_test_snprintf();
  printf( "===================\n" );
  printf( "Total tests passed: " );
  if ( n == batteries )
    printf_bold_green();
  else
    printf_bold_red();
  printf( "%d/%d", n, batteries );
  printf_reset();
  printf( ".\n" );
  printf( "===================\n" );
}
