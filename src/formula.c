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
#include "formula.h"

/*
 * Creation
 */

formula fml_pvar( int i, char* name ) {
  formula res = GC_MALLOC( sizeof( struct formula_s ) );
  res->formula_type = PVAR;
  res->child1 = NULL;
  res->child2 = NULL;
  res->index = i;
  res->name = name;
  return res;
}

formula fml_binary( formula f1, formula f2, enum CASE_FORMULA s ) {
  formula res = GC_MALLOC( sizeof( struct formula_s ) );
  res->formula_type = s;
  res->child1 = f1;
  res->child2 = f2;
  res->index = 0;
  res->name = NULL;
  return res;
}


formula fml_conj( formula f1, formula f2 ) {
  return fml_binary( f1, f2, CONJ );
}

formula fml_disj( formula f1, formula f2 ) {
  return fml_binary( f1, f2, DISJ );
}

formula fml_impl( formula f1, formula f2 ) {
  return fml_binary( f1, f2, IMPL );
}

/*
 * Manipulation
 */

formula fml_copy( formula f ) {
  formula res = GC_MALLOC( sizeof( struct formula_s ) );
  res->formula_type = f->formula_type;
  res->child1 = f->child1 == NULL ? NULL : fml_copy( f->child1 ) ;
  res->child2 = f->child2 == NULL ? NULL : fml_copy( f->child2 ) ;
  res->index = f->index;
  if ( f->name ) {
    int n = strlen( f->name );
    res->name = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
    strcpy( res->name, f->name );
  } else
    res->name = NULL;
  return res;
}

bool fml_equal( formula f1, formula f2 ) {
  if ( f1->formula_type != f2->formula_type )
    return false;
  switch ( f1->formula_type ) {
    case PVAR:
      return f1->index == f2->index && strcmp( f1->name, f2->name) == 0;
      break;
    case CONJ:
      return fml_equal( f1->child1, f2->child1 ) &&
             fml_equal( f1->child2, f2->child2 );
      break;
    case DISJ:
      return fml_equal( f1->child1, f2->child1 ) &&
             fml_equal( f1->child2, f2->child2 );
      break;
    case IMPL:
      return fml_equal( f1->child1, f2->child1 ) &&
             fml_equal( f1->child2, f2->child2 );
      break;
    default:
      fprintf( stderr, "fml_equal: Unknown formula type." );
      exit( 1 );
      break;
  }
}

bool fml_eval( formula f, bool* sigma ) {
  switch ( f->formula_type ) {
    case PVAR:
      return sigma[ f->index ];
      break;
    case CONJ:
      return fml_eval( f->child1, sigma ) && fml_eval( f->child2, sigma );
      break;
    case DISJ:
      return fml_eval( f->child1, sigma ) || fml_eval( f->child2, sigma );
      break;
    case IMPL:
      return ! fml_eval( f->child1, sigma ) || fml_eval( f->child2, sigma );
      break;
    default:
      fprintf( stderr, "fml_eval: Unknown formula type." );
      exit( 1 );
      break;
  }
}

/*
 * Printing
 */

void fml_printf( formula f ) {
  int n = fml_snprintf( NULL, 0, f );
  char* s = GC_MALLOC( (n + 1) * sizeof( char ) );
  fml_snprintf( s, n + 1, f );
  printf( "%s", s );
}

int fml_snprintf( char* buf, int length, formula f ) {
  if ( f == NULL ) {
    return snprintf( buf, length, "NULL" );
  }
  int total = 0, diff = 0;
  switch ( f->formula_type ) {
    case PVAR:
      return snprintf( buf, length, "%s", f->name );
      break;
    case CONJ:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = fml_snprintf( buf, length, f->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") /\\ (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = fml_snprintf( buf, length, f->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case DISJ:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = fml_snprintf( buf, length, f->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") \\/ (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = fml_snprintf( buf, length, f->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case IMPL:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = fml_snprintf( buf, length, f->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") => (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = fml_snprintf( buf, length, f->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    default:
      fprintf( stderr, "fml_snprintf: Unknown formula type." );
      exit( 1 );
      break;
  }
}
