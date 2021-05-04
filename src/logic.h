/********************************************************************
 * logic.h
 *
 * Defines types for logical operators.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_LOGIC_H__
#define __CCL_LOGIC_H__

/*********
 * Types *
 *********/

/* Possible cases for truth values: true, false, and undeterminate. */
enum TRUTH_VALUE {
  FALSE,
  UNDET,
  TRUE
};

#define TRUTH_VALUE_CASES 3

// cases for logical operators: nullary, unary, binary, and ternary
#define LO_CASES 4

/* The type structure for logical operators:
 * - each operator has an arity
 * - and a truth table, encoded as an array of truth values of length
 *   TRUTH_VALUE_CASES^n.
 *   The order is FALSE -> UNDET -> TRUE, with the last value changing first,
 *   e.g., for binary operators, the order is:
 *   FF -> FU -> FT -> UF -> UU -> UT -> TF -> TU -> TT.
 */
typedef struct logical_operator_s {
  int arity;
  enum TRUTH_VALUE* truth_table;
} *logical_operator;

/* The type structure for logics:
 * - a structure per logical operator (negation, implication, disjunction, and
 *   conjunction).
 */
typedef struct logic_s {
  logical_operator neg;
  logical_operator imp;
  logical_operator disj;
  logical_operator conj;
} *logic;

/************
 * Creation *
 ************/

logical_operator table_to_lo( int arity, enum TRUTH_VALUE* truth_table );

logic los_to_log( logical_operator neg, logical_operator imp,
                  logical_operator disj, logical_operator conj );

/****************
 * Manipulation *
 ****************/

/* int_to_truth_value: returns the truth value corresponding to an integer.
 * inputs:
 * - the integer.
 * output: a truth value.
 */
enum TRUTH_VALUE int_to_truth_value( int n );

/* truth_value_to_int: returns the integer corresponding to a truth value.
 * inputs:
 * - the truth value.
 * output: an integer.
 */
int truth_value_to_int( enum TRUTH_VALUE tv );

/* lo_copy: returns a copy of a logical_operator.
 * inputs:
 * - the logical_operator.
 * output: a copy.
 */
logical_operator lo_copy( logical_operator lo );

/* lo_inverse_image: returns the array of inputs that are mapped to a given
 * value.
 * inputs:
 * - a buffer [buf] that will contain the array,
 * - the logical_operator [o],
 * - the value [tv].
 * output: the size of the array of arrays (of size [o->arity]) of truth
 * values representing the inverse image, stocked in buf.
 * warning: buf should be big enough to contain the inverse image.
 * ("int_pow( TRUTH_VALUE_CASES, o->arity ) * sizeof( TRUTH_VALUE* )" is always
 * enough)
 */
int lo_inverse_image( enum TRUTH_VALUE** buf, logical_operator o,
                      enum TRUTH_VALUE tv );

/* lo_equal: returns [true] if two logical_operators are syntactically equal.
 * inputs:
 * - the logical_operators [lo1] and [lo2] to compare.
 * output: [true] if they are equal, [false] otherwise.
 */
bool lo_equal( logical_operator lo1, logical_operator lo2 );

/* log_copy: returns a copy of a logic.
 * inputs:
 * - the logic.
 * output: a copy.
 */
logic log_copy( logic log );

/* log_equal: returns [true] if two logics are syntactically equal.
 * inputs:
 * - the logics [log1] and [log2] to compare.
 * output: [true] if they are equal, [false] otherwise.
 */
bool log_equal( logic log1, logic log2 );

/************
 * Printing *
 ************/

void tv_printf( enum TRUTH_VALUE tv );
int tv_snprintf( char* buf, int length, enum TRUTH_VALUE tv );
void lo_printf( logical_operator o );
int lo_snprintf( char* buf, int length, logical_operator o );
void log_printf( logic l );
int log_snprintf( char* buf, int length, logic l );

#endif // __CCL_PROOF_H__
