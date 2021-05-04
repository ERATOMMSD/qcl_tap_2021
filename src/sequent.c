/********************************************************************
 * sequent.c
 *
 * Defines functions to manipulate sequents.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdio.h>
#include <gc.h>
#include "sequent.h"

/*
 * Creation
 */

sequent sqt_new( int hyp_num, formula* hyp_fmls, expression* hyp_pos_cfds,
                 expression* hyp_neg_cfds, formula ccl_fml, int pos_cfd_num,
                 expression* ccl_pos_cfds, int neg_cfd_num,
                 expression* ccl_neg_cfds )
{
  sequent res = GC_MALLOC( sizeof( struct sequent_s ) );
  res->hyp_num = hyp_num;
  res->hyp_fmls = hyp_fmls;
  res->hyp_pos_cfds = hyp_pos_cfds;
  res->hyp_neg_cfds = hyp_neg_cfds;
  res->ccl_fml = ccl_fml;
  res->pos_cfd_num = pos_cfd_num;
  res->ccl_pos_cfds = ccl_pos_cfds;
  res->neg_cfd_num = neg_cfd_num;
  res->ccl_neg_cfds = ccl_neg_cfds;
  return res;
}

/*
 * Manipulation
 */

sequent sqt_copy( sequent s ) {
  sequent res = GC_MALLOC( sizeof( struct sequent_s ) );
  res->hyp_num = s->hyp_num;
  res->hyp_fmls = GC_MALLOC( res->hyp_num * sizeof( formula ) );
  res->hyp_pos_cfds = GC_MALLOC( res->hyp_num * sizeof( expression ) );
  res->hyp_neg_cfds = GC_MALLOC( res->hyp_num * sizeof( expression ) );
  for ( int i = 0; i < res->hyp_num; ++i ) {
    res->hyp_fmls[ i ] = fml_copy( s->hyp_fmls[ i ] );
    res->hyp_pos_cfds[ i ] = exp_copy( s->hyp_pos_cfds[ i ] );
    res->hyp_neg_cfds[ i ] = exp_copy( s->hyp_neg_cfds[ i ] );
  }
  res->ccl_fml = fml_copy( s->ccl_fml );
  res->pos_cfd_num = s->pos_cfd_num;
  res->ccl_pos_cfds = GC_MALLOC( res->pos_cfd_num * sizeof( expression ) );
  for ( int i = 0; i < res->pos_cfd_num; ++i )
    res->ccl_pos_cfds[ i ] = exp_copy( s->ccl_pos_cfds[ i ] );
  res->neg_cfd_num = s->neg_cfd_num;
  res->ccl_neg_cfds = GC_MALLOC( res->neg_cfd_num * sizeof( expression ) );
  for ( int i = 0; i < res->neg_cfd_num; ++i )
    res->ccl_neg_cfds[ i ] = exp_copy( s->ccl_neg_cfds[ i ] );
  return res;
}

bool sqt_equal( sequent s1, sequent s2 ) {
  if ( s1->hyp_num != s2->hyp_num )
    return false;
  if ( ! fml_equal( s1->ccl_fml, s2->ccl_fml ) )
    return false;
  for ( int i = 0; i < s1->hyp_num; ++i ) {
    if ( ! fml_equal( s1->hyp_fmls[ i ], s2->hyp_fmls[ i ] ) )
      return false;
    if ( ! exp_equal( s1->hyp_pos_cfds[ i ], s2->hyp_pos_cfds[ i ] ) )
      return false;
    if ( ! exp_equal( s1->hyp_neg_cfds[ i ], s2->hyp_neg_cfds[ i ] ) )
      return false;
  }
  if ( s1->pos_cfd_num != s2->pos_cfd_num )
    return false;
  for ( int i = 0; i < s1->pos_cfd_num; ++i )
    if ( ! exp_equal( s1->ccl_pos_cfds[ i ], s2->ccl_pos_cfds[ i ] ) )
      return false;
  if ( s1->neg_cfd_num != s2->neg_cfd_num )
    return false;
  for ( int i = 0; i < s1->neg_cfd_num; ++i )
    if ( ! exp_equal( s1->ccl_neg_cfds[ i ], s2->ccl_neg_cfds[ i ] ) )
      return false;
  return true;
}

/*
 * Printing
 */

void sqt_printf( sequent s ) {
  int n = sqt_snprintf( NULL, 0, s );
  char* str = GC_MALLOC( (n + 1) * sizeof( char ) );
  sqt_snprintf( str, n + 1, s );
  printf( "%s", str );
}

int sqt_snprintf( char* buf, int length, sequent s ) {
  int total = 0, diff = 0;
  for ( int i = 0; i < s->hyp_num - 1; ++i ) {
    diff = fml_snprintf( buf, length, s->hyp_fmls[ i ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " : ( " );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = exp_snprintf( buf, length, s->hyp_pos_cfds[ i ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " , " );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = exp_snprintf( buf, length, s->hyp_neg_cfds[ i ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " ), " );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
  }
  if ( s->hyp_num > 0 ) {
    diff = fml_snprintf( buf, length, s->hyp_fmls[ s->hyp_num - 1 ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " : ( " );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = exp_snprintf( buf, length, s->hyp_pos_cfds[ s->hyp_num - 1 ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " , " );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = exp_snprintf( buf, length, s->hyp_neg_cfds[ s->hyp_num - 1 ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " )" );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
  }
  diff = snprintf( buf, length, " |- " );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = fml_snprintf( buf, length, s->ccl_fml );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  diff = snprintf( buf, length, " : ( " );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  for ( int i = 0; i < s->pos_cfd_num - 1; ++i ) {
    diff = exp_snprintf( buf, length, s->ccl_pos_cfds[ i ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " | " );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
  }
  if ( s->pos_cfd_num > 0 ) {
    diff = exp_snprintf( buf, length, s->ccl_pos_cfds[ s->pos_cfd_num - 1 ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
  }
  diff = snprintf( buf, length, " , " );
  total += diff;
  buf += diff;
  length -= diff;
  if ( length < 0 ) length = 0;
  for ( int i = 0; i < s->neg_cfd_num - 1; ++i ) {
    diff = exp_snprintf( buf, length, s->ccl_neg_cfds[ i ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
    diff = snprintf( buf, length, " | " );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
  }
  if ( s->neg_cfd_num > 0 ) {
    diff = exp_snprintf( buf, length, s->ccl_neg_cfds[ s->neg_cfd_num - 1 ] );
    total += diff;
    buf += diff;
    length -= diff;
    if ( length < 0 ) length = 0;
  }
  diff = snprintf( buf, length, " )" );
  total += diff;
  return total;
}
