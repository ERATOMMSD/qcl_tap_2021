/********************************************************************
 * utils.c
 *
 * Defines utility functions.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <gc.h>
#include "utils.h"

/*
 * Integers
 */

int int_pow( int base, int exp ) {
  if ( exp == 0 )
    return 1;
  else if ( exp % 2 == 0 )
    return int_pow( base * base, exp / 2 );
  else
    return base * int_pow( base * base, exp / 2 );
}

int int_length( int n ) {
  if ( n < 0 )       return 1 + int_length( -n );
  else if ( n < 10 ) return 1;
  else               return 1 + int_length( n / 10 );
}

/*
 * Doubles
 */

int dcompare( double x, double y, double tolerance ) {
  double diff = fabs( x - y );
  double largest = fabs( x );
  if ( fabs( y ) > largest ) largest = fabs( y );
  if ( diff <= tolerance * largest * DBL_EPSILON ) return 0;
  else if ( x < y )                                return -1;
  return 1;
}

double l1_norm( int n, double* v ) {
  double res = 0;
  for ( int i = 0; i < n; ++i ) {
    if ( dcompare( v[ i ], 0, 1 ) < 0 ) {
      if ( v[ i ] < 0 )
      fprintf( stderr, "l1_norm: v[ %d ] = %f < 0.\n", i, v[ i ] );
      exit( 1 );
    }
    res += v[ i ];
  }
  return res;
}

void box_muller( double x, double y, double* res ) {
  res[ 0 ] = sqrt( -2 * log( x ) ) * cos( 2 * M_PI * y );
  res[ 1 ] = sqrt( -2 * log( x ) ) * sin( 2 * M_PI * y );
}

/*
 * Arguments
 */

args args_new() {
  args res = GC_MALLOC( sizeof( struct args_s ) );
  res->test = 0;
  res->print_level = BASIC_PRINT_LEVEL;
  res->mode = NULL;
  res->filename = NULL;
  res->filename_optimisation = NULL;
  return res;
}

void args_read( args a, int argc, char** argv ) {
  for ( int i = 1; i < argc; ) {
    if ( strcmp( argv[ i ], "--test" ) == 0 ) {
      if ( i + 1 < argc ) {
        while ( ++i < argc ) {
          int n = strlen( argv[ i ] );
          if ( n > 2 && argv[ i ][ 0 ] == '-' && argv[ i ][ 1 ] == '-' ) {
            if ( a->test == 0 ) // if no test to run yet
              a->test = ( 1 << TEST_INT ) - 1; // execute all tests
            break;
          } else if ( strcmp( argv[ i ], "expression" ) == 0 ) {
            if ( ( a->test >> TEST_EXPRESSION_BIT ) % 2 == 0 )
              a->test += 1 << TEST_EXPRESSION_BIT;
          } else if ( strcmp( argv[ i ], "formula" ) == 0 ) {
            if ( ( a->test >> TEST_FORMULA_BIT ) % 2 == 0 )
              a->test += 1 << TEST_FORMULA_BIT;
          } else if ( strcmp( argv[ i ], "sequent" ) == 0 ) {
            if ( ( a->test >> TEST_SEQUENT_BIT ) % 2 == 0 )
              a->test += 1 << TEST_SEQUENT_BIT;
          } else if ( strcmp( argv[ i ], "proof" ) == 0 ) {
            if ( ( a->test >> TEST_PROOF_BIT ) % 2 == 0 )
              a->test += 1 << TEST_PROOF_BIT;
          } else if ( strcmp( argv[ i ], "fault_tree" ) == 0 ) {
            if ( ( a->test >> TEST_FAULT_TREE_BIT ) % 2 == 0 )
              a->test += 1 << TEST_FAULT_TREE_BIT;
          } else if ( strcmp( argv[ i ], "logic" ) == 0 ) {
            if ( ( a->test >> TEST_LOGIC_BIT ) % 2 == 0 )
              a->test += 1 << TEST_LOGIC_BIT;
          } else if ( strcmp( argv[ i ], "all" ) == 0 )
            a->test = ( 1 << TEST_INT ) - 1;
          else {
            fprintf( stderr, "Unknown test option (%s).\n", argv[ i ] );
            exit( 1 );
          }
        }
      } else {
        ++i;
        a->test = ( 1 << TEST_INT ) - 1; // execute all tests
      }
    } else if ( strcmp( argv[ i ], "--verbosity" ) == 0 ) {
      if ( ++i == argc ) {
        fprintf( stderr, "--verbosity needs an argument" );
        exit( 1 );
      }
      a->print_level = atoi( argv[ i++ ] );
    } else if ( strcmp( argv[ i ], "-f" ) == 0 ||
                strcmp( argv[ i ], "--filename" ) == 0 ) {
      if ( ++i < argc ) {
        a->filename = argv[ i++ ];
      } else {
        fprintf( stderr, "No filename specified.\n" );
        exit( 1 );
      }
    } else if ( strcmp( argv[ i ], "-o" ) == 0 ||
                strcmp( argv[ i ], "--optimisation" ) == 0 ) {
      if ( ++i < argc ) {
        a->filename_optimisation = argv[ i++ ];
      } else {
        fprintf( stderr, "No optimisation filename specified.\n" );
        exit( 1 );
      }
    } else if ( a->mode != NULL ) {
      fprintf( stderr, "Two modes specified (%s and %s).\n", a->mode,
               argv[ i ] );
      exit( 1 );
    } else if ( strcmp( argv[ i ], ARGS_MODE_PROPAGATE_STR ) == 0 ) {
      a->mode = ARGS_MODE_PROPAGATE_STR;
      i++;
    } else if ( strcmp( argv[ i ], ARGS_MODE_SPLITS_STR ) == 0 ) {
      a->mode = ARGS_MODE_SPLITS_STR;
      i++;
    } else if ( strcmp( argv[ i ], ARGS_MODE_BENCHMARK_STR ) == 0 ) {
      a->mode = ARGS_MODE_BENCHMARK_STR;
      i++;
    } else {
      fprintf( stderr, "Unknown mode (%s).\n", argv[ i ] );
      exit( 1 );
    }
  }
}

/*
 * Printing
 */

void printf_green() {
  printf( "\033[0;32m" );
}

void printf_bold_green() {
  printf( "\033[1;32m" );
}

void printf_bold_red() {
  printf( "\033[1;31m" );
}

void printf_reset() {
  printf( "\033[0m" );
}

/*
 * Vectors
 */

double square_norm( int n, double* v ) {
  double res = 0;
  for ( int i = 0; i < n; ++i )
    res += v[ i ] * v[ i ];
  return res;
}
