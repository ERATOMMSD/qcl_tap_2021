/********************************************************************
 * sequent.h
 *
 * Defines types for sequents.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_SEQUENT_H__
#define __CCL_SEQUENT_H__

#include <stdbool.h>
#include "expression.h"
#include "formula.h"

/*********
 * Types *
 *********/

/* The type structure for sequents:
 * - a number "hyp_num" of hypotheses,
 * - an array "hyp_fmls" of formulas (of length "hyp_num") for hypotheses,
 * - an array "hyp_cfds" of expressions (of length "hyp_num") for confidence in
 *   the hypotheses,
 * - a formula "ccl_fml" for the conclusion,
 * - a number "cfd_num" of branches involved in the confidence in the
 *   conclusion,
 * - an array "ccl_cfd" of expressions (of length "cfd_num") for confidence in
 *   the conclusion.
 */
typedef struct sequent_s {
  int hyp_num;
  formula* hyp_fmls;
  expression* hyp_pos_cfds;
  expression* hyp_neg_cfds;
  formula ccl_fml;
  int pos_cfd_num;
  expression* ccl_pos_cfds;
  int neg_cfd_num;
  expression* ccl_neg_cfds;
} *sequent;

/************
 * Creation *
 ************/

sequent sqt_new( int hyp_num, formula* hyp_fmls, expression* hyp_pos_cfds,
                 expression* hyp_neg_cfds, formula ccl_fml, int pos_cfd_num,
                 expression* ccl_pos_cfds, int neg_cfd_num,
                 expression* ccl_neg_cfds );

/****************
 * Manipulation *
 ****************/

/* sqt_copy: returns a copy of a sequent.
 * inputs:
 * - the sequent.
 * output: a copy.
 */
sequent sqt_copy( sequent s );

/* sqt_equal: returns [true] if two sequents are syntactically equal.
 * inputs:
 * - the sequents [s1] and [s2] to compare.
 * output: [true] if they are equal, [false] otherwise.
 */
bool sqt_equal( sequent s1, sequent s2 );

/************
 * Printing *
 ************/

void sqt_printf( sequent e );
int sqt_snprintf( char* buf, int length, sequent s );

#endif // __CCL_SEQUENT_H__
