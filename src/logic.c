/********************************************************************
 * logic.c
 *
 * Defines functions to manipulate logical operators.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <gc.h>
#include "utils.h"
#include "logic.h"

/*
 * Creation
 */

logical_operator table_to_lo( int arity, enum TRUTH_VALUE* truth_table ) {
  logical_operator res = GC_MALLOC( sizeof( struct logical_operator_s ) );
  res->arity = arity;
  int n = int_pow( TRUTH_VALUE_CASES, arity );
  res->truth_table = GC_MALLOC( n * sizeof( enum TRUTH_VALUE ) );
  for ( int i = 0; i < n; ++i )
    res->truth_table[ i ] = truth_table[ i ];
  return res;
}

logic los_to_log( logical_operator neg, logical_operator imp,
                  logical_operator disj, logical_operator conj ) {
  logic res = GC_MALLOC( sizeof( struct logic_s ) );
  res->neg = neg;
  res->imp = imp;
  res->disj = disj;
  res->conj = conj;
  return res;
}

/*
 * Manipulation
 */

enum TRUTH_VALUE int_to_truth_value( int n ) {
  if ( n >= TRUTH_VALUE_CASES ) {
    fprintf( stderr, "int_to_truth_value: no corresponding truth value.\n" );
    exit( 1 );
  }
  switch ( n ) {
    case 0:
      return FALSE;
      break;
    case 1:
      return UNDET;
      break;
    case 2:
      return TRUE;
      break;
    default:
      fprintf( stderr, "int_to_truth_value: mismatch with TRUTH_VALUE_CASES.\n" );
      exit( 1 );
      break;
  }
}

int truth_value_to_int( enum TRUTH_VALUE tv ) {
  switch ( tv ) {
    case FALSE:
      return 0;
      break;
    case UNDET:
      return 1;
      break;
    case TRUE:
      return 2;
      break;
    default:
      fprintf( stderr, "truth_value_to_int: unknown truth value.\n" );
      exit( 1 );
      break;
  }
}

logical_operator lo_copy( logical_operator lo ) {
  return table_to_lo( lo->arity, lo->truth_table );
}

int lo_inverse_image( enum TRUTH_VALUE** buf, logical_operator o,
                      enum TRUTH_VALUE tv ) {
  int res = 0;
  int n = int_pow( TRUTH_VALUE_CASES, o->arity );
  enum TRUTH_VALUE* input = GC_MALLOC( o->arity * sizeof( enum TRUTH_VALUE ) );
  for ( int i = 0; i < o->arity; ++i )
    input[ i ] = int_to_truth_value( 0 );
  bool cont = true;
  int k = 0;
  while ( cont ) {
    // check whether the current input is the last one
    cont = k < n - 1;
    // add the input to the inverse image if its image is [tv]
    if ( o->truth_table[ k ] == tv ) {
      buf[ res ] = GC_MALLOC( o->arity * sizeof( enum TRUTH_VALUE ) );
      for ( int j = 0; j < o->arity; ++j ) {
        buf[ res ][ j ] = input[ j ];
      }
      ++res;
    }
    // go to the next input
    int i = o->arity - 1;
    bool carry = true;
    while ( i >= 0 && carry ) {
      input[ i ] =
        int_to_truth_value( ( truth_value_to_int( input[ i ] ) + 1 ) %
                            TRUTH_VALUE_CASES );
      if ( truth_value_to_int( input[ i ] ) > 0 )
        carry = false;
      --i;
    }
    ++k;
  }
  return res;
}

bool lo_equal( logical_operator lo1, logical_operator lo2 ) {
  if ( lo1->arity != lo2->arity )
    return false;
  int n = int_pow( TRUTH_VALUE_CASES, lo1->arity );
  for ( int i = 0; i < n; ++i )
    if ( lo1->truth_table[ i ] != lo2->truth_table[ i ] )
      return false;
  return true;
}

logic log_copy( logic log ) {
  return los_to_log( log->neg, log->imp, log->disj, log->conj );
}

bool log_equal( logic log1, logic log2 ) {
  return lo_equal( log1->neg,  log2->neg ) &&
         lo_equal( log1->imp,  log2->imp ) &&
         lo_equal( log1->disj, log2->disj ) &&
         lo_equal( log1->conj, log2->conj );
}

/*
 * Printing
 */

void tv_printf( enum TRUTH_VALUE tv ) {
  switch ( tv ) {
    case FALSE:
      printf( "F" );
      break;
    case UNDET:
      printf( "U" );
      break;
    case TRUE:
      printf( "T" );
      break;
    default:
      fprintf( stderr, "tv_printf: Unknown truth value.\n" );
      exit( 1 );
      break;
  }
}

int tv_snprintf( char* buf, int length, enum TRUTH_VALUE tv ) {
  switch ( tv ) {
    case FALSE:
      return snprintf( buf, length, "F" );
      break;
    case UNDET:
      return snprintf( buf, length, "U" );
      break;
    case TRUE:
      return snprintf( buf, length, "T" );
      break;
    default:
      printf( "tv: %d\n", tv );
      fprintf( stderr, "tv_snprintf: Unknown truth value.\n" );
      exit( 1 );
      break;
  }
}

void lo_printf( logical_operator o ) {
  char* buf = NULL;
  int n = lo_snprintf( buf, 0, o );
  buf = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
  lo_snprintf( buf, n, o );
  printf( "%s", buf );
}

int lo_snprintf( char* buf, int length, logical_operator o ) {
  int total = 0, diff = 0;
  enum TRUTH_VALUE* input;
  switch ( o->arity ) {
    case 0:
      total = tv_snprintf( buf, length, o->truth_table[ 0 ] );
      break;
    case 2:
      diff = snprintf( buf, length, " " );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      for ( int i = 0; i < TRUTH_VALUE_CASES; ++i ) {
        diff = snprintf( buf, length, " " );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
        diff = tv_snprintf( buf, length, int_to_truth_value( i ) );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
      }
      diff = snprintf( buf, length, "\n" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      for ( int i = 0; i < TRUTH_VALUE_CASES; ++i ) {
        diff = tv_snprintf( buf, length, int_to_truth_value( i ) );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
        for ( int j = 0; j < TRUTH_VALUE_CASES; ++j ) {
          diff = snprintf( buf, length, " " );
          total += diff;
          buf += diff;
          length -= diff;
          if ( length < 0 ) length = 0;
          diff =
            tv_snprintf( buf, length,
                         int_to_truth_value(
                           o->truth_table[ TRUTH_VALUE_CASES * i + j ] ) );
          total += diff;
          buf += diff;
          length -= diff;
          if ( length < 0 ) length = 0;
        }
        if ( i < TRUTH_VALUE_CASES - 1 ) {
          diff = snprintf( buf, length, "\n" );
          total += diff;
          buf += diff;
          length -= diff;
          if ( length < 0 ) length = 0;
        }
      }
      break;
    default:
      input = GC_MALLOC( o->arity * sizeof( enum TRUTH_VALUE ) );
      for ( int i = 0; i < o->arity; ++i )
        input[ i ] = int_to_truth_value( 0 );
      bool cont = true;
      int k = 0;
      while ( cont ) {
        // check whether to continue
        cont = false;
        for ( int i = 0; i < o->arity - 1; ++i ) {
          if ( input[ i ] != int_to_truth_value( TRUTH_VALUE_CASES - 1 ) ) {
            cont = true;
          }
          diff = tv_snprintf( buf, length, input[ i ] );
          total += diff;
          buf += diff;
          length -= diff;
          if ( length < 0 ) length = 0;
          diff = snprintf( buf, length, ", " );
          total += diff;
          buf += diff;
          length -= diff;
          if ( length < 0 ) length = 0;
        }
        if ( input[ o->arity - 1 ] !=
             int_to_truth_value( TRUTH_VALUE_CASES - 1 ) ) {
          cont = true;
        }
        diff = tv_snprintf( buf, length, input[ o->arity - 1 ] );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
        diff = snprintf( buf, length, " |-> " );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
        diff = tv_snprintf( buf, length, o->truth_table[ k ] );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
        if ( cont ) {
          diff = snprintf( buf, length, "\n" );
          total += diff;
          buf += diff;
          length -= diff;
          if ( length < 0 ) length = 0;
        }

        // go to the next input
        int i = o->arity - 1;
        bool carry = true;
        while ( i >= 0 && carry ) {
          input[ i ] =
            int_to_truth_value( ( truth_value_to_int( input[ i ] ) + 1 ) %
                                TRUTH_VALUE_CASES );
          if ( truth_value_to_int( input[ i ] ) > 0 )
            carry = false;
          --i;
        }
        ++k;
      }
      break;
  }
  return total;
}

void log_printf( logic l ) {
  int n = log_snprintf( NULL, 0, l );
  char* s = GC_MALLOC( (n + 1) * sizeof( char ) );
  log_snprintf( s, n + 1, l );
  printf( "%s", s );
}

int log_snprintf( char* buf, int length, logic l ) {
  int total = 0, diff = 0;
  diff = snprintf( buf, length, "neg:\n" );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = lo_snprintf( buf, length, l->neg );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = snprintf( buf, length, "\nimp:\n" );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = lo_snprintf( buf, length, l->imp );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = snprintf( buf, length, "\ndisj:\n" );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = lo_snprintf( buf, length, l->disj );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = snprintf( buf, length, "\nconj:\n" );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = lo_snprintf( buf, length, l->conj );
  total += diff;
  return total;
}
