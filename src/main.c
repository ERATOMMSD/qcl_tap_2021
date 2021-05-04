#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <gc.h>
#include <cjson/cJSON.h>
#include "utils.h"
#include "expression.h"
#include "expression_test.h"
#include "formula.h"
#include "formula_test.h"
#include "sequent.h"
#include "sequent_test.h"
#include "logic.h"
#include "logic_test.h"
#include "proof.h"
#include "proof_test.h"
#include "optimisation.h"
#include "fault_tree.h"
#include "fault_tree_test.h"

#define VERSION_NUMBER       0
#define SUBVERSION_NUMBER    0
#define SUBSUBVERSION_NUMBER 1

int main( int argc, char** argv )
{
  // srand( 0 );
  args a = args_new();
  args_read( a, argc, argv );
  if ( a->test ) {
    if ( (a->test >> TEST_EXPRESSION_BIT) % 2 )
      expression_test();
    if ( (a->test >> TEST_FORMULA_BIT) % 2 )
      formula_test();
    if ( (a->test >> TEST_SEQUENT_BIT) % 2 )
      sequent_test();
    if ( (a->test >> TEST_PROOF_BIT) % 2 )
      proof_test();
    if ( (a->test >> TEST_FAULT_TREE_BIT) % 2 )
      fault_tree_test();
    if ( (a->test >> TEST_LOGIC_BIT) % 2 )
      logic_test();
  } else if ( strcmp( a->mode, ARGS_MODE_BENCHMARK_STR ) == 0 ) {
    fprintf( stderr, "No benchmark implemented.\n" );
    exit( 1 );
  } else if ( a->mode != NULL ) {
    FILE* f = fopen( a->filename, "r" );
    if ( f == NULL ) {
      fprintf( stderr, "Error while opening: \"%s\"\n", a->filename );
      exit( 1 );
    }
    fseek( f, 0, SEEK_END );
    int n = ftell( f );
    char* contents = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
    fseek( f, 0, SEEK_SET );
    fread( contents, 1, n, f );
    fclose( f );
    contents[ n ] = '\0';
    if ( strcmp( a->mode, ARGS_MODE_PROPAGATE_STR ) == 0 ) {
      cJSON* json = cJSON_Parse( contents );
      // reading the fault tree
      fault_tree ft =
        cJSON_to_fltt( cJSON_GetObjectItemCaseSensitive( json, "ft" ) );
      // reading the base point
      cJSON* point = cJSON_GetObjectItemCaseSensitive( json, "point" );
      cJSON* coord;
      int n = -1;
      cJSON_ArrayForEach( coord, point ) {
        int m = cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint;
        if ( n < m ) n = m;
      }
      double* sigma = GC_MALLOC( n * sizeof( double ) );
      cJSON_ArrayForEach( coord, point ) {
        sigma[ cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint ] =
          cJSON_GetObjectItemCaseSensitive( coord, "value" )->valuedouble;
      }
      cJSON_Delete( json );
      // computing the result
      double res = fltt_propagate_prob( ft, sigma );
      printf( "%.*f\n", DBL_DIG, res );
    } else if ( strcmp( a->mode, ARGS_MODE_SPLITS_STR ) == 0 ) {
      cJSON* json = cJSON_Parse( contents );
      // reading the fault tree
      fault_tree ft =
        cJSON_to_fltt( cJSON_GetObjectItemCaseSensitive( json, "ft" ) );
      // reading the confidence functions
      cJSON* cfd_funcs = cJSON_GetObjectItemCaseSensitive( json, "conf_funcs" );
      cJSON* coord;
      int n = -1;
      cJSON_ArrayForEach( coord, cfd_funcs ) {
        int m = cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint;
        if ( n < m ) n = m;
      }
      expression* cfd_res = GC_MALLOC( ( n + 1 ) * sizeof( expression ) );
      cJSON_ArrayForEach( coord, cfd_funcs ) {
        cfd_res[ cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint ] =
          cJSON_to_exp( cJSON_GetObjectItemCaseSensitive( coord, "expression" ) );
      }
      // reading the base point
      cJSON* point = cJSON_GetObjectItemCaseSensitive( json, "point" );
      n = -1;
      cJSON_ArrayForEach( coord, point ) {
        int m = cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint;
        if ( n < m ) n = m;
      }
      double* sigma = GC_MALLOC( ( n + 1 ) * sizeof( double ) );
      cJSON_ArrayForEach( coord, point ) {
        sigma[ cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint ] =
          cJSON_GetObjectItemCaseSensitive( coord, "value" )->valuedouble;
      }
      // reading the total resource allowance
      double res =
        cJSON_GetObjectItemCaseSensitive( json, "resources" )->valuedouble;
      cJSON_Delete( json );
      // defining the base logic
      enum TRUTH_VALUE neg_table[ TRUTH_VALUE_CASES ] = { TRUE, UNDET, FALSE };
      enum TRUTH_VALUE imp_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
        { TRUE, TRUE, TRUE, UNDET, UNDET, TRUE, FALSE, UNDET, TRUE };
      enum TRUTH_VALUE disj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
        { FALSE, UNDET, TRUE, UNDET, UNDET, TRUE, TRUE, TRUE, TRUE };
      enum TRUTH_VALUE conj_table[ TRUTH_VALUE_CASES * TRUTH_VALUE_CASES ] =
        { FALSE, FALSE, FALSE, FALSE, UNDET, UNDET, FALSE, UNDET, TRUE };
      logical_operator neg  = table_to_lo( 1, neg_table );
      logical_operator imp  = table_to_lo( 2, imp_table );
      logical_operator disj = table_to_lo( 2, disj_table );
      logical_operator conj = table_to_lo( 2, conj_table );
      logic log = los_to_log( neg, imp, disj, conj );
      // defining the optimisation parameters
      gradient_ascent_params ga_params =
        GC_MALLOC( sizeof( struct gradient_ascent_params_s ) );
      ga_params->step_coef = res / 100;
      hill_climbing_params hc_params =
        GC_MALLOC( sizeof( struct hill_climbing_params_s ) );
      hc_params->step_size = res / 100;
      hc_params->iters = 100;
      optim_params opt_params =
        filename_to_optim_params( a->filename_optimisation );
      // selecting the algorithm
      // opt_params->opt_algo_type = GRAD_ASC;
      // opt_params->opt_algo_type = GRAD_ASC_HILL_CLIMB;
      // opt_params->opt_algo_type = SIM_ANNEAL;
      opt_params->opt_algo_type = SIM_ANNEAL_HILL_CLIMB;
      opt_params->ga_params = ga_params;
      opt_params->hc_params = hc_params;
      opt_params->sa_params = sa_params_from_hyperparams_simple;
      // computing the result
      double* result = resource_repartition( n + 1,
                                             fltt_to_prf( n + 1, ft, log ),
                                             cfd_res, sigma, res, opt_params );
      double total_allowance_spent = 0;
      for ( int i = 0; i <= n; ++i ) {
        if ( result[ i ] < 0 ) {
          fprintf( stderr, "Negative split (%d coordinate): %f.\n",
                   i, result[ i ] );
          exit( 1 );
        }
        total_allowance_spent += result[ i ];
      }
      // if ( dcompare( total_allowance_spent, res, n+1 ) ) {
      //   fprintf( stderr, "Spent allowance difference from specified: %f "
      //            "(instead of %f).\n", total_allowance_spent, res );
      //   exit( 1 );
      // }
      for ( int i = 0; i <= n; ++i )
        printf( "%d=%.*f ", i, DBL_DIG, result[ i ] );
      printf( "\n" );
    } else {
      fprintf( stderr, "Unknown mode (%s).\n", a->mode );
      exit( 1 );
    }
  } else {
    fprintf( stderr, "No mode specified.\n" );
    exit( 1 );
  }
  return 0;
}
