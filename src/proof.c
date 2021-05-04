/********************************************************************
 * proof.c
 *
 * Defines functions to manipulate proofs.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <gc.h>
#include "proof.h"

/*
 * Creation
 */

proof prf_leaf( int n, formula* hyp_fmls, expression* hyp_pos_cfds,
                expression* hyp_neg_cfds, int i ) {
  proof res = GC_MALLOC( sizeof( struct proof_s ) );
  res->proof_type = LEAF;
  res->children = NULL;
  res->subproofs = 0;
  res->conclusion = sqt_new( n, hyp_fmls, hyp_pos_cfds, hyp_neg_cfds,
                             hyp_fmls[ i ], 1, hyp_pos_cfds + i, 1,
                             hyp_neg_cfds + i );
  return res;
}

proof prf_node( int n, proof* subproofs, sequent conclusion ) {
  proof res = GC_MALLOC( sizeof( struct proof_s ) );
  res->proof_type = NODE;
  res->children = subproofs;
  res->subproofs = n;
  res->conclusion = conclusion;
  return res;
}

proof prf_axiom( int n, formula* hypotheses, expression* pos_cfds,
                 expression* neg_cfds, int i ) {
  return prf_leaf( n, hypotheses, pos_cfds, neg_cfds, i );
}

proof prf_unary( proof p, sequent conclusion ) {
  proof* l = GC_MALLOC( sizeof( proof ) );
  l[ 0 ] = p;
  return prf_node( 1, l, conclusion );
}

proof prf_binary( proof p1, proof p2, sequent conclusion ) {
  proof* l = GC_MALLOC( 2 * sizeof( proof ) );
  l[ 0 ] = p1;
  l[ 1 ] = p2;
  return prf_node( 2, l, conclusion );
}

proof prf_ternary( proof p1, proof p2, proof p3, sequent conclusion ) {
  proof* l = GC_MALLOC( 3 * sizeof( proof ) );
  l[ 0 ] = p1;
  l[ 1 ] = p2;
  l[ 2 ] = p3;
  return prf_node( 2, l, conclusion );
}

proof prf_impl_i( proof p, logic log, int i ) {
  if ( i >= p->conclusion->hyp_num ) {
    fprintf( stderr, "prf_impl_i: Not enough hypotheses.\n" );
    exit( 1 );
  }
  // forming the hypotheses of the conclusion sequent
  formula* hyp_fmls = GC_MALLOC( ( p->conclusion->hyp_num - 1 ) *
                                 sizeof( formula ) );
  expression* hyp_pos_cfds = GC_MALLOC( ( p->conclusion->hyp_num - 1 ) *
                                        sizeof( expression ) );
  expression* hyp_neg_cfds = GC_MALLOC( ( p->conclusion->hyp_num - 1 ) *
                                        sizeof( expression ) );
  for ( int j = 0; j < i; ++j ) {
    hyp_fmls[ j ] = p->conclusion->hyp_fmls[ j ];
    hyp_pos_cfds[ j ] = p->conclusion->hyp_pos_cfds[ j ];
    hyp_neg_cfds[ j ] = p->conclusion->hyp_neg_cfds[ j ];
  }
  for ( int j = i; j < p->conclusion->hyp_num - 1; ++j ) {
    hyp_fmls[ j ] = p->conclusion->hyp_fmls[ j + 1 ];
    hyp_pos_cfds[ j ] = p->conclusion->hyp_pos_cfds[ j + 1 ];
    hyp_neg_cfds[ j ] = p->conclusion->hyp_neg_cfds[ j + 1 ];
  }
  // forming the conclusion formula of the conclusion sequent
  formula fml = fml_impl( p->conclusion->hyp_fmls[ i ],
                          p->conclusion->ccl_fml );
  // forming the array of positive confidences of the conclusion sequent
  enum TRUTH_VALUE** buf_cfds = GC_MALLOC( TRUTH_VALUE_CASES *
                                           TRUTH_VALUE_CASES *
                                           sizeof( enum TRUTH_VALUE* ) );
  int size_ii = lo_inverse_image( buf_cfds, log->imp, TRUE );
  int cfd_num = p->conclusion->pos_cfd_num *
                p->conclusion->neg_cfd_num;
  expression* pos_cfds = GC_MALLOC( cfd_num * sizeof( expression ) );
  expression hyp_exp, ccl_exp;
  for ( int j = 0; j < p->conclusion->pos_cfd_num; ++j ) {
    for ( int k = 0; k < p->conclusion->neg_cfd_num; ++k ) {
      int jk = p->conclusion->neg_cfd_num * j + k;
      pos_cfds[ jk ] = exp_const( 0 );
      for ( int l = 0; l < size_ii; ++l ) {
        switch ( buf_cfds[ l ][ 0 ] ) {
          case FALSE:
            hyp_exp = p->conclusion->hyp_neg_cfds[ i ];
            break;
          case UNDET:
            hyp_exp = exp_sub( exp_const( 1 ),
                               exp_add( p->conclusion->hyp_pos_cfds[ i ],
                                        p->conclusion->hyp_neg_cfds[ i ] ) );
            break;
          case TRUE:
            hyp_exp = p->conclusion->hyp_pos_cfds[ i ];
            break;
          default:
            break;
        }
        switch ( buf_cfds[ l ][ 1 ] ) {
          case FALSE:
            ccl_exp = p->conclusion->ccl_neg_cfds[ k ];
            break;
          case UNDET:
            ccl_exp = exp_sub( exp_const( 1 ),
                               exp_add( p->conclusion->ccl_pos_cfds[ j ],
                                        p->conclusion->ccl_neg_cfds[ k ] ) );
            break;
          case TRUE:
            ccl_exp = p->conclusion->ccl_pos_cfds[ j ];
            break;
          default:
            break;
        }
        pos_cfds[ jk ] = exp_add( pos_cfds[ jk ], exp_mul( hyp_exp, ccl_exp ) );
      }
    }
  }
  // forming the array of negative confidences of the conclusion sequent
  size_ii = lo_inverse_image( buf_cfds, log->imp, FALSE );
  expression* neg_cfds = GC_MALLOC( cfd_num * sizeof( expression ) );
  for ( int j = 0; j < p->conclusion->pos_cfd_num; ++j ) {
    for ( int k = 0; k < p->conclusion->neg_cfd_num; ++k ) {
      int jk = p->conclusion->neg_cfd_num * j + k;
      neg_cfds[ jk ] = exp_const( 0 );
      for ( int l = 0; l < size_ii; ++l ) {
        switch ( buf_cfds[ l ][ 0 ] ) {
          case FALSE:
            hyp_exp = p->conclusion->hyp_neg_cfds[ i ];
            break;
          case UNDET:
            hyp_exp = exp_sub( exp_const( 1 ),
                               exp_add( p->conclusion->hyp_pos_cfds[ i ],
                                        p->conclusion->hyp_neg_cfds[ i ] ) );
            break;
          case TRUE:
            hyp_exp = p->conclusion->hyp_pos_cfds[ i ];
            break;
          default:
            break;
        }
        switch ( buf_cfds[ l ][ 1 ] ) {
          case FALSE:
            ccl_exp = p->conclusion->ccl_neg_cfds[ k ];
            break;
          case UNDET:
            ccl_exp = exp_sub( exp_const( 1 ),
                               exp_add( p->conclusion->ccl_pos_cfds[ j ],
                                        p->conclusion->ccl_neg_cfds[ k ] ) );
            break;
          case TRUE:
            ccl_exp = p->conclusion->ccl_pos_cfds[ j ];
            break;
          default:
            break;
        }
        neg_cfds[ jk ] = exp_add( neg_cfds[ jk ], exp_mul( hyp_exp, ccl_exp ) );
      }
    }
  }
  // conclusion sequent
  sequent s = sqt_new( p->conclusion->hyp_num - 1, hyp_fmls, hyp_pos_cfds,
                       hyp_neg_cfds, fml, cfd_num, pos_cfds, cfd_num,
                       neg_cfds );
  return prf_unary( p, s );
}

proof prf_conj_i( proof l, proof r, logic log ) {
  ( void ) log;
  if ( l->conclusion->hyp_num != r->conclusion->hyp_num ) {
    fprintf( stderr, "prf_conj_i: Different number of hypotheses in contexts.\n" );
    exit( 1 );
  }
  for ( int i = 0; i < l->conclusion->hyp_num; ++i ) {
    if ( ! fml_equal( l->conclusion->hyp_fmls[ i ],
                      r->conclusion->hyp_fmls[ i ] ) ) {
      fprintf( stderr, "prf_conj_i: Different formulas in contexts.\n" );
      exit( 1 );
    }
    if ( ! exp_equal( l->conclusion->hyp_pos_cfds[ i ],
                      r->conclusion->hyp_pos_cfds[ i ] ) ) {
      fprintf( stderr, "prf_conj_i: Different positive confidences in contexts.\n" );
      exit( 1 );
    }
    if ( ! exp_equal( l->conclusion->hyp_neg_cfds[ i ],
                      r->conclusion->hyp_neg_cfds[ i ] ) ) {
      fprintf( stderr, "prf_conj_i: Different negative confidences in contexts.\n" );
      exit( 1 );
    }
  }
  // forming the conclusion formula of the conclusion sequent
  formula fml = fml_conj( l->conclusion->ccl_fml, r->conclusion->ccl_fml );
  // forming the array of positive confidences of the conclusion sequent
  expression* pos_cfds = exp_mul_array( l->conclusion->pos_cfd_num,
                                        l->conclusion->ccl_pos_cfds,
                                        r->conclusion->pos_cfd_num,
                                        r->conclusion->ccl_pos_cfds );
  // forming the array of negative confidences of the conclusion sequent
  expression* neg_cfds = exp_cup_array( l->conclusion->neg_cfd_num,
                                        l->conclusion->ccl_neg_cfds,
                                        r->conclusion->neg_cfd_num,
                                        r->conclusion->ccl_neg_cfds );
  // conclusion sequent
  sequent s = sqt_new( l->conclusion->hyp_num, l->conclusion->hyp_fmls,
                       l->conclusion->hyp_pos_cfds, l->conclusion->hyp_neg_cfds,
                       fml,
                       l->conclusion->pos_cfd_num * r->conclusion->pos_cfd_num,
                       pos_cfds,
                       l->conclusion->neg_cfd_num * r->conclusion->neg_cfd_num,
                       neg_cfds );
  return prf_binary( l, r, s );
}

proof prf_disj_i( proof l, proof r, logic log ) {
  ( void ) log;
  if ( l->conclusion->hyp_num != r->conclusion->hyp_num ) {
    fprintf( stderr, "prf_disj_i: Different number of hypotheses in contexts.\n" );
    exit( 1 );
  }
  for ( int i = 0; i < l->conclusion->hyp_num; ++i ) {
    if ( ! fml_equal( l->conclusion->hyp_fmls[ i ],
                      r->conclusion->hyp_fmls[ i ] ) ) {
      fprintf( stderr, "prf_disj_i: Different formulas in contexts.\n" );
      exit( 1 );
    }
    if ( ! exp_equal( l->conclusion->hyp_pos_cfds[ i ],
                      r->conclusion->hyp_pos_cfds[ i ] ) ) {
      fprintf( stderr, "prf_disj_i: Different positive confidences in contexts.\n" );
      exit( 1 );
    }
    if ( ! exp_equal( l->conclusion->hyp_neg_cfds[ i ],
                      r->conclusion->hyp_neg_cfds[ i ] ) ) {
      fprintf( stderr, "prf_disj_i: Different negative confidences in contexts.\n" );
      exit( 1 );
    }
  }
  // forming the conclusion formula of the conclusion sequent
  formula fml = fml_disj( l->conclusion->ccl_fml, r->conclusion->ccl_fml );
  // forming the array of positive confidences of the conclusion sequent
  expression* pos_cfds = exp_cup_array( l->conclusion->pos_cfd_num,
                                        l->conclusion->ccl_pos_cfds,
                                        r->conclusion->pos_cfd_num,
                                        r->conclusion->ccl_pos_cfds );
  // forming the array of negative confidences of the conclusion sequent
  expression* neg_cfds = exp_mul_array( l->conclusion->neg_cfd_num,
                                        l->conclusion->ccl_neg_cfds,
                                        r->conclusion->neg_cfd_num,
                                        r->conclusion->ccl_neg_cfds );
  // conclusion sequent
  sequent s = sqt_new( l->conclusion->hyp_num, l->conclusion->hyp_fmls,
                       l->conclusion->hyp_pos_cfds, l->conclusion->hyp_neg_cfds,
                       fml,
                       l->conclusion->pos_cfd_num * r->conclusion->pos_cfd_num,
                       pos_cfds,
                       l->conclusion->neg_cfd_num * r->conclusion->neg_cfd_num,
                       neg_cfds );
  return prf_binary( l, r, s );
}

proof prf_acc( int n, proof* ps ) {
  if ( n < 2 ) {
    fprintf( stderr, "prf_acc: Needs at least two proofs.\n" );
    exit( 1 );
  }
  sequent s = ps[ 0 ]->conclusion;
  for ( int i = 0; i < n; ++i ) {
    if ( ps[ i ]->conclusion->hyp_num != s->hyp_num ) {
      fprintf( stderr, "prf_acc: Different number of hypotheses in contexts.\n" );
      exit( 1 );
    }
    for ( int j = 0; j < s->hyp_num; ++j ) {
      if ( ! fml_equal( ps[ i ]->conclusion->hyp_fmls[ j ],
                        s->hyp_fmls[ j ] ) ) {
        fprintf( stderr, "prf_acc: Different formulas in contexts.\n" );
        exit( 1 );
      }
      if ( ! exp_equal( ps[ i ]->conclusion->hyp_pos_cfds[ j ],
                        s->hyp_pos_cfds[ j ] ) ) {
        fprintf( stderr, "prf_acc: Different positive confidences in contexts.\n" );
        exit( 1 );
      }
      if ( ! exp_equal( ps[ i ]->conclusion->hyp_neg_cfds[ j ],
                        s->hyp_neg_cfds[ j ] ) ) {
        fprintf( stderr, "prf_acc: Different negative confidences in contexts.\n" );
        exit( 1 );
      }
    }
    if ( ! fml_equal( ps[ i ]->conclusion->ccl_fml, s->ccl_fml ) ) {
      fprintf( stderr, "prf_acc: Different conclusion formulas.\n" );
      exit( 1 );
    }
  }
  // computing number of positive confidences for conclusion formula
  int pos_l = 0;
  for ( int i = 0; i < n; ++i ) pos_l += ps[ i ]->conclusion->pos_cfd_num;
  // computing array of positive confidences for conclusion formula
  expression* pos_cfds = GC_MALLOC( pos_l * sizeof( expression ) );
  int k = 0;
  for ( int i = 0; i < n; ++i ) {
    for ( int j = 0; j < ps[ i ]->conclusion->pos_cfd_num; ++j )
      pos_cfds[ k++ ] = ps[ i ]->conclusion->ccl_pos_cfds[ j ];
  }
  // computing number of negative confidences for conclusion formula
  int neg_l = 0;
  for ( int i = 0; i < n; ++i ) neg_l += ps[ i ]->conclusion->neg_cfd_num;
  // computing array of negative confidences for conclusion formula
  expression* neg_cfds = GC_MALLOC( neg_l * sizeof( expression ) );
  k = 0;
  for ( int i = 0; i < n; ++i ) {
    for ( int j = 0; j < ps[ i ]->conclusion->neg_cfd_num; ++j )
      neg_cfds[ k++ ] = ps[ i ]->conclusion->ccl_neg_cfds[ j ];
  }
  // conclusion sequent
  s = sqt_new( s->hyp_num, s->hyp_fmls, s->hyp_pos_cfds, s->hyp_neg_cfds,
               s->ccl_fml, pos_l, pos_cfds, neg_l, neg_cfds );
  return prf_node( n, ps, s );
}

/*
 * Manipulation
 */

proof prf_copy( proof p ) {
  proof res = GC_MALLOC( sizeof( struct proof_s ) );
  res->proof_type = p->proof_type;
  res->subproofs = p->subproofs;
  res->children = GC_MALLOC( res->subproofs * sizeof( proof ) );
  for ( int i = 0; i < res->subproofs; ++i )
    res->children[ i ] = prf_copy( p->children[ i ] );
  res->conclusion = sqt_copy( p->conclusion );
  return res;
}

bool prf_equal( proof p1, proof p2 ) {
  if ( p1->proof_type != p2->proof_type )                      return false;
  if ( p1->subproofs != p2->subproofs )                        return false;
  if ( ! sqt_equal( p1->conclusion, p2->conclusion ) )         return false;
  for ( int i = 0; i < p1->subproofs; ++i )
    if ( ! prf_equal( p1->children[ i ], p2->children[ i ] ) ) return false;
  return true;
}

/*
 * Printing
 */

void prf_printf( proof p ) {
  char* buf = NULL;
  int n = prf_snprintf( buf, 0, p );
  buf = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
  prf_snprintf( buf, n, p );
  printf( "%s", buf );
}

int prf_snprintf( char* buf, int length, proof p ) {
  int total = 0, diff = 0;
  switch ( p->proof_type ) {
    case LEAF:
      total = sqt_snprintf( buf, length, p->conclusion );
      break;
    case NODE:
      diff = snprintf( buf, length, "[ " );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      for ( int i = 0; i < p->subproofs - 1; ++i ) {
        diff = prf_snprintf( buf, length, p->children[ i ] );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
        diff = snprintf( buf, length, "; " );
        total += diff;
        buf += diff;
        length -= diff;
        if ( length < 0 ) length = 0;
      }
      if ( p->subproofs > 0 ) {
        diff = prf_snprintf( buf, length, p->children[ p->subproofs - 1 ] );
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
      diff = snprintf( buf, length, "] |= " );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = sqt_snprintf( buf, length, p->conclusion );
      total += diff;
      break;
    default:
      fprintf( stderr, "prf_snprintf: Unknown proof type.\n" );
      exit( 1 );
      break;
  }
  return total;
}
