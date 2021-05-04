/********************************************************************
 * expression.c
 *
 * Defines functions to manipulate expressions.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gc.h>
#include "expression.h"

/*
 * Creation
 */

expression exp_var( int i, char* name ) {
  expression res = GC_MALLOC( sizeof( struct expression_s ) );
  res->expression_type = VAR;
  res->child1 = NULL;
  res->child2 = NULL;
  res->index = i;
  res->name = name;
  res->constant = 0;
  return res;
}

expression exp_const( double c ) {
  expression res = GC_MALLOC( sizeof( struct expression_s ) );
  res->expression_type = CONST;
  res->child1 = NULL;
  res->child2 = NULL;
  res->index = 0;
  res->name = NULL;
  res->constant = c;
  return res;
}

expression exp_unary( expression e, enum CASE_EXPRESSION s ) {
  expression res = GC_MALLOC( sizeof( struct expression_s ) );
  res->expression_type = s;
  res->child1 = e;
  res->child2 = NULL;
  res->index = 0;
  res->name = NULL;
  res->constant = 0;
  return res;
}

expression exp_opp( expression e ) {
  return exp_unary( e, OPP );
}

expression exp_log( expression e ) {
  return exp_unary( e, LOG );
}

expression exp_binary( expression e1, expression e2, enum CASE_EXPRESSION s ) {
  expression res = GC_MALLOC( sizeof( struct expression_s ) );
  res->expression_type = s;
  res->child1 = e1;
  res->child2 = e2;
  res->index = 0;
  res->name = NULL;
  res->constant = 0;
  return res;
}

expression exp_add( expression e1, expression e2 ) {
  return exp_binary( e1, e2, ADD );
}

expression exp_sub( expression e1, expression e2 ) {
  return exp_binary( e1, e2, SUB );
}

expression exp_mul( expression e1, expression e2 ) {
  return exp_binary( e1, e2, MUL );
}

expression exp_div( expression e1, expression e2 ) {
  return exp_binary( e1, e2, DIV );
}

expression exp_pow( expression e1, expression e2 ) {
  return exp_binary( e1, e2, POW );
}

/*
 * Manipulation
 */

expression exp_copy( expression e ) {
  expression res = GC_MALLOC( sizeof( struct expression_s ) );
  res->expression_type = e->expression_type;
  res->child1 = e->child1 == NULL ? NULL : exp_copy( e->child1 ) ;
  res->child2 = e->child2 == NULL ? NULL : exp_copy( e->child2 ) ;
  res->index = e->index;
  if ( e->name ) {
    int n = strlen( e->name );
    res->name = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
    strcpy( res->name, e->name );
  } else
    res->name = NULL;
  res->constant = e->constant;
  return res;
}

bool exp_equal( expression e1, expression e2 ) {
  if ( e1->expression_type != e2->expression_type )
    return false;
  switch ( e1->expression_type ) {
    case VAR:
      return e1->index == e2->index && strcmp( e1->name, e2->name) == 0;
      break;
    case CONST:
      return e1->constant == e2->constant;
      break;
    case OPP:
      return exp_equal( e1->child1, e2->child1 );
      break;
    case LOG:
      return exp_equal( e1->child1, e2->child1 );
      break;
    case ADD:
      return exp_equal( e1->child1, e2->child1 ) &&
             exp_equal( e1->child2, e2->child2 );
      break;
    case SUB:
      return exp_equal( e1->child1, e2->child1 ) &&
             exp_equal( e1->child2, e2->child2 );
      break;
    case MUL:
      return exp_equal( e1->child1, e2->child1 ) &&
             exp_equal( e1->child2, e2->child2 );
      break;
    case DIV:
      return exp_equal( e1->child1, e2->child1 ) &&
             exp_equal( e1->child2, e2->child2 );
      break;
    case POW:
      return exp_equal( e1->child1, e2->child1 ) &&
             exp_equal( e1->child2, e2->child2 );
      break;
    default:
      fprintf( stderr, "exp_equal: Unknown expression type." );
      exit( 1 );
      break;
  }
}

int exp_size( expression e ) {
  switch ( e->expression_type ) {
    case VAR:
      return 0;
      break;
    case CONST:
      return 1;
      break;
    case OPP:
    case LOG:
      return 1 + exp_size( e->child1 );
      break;
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case POW:
      return 1 + exp_size( e->child1 ) + exp_size( e->child2 );
      break;
    default:
      fprintf( stderr, "exp_size: Unknown expression type." );
      exit( 1 );
      break;
  }
}

double exp_eval( expression e, double* sigma ) {
  switch ( e->expression_type ) {
    case VAR:
      return sigma[ e->index ];
      break;
    case CONST:
      return e->constant;
      break;
    case OPP:
      return - exp_eval( e->child1, sigma );
      break;
    case LOG:
      return log( exp_eval( e->child1, sigma ) );
      break;
    case ADD:
      return exp_eval( e->child1, sigma ) + exp_eval( e->child2, sigma );
      break;
    case SUB:
      return exp_eval( e->child1, sigma ) - exp_eval( e->child2, sigma );
      break;
    case MUL:
      return exp_eval( e->child1, sigma ) * exp_eval( e->child2, sigma );
      break;
    case DIV:
      return exp_eval( e->child1, sigma ) / exp_eval( e->child2, sigma );
      break;
    case POW:
      return pow( exp_eval( e->child1, sigma ), exp_eval( e->child2, sigma ) );
      break;
    default:
      fprintf( stderr, "exp_eval: Unknown expression type." );
      exit( 1 );
      break;
  }
}

expression exp_array_composition( expression e, expression* esub ) {
  switch ( e->expression_type ) {
    case VAR:
      return esub[ e->index ];
      break;
    case CONST:
      return e;
      break;
    case OPP:
      return exp_opp( exp_array_composition( e->child1, esub ) );
      break;
    case LOG:
      return exp_log( exp_array_composition( e->child1, esub ) );
      break;
    case ADD:
      return exp_add( exp_array_composition( e->child1, esub ),
                      exp_array_composition( e->child2, esub ) );
      break;
    case SUB:
      return exp_sub( exp_array_composition( e->child1, esub ),
                      exp_array_composition( e->child2, esub ) );
      break;
    case MUL:
      return exp_mul( exp_array_composition( e->child1, esub ),
                      exp_array_composition( e->child2, esub ) );
      break;
    case DIV:
      return exp_div( exp_array_composition( e->child1, esub ),
                      exp_array_composition( e->child2, esub ) );
      break;
    case POW:
      return exp_pow( exp_array_composition( e->child1, esub ),
                      exp_array_composition( e->child2, esub ) );
      break;
    default:
      fprintf( stderr, "exp_array_composition: Unknown expression type." );
      exit( 1 );
      break;
  }
}

expression exp_simplification( expression e ) {
  expression e1, e2;
  switch ( e->expression_type ) {
    case VAR:
      return e;
      break;
    case CONST:
      return e;
      break;
    case OPP:
      e1 = exp_simplification( e->child1 );
      if ( e1->expression_type == CONST )
        return exp_const( exp_eval( exp_opp( e1 ), NULL ) );
      return exp_opp( e1 );
      break;
    case LOG:
      e1 = exp_simplification( e->child1 );
      if ( e1->expression_type == CONST )
        return exp_const( exp_eval( exp_log( e1 ), NULL ) );
      return exp_log( e1 );
      break;
    case ADD:
      e1 = exp_simplification( e->child1 );
      e2 = exp_simplification( e->child2 );
      if ( e1->expression_type == CONST && e2->expression_type == CONST )
        return exp_const( exp_eval( exp_add( e1, e2 ), NULL ) );
      else if ( e1->expression_type == CONST && e1->constant == 0 )
        return e2;
      else if ( e2->expression_type == CONST && e2->constant == 0 )
        return e1;
      return exp_add( e1, e2 );
      break;
    case SUB:
      e1 = exp_simplification( e->child1 );
      e2 = exp_simplification( e->child2 );
      if ( e1->expression_type == CONST && e2->expression_type == CONST )
        return exp_const( exp_eval( exp_sub( e1, e2 ), NULL ) );
      else if ( e1->expression_type == CONST && e1->constant == 0 )
        return exp_opp( e2 );
      else if ( e2->expression_type == CONST && e2->constant == 0 )
        return e1;
      return exp_sub( e1, e2 );
      break;
    case MUL:
      e1 = exp_simplification( e->child1 );
      e2 = exp_simplification( e->child2 );
      if ( e1->expression_type == CONST && e2->expression_type == CONST )
        return exp_const( exp_eval( exp_mul( e1, e2 ), NULL ) );
      else if ( e1->expression_type == CONST && e1->constant == 1 )
        return e2;
      else if ( e1->expression_type == CONST && e1->constant == 0 )
        return exp_const( 0 );
      else if ( e2->expression_type == CONST && e2->constant == 1 )
        return e1;
      else if ( e2->expression_type == CONST && e2->constant == 0 )
        return exp_const( 0 );
      return exp_mul( e1, e2 );
      break;
    case DIV:
      e1 = exp_simplification( e->child1 );
      e2 = exp_simplification( e->child2 );
      if ( e1->expression_type == CONST && e2->expression_type == CONST )
        return exp_const( exp_eval( exp_div( e1, e2 ), NULL ) );
      else if ( e2->expression_type == CONST && e2->constant == 1 )
        return e1;
      else if ( e1->expression_type == CONST && e1->constant == 0 )
        return exp_const( 0 );
      return exp_div( e1, e2 );
      break;
    case POW:
      e1 = exp_simplification( e->child1 );
      e2 = exp_simplification( e->child2 );
      if ( e1->expression_type == CONST && e2->expression_type == CONST )
        return exp_const( exp_eval( exp_pow( e1, e2 ), NULL ) );
      else if ( e1->expression_type == CONST && e1->constant == 0 )
        return exp_const( 0 );
      else if ( e1->expression_type == CONST && e1->constant == 1 )
        return exp_const( 1 );
      else if ( e2->expression_type == CONST && e2->constant == 0 )
        return exp_const( 1 );
      else if ( e2->expression_type == CONST && e2->constant == 1 )
        return e1;
      return exp_pow( e1, e2 );
      break;
    default:
      fprintf( stderr, "exp_simplification: Unknown expression type." );
      exit( 1 );
      break;
  }
}

// TODO
// change this back to using no auxiliary function
expression exp_derivative_aux( expression e, int i, int n ) {
  expression res;
  switch ( e->expression_type ) {
    case VAR:
      if ( e->index == i )
        res = exp_const( 1 );
      else {
        res = exp_const( 0 );
      }
      break;
    case CONST:
      res = exp_const( 0 );
      break;
    case OPP:
      res = exp_opp( exp_derivative_aux( exp_copy( e->child1 ), i, n + 1 ) );
      break;
    case LOG:
      return exp_div( exp_derivative( e->child1, i ), e->child1 );
      break;
    case ADD:
      res = exp_add( exp_derivative_aux( exp_copy( e->child1 ), i, n + 1 ),
                     exp_derivative_aux( exp_copy( e->child2 ), i, n + 1 ) );
      break;
    case SUB:
      res = exp_sub( exp_derivative_aux( exp_copy( e->child1 ), i, n + 1 ),
                     exp_derivative_aux( exp_copy( e->child2 ), i, n + 1 ) );
      break;
    case MUL:
      res = exp_add( exp_mul( exp_derivative_aux( exp_copy( e->child1 ), i, n + 1 ), exp_copy( e->child2 ) ),
                     exp_mul( exp_derivative_aux( exp_copy( e->child2 ), i, n + 1 ), exp_copy( e->child1 ) ) );
      break;
    case DIV:
      res = exp_div( exp_sub( exp_mul( exp_derivative_aux( exp_copy( e->child1 ), i, n + 1 ),
                                        exp_copy( e->child2 ) ),
                              exp_mul( exp_derivative_aux( exp_copy( e->child2 ), i, n + 1 ),
                                       exp_copy( e->child1 ) ) ),
                     exp_mul( exp_copy( e->child2 ), exp_copy( e->child2 ) ) );
      break;
    case POW:
      return exp_add( exp_mul( exp_log( e->child1 ),
                               exp_mul( exp_derivative_aux( e->child2, i, n + 1 ),
                                        exp_pow( e->child1, e->child2 ) ) ),
                      exp_mul( exp_pow( e->child1,
                                        exp_sub( e->child2, exp_const( 1 ) ) ),
                               exp_mul( e->child2,
                                        exp_derivative_aux( e->child1, i, n + 1 ) ) ) );
      break;
    default:
      fprintf( stderr, "exp_derivative: Unknown expression type." );
      exit( 1 );
      break;
  }
  return res;
}

expression exp_derivative( expression e, int i ) {
  return exp_derivative_aux( e, i, 0 );
}

/*
 * Printing
 */

void exp_printf( expression e ) {
  int n = exp_snprintf( NULL, 0, e );
  char* s = GC_MALLOC( (n + 1) * sizeof( char ) );
  exp_snprintf( s, n + 1, e );
  printf( "%s", s );
}

int exp_snprintf( char* buf, int length, expression e ) {
  if ( e == NULL ) {
    return snprintf( buf, length, "NULL" );
  }
  int total = 0, diff = 0;
  switch ( e->expression_type ) {
    case VAR:
      // TODO
      // change this back at some point?
      return snprintf( buf, length, "x_%d", e->index );
      return snprintf( buf, length, "%s", e->name );
      break;
    case CONST:
      return snprintf( buf, length, "%f", e->constant );
      break;
    case OPP:
      diff = snprintf( buf, length, "- (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case LOG:
      diff = snprintf( buf, length, "log (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case ADD:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") + (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case SUB:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") - (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case MUL:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") * (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case DIV:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") / (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    case POW:
      diff = snprintf( buf, length, "(" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child1 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ") ^ (" );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = exp_snprintf( buf, length, e->child2 );
      total += diff;
      buf += diff;
      length -= diff;
      if ( length < 0 ) length = 0;
      diff = snprintf( buf, length, ")" );
      total += diff;
      return total;
      break;
    default:
      fprintf( stderr, "exp_snprintf: Unknown expression type." );
      exit( 1 );
      break;
  }
}

enum CASE_EXPRESSION exp_type_lexer( char* token ) {
  if      ( strcmp( token, "var" )   == 0 ) return VAR;
  else if ( strcmp( token, "const" ) == 0 ) return CONST;
  else if ( strcmp( token, "opp" )   == 0 ) return OPP;
  else if ( strcmp( token, "log" )   == 0 ) return LOG;
  else if ( strcmp( token, "add" )   == 0 ) return ADD;
  else if ( strcmp( token, "sub" )   == 0 ) return SUB;
  else if ( strcmp( token, "mul" )   == 0 ) return MUL;
  else if ( strcmp( token, "div" )   == 0 ) return DIV;
  else if ( strcmp( token, "pow" )   == 0 ) return POW;
  fprintf( stderr, "exp_type_lexer: Unknown token %s.\n", token );
  exit( 1 );
}

expression cJSON_to_exp( cJSON* json ) {
  cJSON* json1 = cJSON_GetObjectItemCaseSensitive( json, "type" );
  if ( json1 == NULL ) {
    fprintf( stderr, "cJSON_to_exp: no \"type\" specified in expression %s\n",
             cJSON_Print( json ) );
    exit( 1 );
  }
  char* type;
  if ( cJSON_IsString( json1 ) )
    type = json1->valuestring;
  else {
    fprintf( stderr, "cJSON_to_exp: \"type\" is not a string in expression "
             "%s\n", cJSON_Print( json ) );
    exit( 1 );
  }
  char* name = NULL;
  int n;
  enum CASE_EXPRESSION exp_type = exp_type_lexer( type );
  cJSON* json2;
  switch ( exp_type ) {
    case VAR:
      json1 = cJSON_GetObjectItemCaseSensitive( json, "index" );
      if ( json1 == NULL ) {
        fprintf( stderr, "cJSON_to_exp: no \"index\" specified in variable "
                 "%s\n", cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsNumber( json1 ) ) {
        fprintf( stderr, "cJSON_to_exp: \"index\" is not a number in variable "
                 "%s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      json2 = cJSON_GetObjectItemCaseSensitive( json, "name" );
      if ( json2 == NULL ) {
        json2 = cJSON_GetObjectItemCaseSensitive( json, "wire_index" );
        if ( json2 == NULL ) {
          fprintf( stderr, "cJSON_to_exp: no \"name\" or \"wire_index\" "
                   "specified in variable %s\n", cJSON_Print( json ) );
          exit( 1 );
        } else if ( ! cJSON_IsNumber( json2 ) ) {
          fprintf( stderr, "cJSON_to_exp: \"wire_index\" is not a number in "
                   "variable %s\n", cJSON_Print( json ) );
          exit( 1 );
        } else if ( json1->valueint == 2 * json2->valueint ) {
          // fprintf( stderr, "cJSON_to_exp: no \"name\" specified, using "
          //          "\"wire_index\"\n" );
          n = snprintf( name, 0, "%d+", json2->valueint );
          name = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
          snprintf( name, n, "%d+", json2->valueint );
          // TODO
          // make sure this is okay
          return exp_var( json2->valueint, name );
          // return exp_var( json1->valueint, name );
        } else if ( json1->valueint == 2 * json2->valueint + 1 ) {
          // fprintf( stderr, "cJSON_to_exp: no \"name\" specified, using "
          //          "\"wire_index\"\n" );
          n = snprintf( name, 0, "%d-", json2->valueint );
          name = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
          snprintf( name, n, "%d-", json2->valueint );
          // TODO
          // make sure this is okay
          return exp_var( json2->valueint, name );
          // return exp_var( json1->valueint, name );
        } else {
          fprintf( stderr, "cJSON_to_exp: incompatible \"index\" (%d) and "
                   "\"wire_index\" (%d).\n", json1->valueint, json2->valueint );
          exit( 1 );
        }
      }
      fprintf( stderr, "cJSON_to_exp: \"name\" specified in variable.\n" );
      fprintf( stderr, "cJSON_to_exp: I hoped we wouldn't fall in this "
               "case.\n" );
      exit( 1 );
      return exp_var( json1->valueint, json2->valuestring );
      break;
    case CONST:
      json1 = cJSON_GetObjectItemCaseSensitive( json, "constant" );
      if ( json1 == NULL ) {
        fprintf( stderr, "cJSON_to_exp: no \"constant\" specified in constant "
                 "%s\n", cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsNumber( json1 ) ) {
        fprintf( stderr, "cJSON_to_exp: \"constant\" is not a number in "
                 "constant %s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      return exp_const( json1->valuedouble );
      break;
    case OPP:
    case LOG:
      json1 = cJSON_GetObjectItemCaseSensitive( json, "expression1" );
      if ( json1 == NULL ) {
        fprintf( stderr, "cJSON_to_exp: no \"expression1\" specified in unary "
                 "expression %s\n", cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsObject( json1 ) ) {
        fprintf( stderr, "cJSON_to_exp: \"expression1\" is not a JSON object "
                 "in unary expression %s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      return exp_unary( cJSON_to_exp( json1 ), exp_type );
      break;
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case POW:
      json1 = cJSON_GetObjectItemCaseSensitive( json, "expression1" );
      if ( json1 == NULL ) {
        fprintf( stderr, "cJSON_to_exp: no \"expression1\" specified in binary "
                 "expression %s\n", cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsObject( json1 ) ) {
        fprintf( stderr, "cJSON_to_exp: \"expression1\" is not a JSON object "
                 "in unary expression %s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      json2 = cJSON_GetObjectItemCaseSensitive( json, "expression2" );
      if ( json2 == NULL ) {
        fprintf( stderr, "cJSON_to_exp: no \"expression2\" specified in binary "
                 "expression %s\n", cJSON_Print( json ) );
        exit( 1 );
      } else if ( ! cJSON_IsObject( json2 ) ) {
        fprintf( stderr, "cJSON_to_exp: \"expression2\" is not a JSON object "
                 "in unary expression %s\n", cJSON_Print( json ) );
        exit( 1 );
      }
      return exp_binary( cJSON_to_exp( json1 ), cJSON_to_exp( json2 ),
                         exp_type );
      break;
    default:
      fprintf( stderr, "cJSON_to_exp: Unknown expression type." );
      exit( 1 );
      break;
  }
}

/*
 * Arrays
 */

expression* exp_mul_1_many( expression e, int n, expression* es ) {
  expression* res = GC_MALLOC( n * sizeof( expression ) );
  for ( int i = 0; i < n; ++i )
    res[ i ] = exp_mul( e, es[ i ] );
  return res;
}

expression* exp_mul_array( int n1, expression* es1, int n2, expression* es2 ) {
  expression* res = GC_MALLOC( n1 * n2 * sizeof( expression ) );
  for ( int i = 0; i < n1; ++i )
    for ( int j = 0; j < n2; ++j )
      res[ i * n2 + j ] = exp_mul( es1[ i ], es2[ j ] );
  return res;
}

expression* exp_cup_1_many( expression e, int n, expression* es ) {
  expression* res = GC_MALLOC( n * sizeof( expression ) );
  for ( int i = 0; i < n; ++i )
    res[ i ] = exp_sub( exp_add( e, es[ i ] ),
                        exp_mul( e, es[ i ] ) );
  return res;
}

expression* exp_cup_array( int n1, expression* es1, int n2, expression* es2 ) {
  expression* res = GC_MALLOC( n1 * n2 * sizeof( expression ) );
  for ( int i = 0; i < n1; ++i )
    for ( int j = 0; j < n2; ++j )
      res[ i * n2 + j ] = exp_sub( exp_add( es1[ i ], es2[ j ] ),
                                   exp_mul( es1[ i ], es2[ j ] ) );
  return res;
}
