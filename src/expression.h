/********************************************************************
 * expression.h
 *
 * Defines types for expressions.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_EXPRESSION_H__
#define __CCL_EXPRESSION_H__

#include <stdbool.h>
#include <cjson/cJSON.h>

/*********
 * Types *
 *********/

#define EXP_CASES 9

/* Different possible cases for expressions:
 * variables, constants, unary minus, addition, substraction, multiplication,
 * division, ...
 */
enum CASE_EXPRESSION {
  VAR,
  CONST,
  OPP,
  ADD,
  SUB,
  MUL,
  DIV,
  POW,
  LOG
  // ...
};

/* The type structure for expressions:
 * - each node has a type (as defined above),
 * - for variables, both children are NULL, "index" contains the index of the
 *   variable, and "name" its name,
 * - for constants, both children are NULL and "constant" contains the constant,
 * - for unary operators (OPP), only the second child is NULL,
 * - for binary operators (ADD, SUB, ...), no child is NULL.
 */
typedef struct expression_s {
  enum CASE_EXPRESSION expression_type;
  struct expression_s* child1;
  struct expression_s* child2;
  int index;
  char* name;
  double constant;
} *expression;

/************
 * Creation *
 ************/

expression exp_const( double c );
expression exp_var( int i, char* name );
expression exp_opp( expression e );
expression exp_log( expression e );
expression exp_add( expression e1, expression e2 );
expression exp_sub( expression e1, expression e2 );
expression exp_mul( expression e1, expression e2 );
expression exp_div( expression e1, expression e2 );
expression exp_pow( expression e1, expression e2 );

/****************
 * Manipulation *
 ****************/

/* exp_copy: returns a copy of an expression.
 * inputs:
 * - the expression.
 * output: a copy.
 */
expression exp_copy( expression e );

/* exp_equal: returns [true] if two expressions are syntactically
 * equal.
 * inputs:
 * - the expressions [e1] and [e2] to compare.
 * output: [true] if they are equal, [false] otherwise.
 */
bool exp_equal( expression e1, expression e2 );

/* exp_size: returns the ``size'' of an expression (basically the
 * number of symbols in the expression).
 * inputs: the expression [e].
 * output: an integer.
 */
int exp_size( expression e );

/* exp_eval: evaluates an expression given values for variables.
 * inputs:
 * - the expression [e] to evaluate,
 * - an array [sigma] of doubles that associates a value to each variable
 * output: the value of the expression.
 */
double exp_eval( expression e, double* sigma );

// /* exp_composition: composes two expressions.
//  * inputs:
//  * - the main expression [e],
//  * - the index [i] of the variable to be substituted,
//  * - the expression [esub] to substitute
//  * output: the composed expression.
//  */
// expression exp_composition( expression e, int i, expression esub );

/* exp_array_composition: composes expressions.
 * inputs:
 * - the main expression [e],
 * - an array [esub] of expressions to substitute for variables
 *   (assumed to be large enough).
 * output: the composed expression.
 */
expression exp_array_composition( expression e, expression* esub );

/* exp_simplification: computes a simpler form of an expression.
 * Currently: polynomial form.
 * inputs:
 * - the expression [e] to simplify.
 * output: a simpler expression.
 */
expression exp_simplification( expression e );

/* exp_derivative: computes the parital derivative in a variable of an
 * expression.
 * inputs:
 * - the expression [e] to derivate,
 * - the index [i] of the variable.
 * output: the partial derivative.
 */
expression exp_derivative( expression e, int i );

/************
 * Printing *
 ************/

void exp_printf( expression e );
int exp_snprintf( char* buf, int length, expression e );
expression cJSON_to_exp( cJSON* json );

/**********
 * Arrays *
 **********/

/* exp_mul_1_many: maps multiplication with a fixed expression across an array
 * of expressions.
 * inputs:
 * - the expression [e] to multiply by,
 * - the length [n] of the array,
 * - the array [es].
 * output: the resulting array.
 */
expression* exp_mul_1_many( expression e, int n, expression* es );

/* exp_mul_array: pairwise multiplies all expressions contained in two arrays.
 * inputs:
 * - the arrays of expressions [es1] and [es2],
 * - the lengths [n1] and [n2] of the arrays.
 * output: an array containing all multiplications
 */
expression* exp_mul_array( int n1, expression* es1, int n2, expression* es2 );

/* exp_cup_1_many: maps "cup" with a fixed expression across an array of
 * expressions.
 * inputs:
 * - the expression [e] to cup by,
 * - the length [n] of the array,
 * - the array [es].
 * output: the resulting array.
 */
expression* exp_cup_1_many( expression e, int n, expression* es );

/* exp_cup_array: pairwise "cups" all expressions contained in two arrays.
 * inputs:
 * - the arrays of expressions [es1] and [es2],
 * - the lengths [n1] and [n2] of the arrays.
 * output: an array containing all cups
 */
expression* exp_cup_array( int n1, expression* es1, int n2, expression* es2 );

#endif // __CCL_EXPRESSION_H__
