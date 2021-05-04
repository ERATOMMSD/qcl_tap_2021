/********************************************************************
 * formula.h
 *
 * Defines types for formulas.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_FORMULA_H__
#define __CCL_FORMULA_H__

#include <stdbool.h>

/*********
 * Types *
 *********/

#define FML_CASES 4

/* Different possible cases for formulas:
 * propositional variables, conjunction, disjunction, implication...
 */
enum CASE_FORMULA {
  PVAR,
  CONJ,
  DISJ,
  IMPL
};

/* The type structure for formulas:
 * - each node has a type (as defined above),
 * - for propositional variables, both children are NULL and "index" contains
 *   the index of the variable and "name" its name,
 * - for conjunction, disjunction, and implications, the children correspond to
 *   subformulas.
 */
typedef struct formula_s {
  enum CASE_FORMULA formula_type;
  struct formula_s* child1;
  struct formula_s* child2;
  int index;
  char* name;
} *formula;

/************
 * Creation *
 ************/

formula fml_pvar( int i, char* name );
formula fml_binary( formula f1, formula f2, enum CASE_FORMULA s );
formula fml_conj( formula f1, formula f2 );
formula fml_disj( formula f1, formula f2 );
formula fml_impl( formula f1, formula f2 );

/****************
 * Manipulation *
 ****************/

/* fml_copy: returns a copy of a formula.
 * inputs:
 * - the formula.
 * output: a copy.
 */
formula fml_copy( formula f );

/* fml_equal: returns [true] if two formulas are syntactically equal.
 * inputs:
 * - the formulas [f1] and [f2] to compare.
 * output: [true] if they are equal, [false] otherwise.
 */
bool fml_equal( formula f1, formula f2 );

/* fml_eval: evaluates a formula given values for variables.
 * inputs:
 * - the formula [f] to evaluate,
 * - an array [sigma] of booleans that associates a value to each variable
 * output: the value of the expression.
 */
bool fml_eval( formula f, bool* sigma );

/************
 * Printing *
 ************/

void fml_printf( formula f );
int fml_snprintf( char* buf, int length, formula f );

#endif // __CCL_FORMULA_H__
