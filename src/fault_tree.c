/********************************************************************
 * formula.c
 *
 * Defines functions to manipulate formulas.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gc.h>
#include "utils.h"
#include "fault_tree.h"

/*
 * Creation
 */

fault_tree fltt_wire( int i ) {
  fault_tree res = GC_MALLOC( sizeof( struct fault_tree_s ) );
  res->ports = 0;
  res->fault_tree_type = WIRE;
  res->children = NULL;
  res->index = i;
  return res;
}

fault_tree fltt_binary( fault_tree t1, fault_tree t2, enum CASE_FAULT_TREE s ) {
  fault_tree res = GC_MALLOC( sizeof( struct fault_tree_s ) );
  res->fault_tree_type = s;
  res->ports = 2;
  res->children = GC_MALLOC( res->ports * sizeof( fault_tree ) );
  res->children[ 0 ] = t1;
  res->children[ 1 ] = t2;
  res->index = 0;
  return res;
}


fault_tree fltt_and( fault_tree t1, fault_tree t2 ) {
  return fltt_binary( t1, t2, AND );
}

fault_tree fltt_or( fault_tree t1, fault_tree t2 ) {
  return fltt_binary( t1, t2, OR );
}

fault_tree fltt_pand( fault_tree t1, fault_tree t2 ) {
  return fltt_binary( t1, t2, PAND );
}

/*
 * Manipulation
 */

fault_tree fltt_copy( fault_tree t ) {
  fault_tree res = GC_MALLOC( sizeof( struct fault_tree_s ) );
  res->fault_tree_type = t->fault_tree_type;
  res->ports = t->ports;
  res->children = GC_MALLOC( res->ports * sizeof( fault_tree ) );
  for ( int i = 0; i < res->ports; ++i )
    res->children[ i ] = fltt_copy( t->children[ i ] );
  res->index = t->index;
  return res;
}

bool fltt_equal( fault_tree t1, fault_tree t2 ) {
  if ( t1->fault_tree_type != t2->fault_tree_type )
    return false;
  if ( t1->fault_tree_type == WIRE )
    return t1->index == t2->index;
  switch ( t1->fault_tree_type ) {
    // binary cases
    case AND:
    case OR:
    case PAND:
      return fltt_equal( t1->children[ 0 ], t2->children[ 0 ] ) &&
             fltt_equal( t1->children[ 1 ], t2->children[ 1 ] );
      break;
    case WIRE:
      fprintf( stderr, "fltt_equal: Reached bad case." );
      exit( 1 );
      break;
    default:
      fprintf( stderr, "fltt_equal: Unknown fault tree type." );
      exit( 1 );
      break;
  }
}

bool fltt_propagate( fault_tree t, bool* sigma ) {
  switch ( t->fault_tree_type ) {
    case WIRE:
      return sigma[ t->index ];
      break;
    case AND:
    case PAND:
      return fltt_propagate( t->children[ 0 ], sigma ) &&
             fltt_propagate( t->children[ 1 ], sigma );
      break;
    case OR:
      return fltt_propagate( t->children[ 0 ], sigma ) ||
             fltt_propagate( t->children[ 1 ], sigma );
      break;
    default:
      fprintf( stderr, "fltt_eval: Unknown fault tree type." );
      exit( 1 );
      break;
  }
}

double fltt_propagate_prob( fault_tree t, double* sigma ) {
  switch ( t->fault_tree_type ) {
    case WIRE:
      return sigma[ t->index ];
      break;
    case AND:
    case PAND:
      return fltt_propagate_prob( t->children[ 0 ], sigma ) *
             fltt_propagate_prob( t->children[ 1 ], sigma );
      break;
    case OR:
      return fltt_propagate_prob( t->children[ 0 ], sigma ) +
             fltt_propagate_prob( t->children[ 1 ], sigma ) -
             fltt_propagate_prob( t->children[ 0 ], sigma ) *
             fltt_propagate_prob( t->children[ 1 ], sigma );
      break;
    default:
      fprintf( stderr, "fltt_eval: Unknown fault tree type." );
      exit( 1 );
      break;
  }
}

proof fltt_to_prf( int n, fault_tree t, logic log ) {
  formula* hyp_fmls;
  expression* hyp_pos_cfds;
  expression* hyp_neg_cfds;
  char** hyp_bufs;
  char** pos_cfd_bufs;
  char** neg_cfd_bufs;
  switch ( t->fault_tree_type ) {
    case WIRE:
      hyp_bufs = GC_MALLOC( n * sizeof( char* ) );
      pos_cfd_bufs = GC_MALLOC( n * sizeof( char* ) );
      neg_cfd_bufs = GC_MALLOC( n * sizeof( char* ) );
      hyp_fmls = GC_MALLOC( n * sizeof( formula ) );
      hyp_pos_cfds = GC_MALLOC( n * sizeof( expression ) );
      hyp_neg_cfds = GC_MALLOC( n * sizeof( expression ) );
      for ( int i = 0; i < n; ++i ) {
        hyp_bufs[ i ] = GC_MALLOC( ( int_length( i ) + 1 ) * sizeof( char ) );
        pos_cfd_bufs[ i ] = GC_MALLOC( ( int_length( i ) + 2 ) * sizeof( char ) );
        neg_cfd_bufs[ i ] = GC_MALLOC( ( int_length( i ) + 2 ) * sizeof( char ) );
        snprintf( hyp_bufs[ i ], int_length( i ) + 1, "%d", i );
        snprintf( pos_cfd_bufs[ i ], int_length( i ) + 2, "%d+", i );
        snprintf( neg_cfd_bufs[ i ], int_length( i ) + 2, "%d-", i );
        hyp_fmls[ i ] = fml_pvar( i, hyp_bufs[ i ] );
        hyp_pos_cfds[ i ] = exp_var( 2 * i, pos_cfd_bufs[ i ] );
        hyp_neg_cfds[ i ] = exp_var( 2 * i + 1, neg_cfd_bufs[ i ] );
      }
      return prf_axiom( n, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds, t->index );
      break;
    case AND:
      return prf_disj_i( fltt_to_prf( n, t->children[ 0 ], log ),
                         fltt_to_prf( n, t->children[ 1 ], log ),
                         log );
      break;
    case OR:
      return prf_conj_i( fltt_to_prf( n, t->children[ 0 ], log ),
                         fltt_to_prf( n, t->children[ 1 ], log ),
                         log );
      break;
    case PAND:
      return prf_disj_i( fltt_to_prf( n, t->children[ 0 ], log ),
                         fltt_to_prf( n, t->children[ 1 ], log ),
                         log );
      break;
    default:
      fprintf( stderr, "fltt_to_prf: Unknown fault tree type." );
      exit( 1 );
      break;
  }
}

/*
 * Printing
 */

void fltt_printf( fault_tree t ) {
  int n = fltt_snprintf( NULL, 0, t );
  char* s = GC_MALLOC( (n + 1) * sizeof( char ) );
  fltt_snprintf( s, n + 1, t );
  printf( "%s", s );
}

int fltt_symbol_snprintf( char* buf, int length, enum CASE_FAULT_TREE s ) {
  switch ( s ) {
    case AND:
      snprintf( buf, length, "AND" );
      return strlen( "AND" );
      break;
    case OR:
      snprintf( buf, length, "OR" );
      return strlen( "OR" );
      break;
    case PAND:
      snprintf( buf, length, "PAND" );
      return strlen( "PAND" );
      break;
    default:
      fprintf( stderr, "fltt_snprintf: Unknown fault tree type.\n" );
      exit( 1 );
      break;
  }
}

int fltt_snprintf( char* buf, int length, fault_tree t ) {
  int total = 0, diff = 0;
  switch ( t->fault_tree_type ) {
    case WIRE:
      total = snprintf( buf, length, "%d", t->index );
      break;
    default:
      diff = fltt_symbol_snprintf( buf, length, t->fault_tree_type );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, "( " );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      for ( int i = 0; i < t->ports - 1; ++i ) {
        diff = fltt_snprintf( buf, length, t->children[ i ] );
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
      if ( t->ports > 0 ) {
        diff = fltt_snprintf( buf, length, t->children[ t->ports - 1 ] );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
        diff = snprintf( buf, length, " " );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
      }
      diff = snprintf( buf, length, ")" );
      total += diff;
      break;
  }
  return total;
}

enum CASE_FAULT_TREE fltt_type_lexer( char* token ) {
  if      ( strcmp( token, "wire" ) == 0 ) return WIRE;
  else if ( strcmp( token, "and" )  == 0 ) return AND;
  else if ( strcmp( token, "or" )   == 0 ) return OR;
  else if ( strcmp( token, "pand" ) == 0 ) return PAND;
  fprintf( stderr, "fltt_type_lexer: Unknown token %s.\n", token );
  exit( 1 );
}

fault_tree cJSON_to_fltt( cJSON* json ) {
  cJSON* json1 = cJSON_GetObjectItemCaseSensitive( json, "type" );
  if ( json1 == NULL ) {
    fprintf( stderr, "cJSON_to_fltt: no \"type\" specified in fault tree %s\n",
             cJSON_Print( json ) );
    exit( 1 );
  }
  char* type;
  if ( cJSON_IsString( json1 ) ) {
    type = json1->valuestring;
  } else {
    fprintf( stderr, "cJSON_to_fltt: \"type\" is not a string in fault tree "
             "%s\n", cJSON_Print( json ) );
    exit( 1 );
  }
  enum CASE_FAULT_TREE fltt_type = fltt_type_lexer( type );
  int index;
  cJSON* json2;
  switch ( fltt_type ) {
    case WIRE:
      json1 = cJSON_GetObjectItemCaseSensitive( json, "index" );
      if ( json1 == NULL ) {
        fprintf( stderr, "cJSON_to_fltt: no \"index\" specified in wire %s\n",
                 cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsNumber( json1 ) ) {
        fprintf( stderr, "cJSON_to_fltt: \"index\" is not a number in wire "
                 "%s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      index = json1->valueint;
      return fltt_wire( index );
      break;
    case AND:
    case OR:
    case PAND:
      json1 = cJSON_GetObjectItemCaseSensitive( json, "subtree1" );
      if ( json1 == NULL ) {
        fprintf( stderr, "cJSON_to_fltt: no \"subtree1\" specified in binary "
                 "tree %s\n",
                 cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsObject( json1 ) ) {
        fprintf( stderr, "cJSON_to_fltt: \"subtree1\" is not a JSON object in "
                 "fault tree %s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      json2 = cJSON_GetObjectItemCaseSensitive( json, "subtree2" );
      if ( json2 == NULL ) {
        fprintf( stderr, "cJSON_to_fltt: no \"subtree2\" specified in binary "
                 "tree %s\n",
                 cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsObject( json2 ) ) {
        fprintf( stderr, "cJSON_to_fltt: \"subtree2\" is not a JSON object in "
                 "fault tree %s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      return fltt_binary( cJSON_to_fltt( json1 ), cJSON_to_fltt( json2 ),
                          fltt_type );
      break;
    default:
      fprintf( stderr, "cJSON_to_exp: Unknown expression type." );
      exit( 1 );
      break;
  }
}
