/********************************************************************
 * proof.h
 *
 * Defines types for proofs.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_PROOF_H__
#define __CCL_PROOF_H__

#include <stdbool.h>
#include "logic.h"
#include "sequent.h"

/*********
 * Types *
 *********/

/* Possible cases for proofs: a proof is either a leaf or a node. */
enum CASE_PROOF {
  LEAF,
  NODE
};

/* The type structure for proofs:
 * - each node has a type (as defined above), a "conclusion" sequent that is the
 *   conclusion of this (sub)proof and a "confidence" expression that expresses
 *   the confidence in the proof in function of the confidence in the
 *   hypotheses,
 * - for nodes, "children" denotes an array of children nodes of "subproofs",
 * - for leaves, "children" is NULL.
 */
typedef struct proof_s {
  enum CASE_PROOF proof_type;
  int subproofs;
  struct proof_s** children;
  sequent conclusion;
} *proof;

/************
 * Creation *
 ************/

proof prf_leaf( int n, formula* hyp_fmls, expression* hyp_pos_cfds,
                expression* hyp_neg_cfds, int i );
proof prf_node( int n, proof* subproofs, sequent conclusion );
proof prf_unary( proof p, sequent conclusion );
proof prf_binary( proof p1, proof p2, sequent conclusion );
proof prf_ternary( proof p1, proof p2, proof p3, sequent conclusion );

proof prf_axiom( int n, formula* hypotheses, expression* pos_cfds,
                 expression* neg_cfds, int i );
proof prf_impl_i( proof p, logic log, int i );
proof prf_conj_i( proof l, proof r, logic log );
proof prf_disj_i( proof l, proof r, logic log );
proof prf_acc( int n, proof* ps ); // ( t | t', f | f' )

/****************
 * Manipulation *
 ****************/

/* prf_copy: returns a copy of a proof.
 * inputs:
 * - the proof.
 * output: a copy.
 */
proof prf_copy( proof p );

/* prf_equal: returns [true] if two proofs are syntactically equal.
 * inputs:
 * - the proofs [p1] and [p2] to compare.
 * output: [true] if they are equal, [false] otherwise.
 */
bool prf_equal( proof p1, proof p2 );

// /* prf_formula: returns the formula associated to a proof.
//  * inputs:
//  * - the proof.
//  * output: the associated formula.
//  */
// formula prf_formula( proof p );

/************
 * Printing *
 ************/

void prf_printf( proof p );
int prf_snprintf( char* buf, int length, proof p );

#endif // __CCL_PROOF_H__
