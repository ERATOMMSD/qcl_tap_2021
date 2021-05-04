/********************************************************************
 * formula.h
 *
 * Defines types for fault trees.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_FAULT_TREE_H__
#define __CCL_FAULT_TREE_H__

#include <stdbool.h>
#include "proof.h"

/*********
 * Types *
 *********/

#define FLTT_CASES 4

/* Different possible cases for fault trees:
 * wires, and, or, pand...
 */
enum CASE_FAULT_TREE {
  WIRE,
  AND,
  OR,
  PAND
};

/* The type structure for fault trees:
 * - each node has a type (as defined above), a number of "ports", and a list of
 *   children of the size as the number of ports,
 * - wires have 0 ports and "index" contains the index of the wire,
 * - for gates, the children correspond to subformulas.
 */
typedef struct fault_tree_s {
  enum CASE_FAULT_TREE fault_tree_type;
  int ports;
  struct fault_tree_s** children;
  int index;
} *fault_tree;

/************
 * Creation *
 ************/

fault_tree fltt_wire( int i );
fault_tree fltt_binary( fault_tree t1, fault_tree t2, enum CASE_FAULT_TREE s );
fault_tree fltt_and( fault_tree t1, fault_tree t2 );
fault_tree fltt_or( fault_tree t1, fault_tree t2 );
fault_tree fltt_pand( fault_tree t1, fault_tree t2 );

/****************
 * Manipulation *
 ****************/

/* fltt_copy: returns a copy of a fault tree.
 * inputs:
 * - the fault_tree.
 * output: a copy.
 */
fault_tree fltt_copy( fault_tree t );

/* fltt_equal: returns [true] if two fault trees are syntactically equal.
 * inputs:
 * - the fault trees [f1] and [f2] to compare.
 * output: [true] if they are equal, [false] otherwise.
 */
bool fltt_equal( fault_tree t1, fault_tree t2 );

/* fltt_propagate: propagates faults in a fault tree given faulty wires.
 * inputs:
 * - the fault tree [t],
 * - an array [sigma] of booleans that associates "true" to a wire if it is
 *   faulty.
 * output: a boolean that represents whether the whole system is faulty.
 */
bool fltt_propagate( fault_tree t, bool* sigma );

/* fltt_propagate_prob: propagates fault probabilities in a fault tree given
 * probabilities of fault for wires.
 * inputs:
 * - the fault tree [t],
 * - an array [sigma] of doubles that associates a probability of failure to
 *   each wire.
 * output: a double that represents the probability of failure of the system.
 */
double fltt_propagate_prob( fault_tree t, double* sigma );

/* fltt_to_prf: translates a fault tree to a proof.
 * inputs:
 * - the maximal index [n] of wires used in [t],
 * - the fault tree [t],
 * - the logic [log] (the interpretation of connectors used in the proof).
 * output: a proof tree corresponding to faults _not_ propagating to the whole
 *         system.
 */
proof fltt_to_prf( int n, fault_tree t, logic log );

/************
 * Printing *
 ************/

void fltt_printf( fault_tree t );
int fltt_symbol_snprintf( char* buf, int length, enum CASE_FAULT_TREE s );
int fltt_snprintf( char* buf, int length, fault_tree t );
fault_tree cJSON_to_fltt( cJSON* json );

#endif // __CCL_FAULT_TREE_H__
