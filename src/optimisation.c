/********************************************************************
 * optimisation.c
 *
 * Defines functions to optimise the confidence in a proof.
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <gc.h>
#include "utils.h"
#include "expression.h"
#include "formula.h"
#include "sequent.h"
#include "proof.h"
#include "optimisation.h"
#include <stdio.h>

/*
 * Algorithms
 */

// sa_params_from_hyperparams_simple

simulated_annealing_params sa_params_from_hyperparams_simple;

double sa_params_from_hyperparams_simple_t_max;
double sa_params_from_hyperparams_simple_ratio0;
double sa_params_from_hyperparams_simple_delta_factor;
double* sa_params_from_hyperparams_simple_starting_point;

double* sa_params_from_hyperparams_simple_init
  ( int n, double* sigma, double all ) {
  double* init = GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < n; ++i )
    init[ i ] = sigma[ i ] + all / n;
  return init;
}

double sa_params_from_hyperparams_simple_temp ( int k ) {
  return log( 2 ) * sa_params_from_hyperparams_simple_t_max / log( k + 1 );
}

double sa_params_from_hyperparams_simple_energy ( double cfd ) {
  return - cfd;
}

double sa_params_from_hyperparams_simple_prob
  ( double energy_old, double energy_new, double temp ) {
  if ( energy_new <= energy_old )
    return 1;
  return exp( ( energy_old - energy_new ) / temp );
}

// we take a Gaussian in an orthonormal basis B of H_0 and add it to the current
// point.
// H_0 = { (x_1,...,x_n) | sum x_i = 0 }
// B = (v_0,...,v_{n-1}), with (v_i)_j =
// - 1 / sqrt((i+1) + (i+1)^2) if j < i+1,
// - - (i+1) / sqrt((i+1) + (i+1)^2) if j = i+1,
// - 0 if j > i+1.
double* sa_params_from_hyperparams_simple_neighbour ( int n, double* point ) {
  bool reject = true;
  double x, y;
  double* coefs = GC_MALLOC( ( n - 1 ) * sizeof( double ) );
  double* new = GC_MALLOC( n * sizeof( double ) );

  double* lambdas = GC_MALLOC( ( n - 2 ) * sizeof( double ) );
  for ( int i = 0; i < n - 1; ++i )
    lambdas[ i ] = 1 / sqrt( ( i + 1 ) + ( i + 1 ) * ( i + 1 ) );

  while ( reject ) {
    for ( int i = 0; i < n; ++i ) {
      new[ i ] = point[ i ];
    }
    for ( int i = 0; i < ( n - 1 ) / 2; ++i ) {
      x = ( double ) rand() / RAND_MAX;
      y = ( double ) rand() / RAND_MAX;
      box_muller( x, y, coefs + ( 2 * i ) );
    }
    if ( n % 2 == 0 ) {
      x = ( double ) rand() / RAND_MAX;
      y = ( double ) rand() / RAND_MAX;
      box_muller( x, y, coefs + n - 3 ); // overwrites the penultimate value,
                                         // but who cares?
    }
    for ( int i = 0; i < n - 1; ++i ) {
      for ( int j = 0; j < i + 1; ++j ) {
        new[ j ] +=
          sa_params_from_hyperparams_simple_delta_factor *
          lambdas[ i ] * coefs[ i ];
      }
      new[ i + 1 ] -=
        sa_params_from_hyperparams_simple_delta_factor *
        ( i + 1 ) * lambdas[ i ] * coefs[ i ];
    }
    reject = false;
    for ( int i = 0; i < n; ++i )
      if ( new[ i ] < sa_params_from_hyperparams_simple_starting_point[ i ] ) {
        reject = true;
      }
  }
  return new;
}

void sa_params_from_hyperparams_simple_set( int max_step, double p_init, int k0,
                                            int n, double* sigma ) {
  sa_params_from_hyperparams_simple_delta_factor = sqrt( M_PI / max_step );
  sa_params_from_hyperparams_simple_ratio0 = k0 / max_step;
  sa_params_from_hyperparams_simple_t_max = - 1 / log( 1 - p_init );
  sa_params_from_hyperparams_simple_starting_point =
    GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < n; ++i )
    sa_params_from_hyperparams_simple_starting_point[ i ] = sigma[ i ];
  sa_params_from_hyperparams_simple =
    GC_MALLOC( sizeof( struct simulated_annealing_params_s ) );
  sa_params_from_hyperparams_simple->max_step = max_step;
  sa_params_from_hyperparams_simple->init =
    sa_params_from_hyperparams_simple_init;
  sa_params_from_hyperparams_simple->temp =
    sa_params_from_hyperparams_simple_temp;
  sa_params_from_hyperparams_simple->energy =
    sa_params_from_hyperparams_simple_energy;
  sa_params_from_hyperparams_simple->prob =
    sa_params_from_hyperparams_simple_prob;
  sa_params_from_hyperparams_simple->neighbour =
    sa_params_from_hyperparams_simple_neighbour;
}

void sa_params_from_hyperparams_simple_set_with_lambda(
  int max_step, double p_init, int k0, int n, double* sigma, double lambda,
  double function_constant ) {
  sa_params_from_hyperparams_simple_delta_factor =
    lambda * sqrt( M_PI / max_step );
  sa_params_from_hyperparams_simple_ratio0 = k0 / max_step;
  sa_params_from_hyperparams_simple_starting_point =
    GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < n; ++i )
    sa_params_from_hyperparams_simple_starting_point[ i ] = sigma[ i ];
  sa_params_from_hyperparams_simple_t_max = 0;
  for ( int i = 0; i < n; ++i )
    sa_params_from_hyperparams_simple_t_max +=
      pow( function_constant,
           sa_params_from_hyperparams_simple_starting_point[ i ] + 1 );
  sa_params_from_hyperparams_simple_t_max *=
    log( function_constant ) * sa_params_from_hyperparams_simple_delta_factor /
    log( p_init );
  sa_params_from_hyperparams_simple =
    GC_MALLOC( sizeof( struct simulated_annealing_params_s ) );
  sa_params_from_hyperparams_simple->max_step = max_step;
  sa_params_from_hyperparams_simple->init =
    sa_params_from_hyperparams_simple_init;
  sa_params_from_hyperparams_simple->temp =
    sa_params_from_hyperparams_simple_temp;
  sa_params_from_hyperparams_simple->energy =
    sa_params_from_hyperparams_simple_energy;
  sa_params_from_hyperparams_simple->prob =
    sa_params_from_hyperparams_simple_prob;
  sa_params_from_hyperparams_simple->neighbour =
    sa_params_from_hyperparams_simple_neighbour;
}

optim_params cJSON_to_optim_params( cJSON* json ) {
  optim_params opt_params = GC_MALLOC( sizeof( struct optim_params_s ) );
  char* s = cJSON_GetObjectItemCaseSensitive( json, "type" )->valuestring;

  if ( strcmp( s, "ga" ) == 0 ) {
    opt_params->opt_algo_type = GRAD_ASC;
  } else if ( strcmp( s, "gahc" ) == 0 ) {
    opt_params->opt_algo_type = GRAD_ASC_HILL_CLIMB;
  } else if ( strcmp( s, "sa" ) == 0 ) {
    opt_params->opt_algo_type = SIM_ANNEAL;
  } else if ( strcmp( s, "sahc" ) == 0 ) {
    opt_params->opt_algo_type = SIM_ANNEAL_HILL_CLIMB;
  } else {
    fprintf( stderr, "Unknown optimisation algorithm (%s).\n", s );
    exit( 1 );
  }

  if ( opt_params->opt_algo_type == SIM_ANNEAL ||
       opt_params->opt_algo_type == SIM_ANNEAL_HILL_CLIMB ) {
    cJSON* json_sa = cJSON_GetObjectItemCaseSensitive( json, "sa" );
    if ( json_sa == NULL ) {
      fprintf( stderr, "No simulated annealing arguments provided.\n" );
      exit( 1 );
    }
    int max_step =
      cJSON_GetObjectItemCaseSensitive( json_sa, "max_step" )->valueint;
    double p_init =
      cJSON_GetObjectItemCaseSensitive( json_sa, "p_init" )->valuedouble;
    int k0 = 0;
    if ( cJSON_GetObjectItemCaseSensitive( json_sa, "k0" ) != NULL )
      k0 = cJSON_GetObjectItemCaseSensitive( json_sa, "k0" )->valueint;
    double lambda =
      cJSON_GetObjectItemCaseSensitive( json_sa, "lambda" )->valuedouble;
    double function_constant =
      cJSON_GetObjectItemCaseSensitive( json_sa,
                                        "function_constant" )->valuedouble;
    int n = -1;
    cJSON* point = cJSON_GetObjectItemCaseSensitive( json_sa, "point" );
    cJSON* coord;
    cJSON_ArrayForEach( coord, point ) {
      int m = cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint;
      if ( n < m ) n = m;
    }
    double* sigma = GC_MALLOC( n * sizeof( double ) );
    cJSON_ArrayForEach( coord, point ) {
      sigma[ cJSON_GetObjectItemCaseSensitive( coord, "index" )->valueint ] =
        cJSON_GetObjectItemCaseSensitive( coord, "value" )->valuedouble;
    }
    sa_params_from_hyperparams_simple_set_with_lambda( max_step, p_init, k0,
                                                       n+1, sigma, lambda,
                                                       function_constant );
    opt_params->sa_params = sa_params_from_hyperparams_simple;

  } else if ( opt_params->opt_algo_type == GRAD_ASC ||
              opt_params->opt_algo_type == GRAD_ASC_HILL_CLIMB ) {
    fprintf( stderr, "Not implemented yet.\n" );
    exit( 1 );

  } else {
    fprintf( stderr, "Unknown optimisation algorithm (coded %d).\n",
             opt_params->opt_algo_type );
    exit( 1 );

  }

  if ( opt_params->opt_algo_type == GRAD_ASC_HILL_CLIMB ||
       opt_params->opt_algo_type == SIM_ANNEAL_HILL_CLIMB ) {
    // may be useful in the future?
    // cJSON* json_hc = cJSON_GetObjectItemCaseSensitive( json, "hc" );
    // if ( json_hc == NULL ) {
    //   fprintf( stderr, "No simulated annealing arguments provided.\n" );
    //   exit( 1 );
    // }
    // int step_size =
    //   cJSON_GetObjectItemCaseSensitive( json_sa, "step_size" )->valueint;
    // int iters =
    //   cJSON_GetObjectItemCaseSensitive( json_sa, "iters" )->valueint;
  }

  return opt_params;
}

optim_params filename_to_optim_params( char* filename ) {
  if ( filename == NULL ) {
    fprintf( stderr, "No optimisation parameters provided.\n" );
    exit( 1 );
  }
  FILE* f_opt = fopen( filename, "r" );
  if ( f_opt == NULL ) {
    fprintf( stderr, "Error while opening: \"%s\"\n",
             filename );
    exit( 1 );
  }
  fseek( f_opt, 0, SEEK_END );
  int n = ftell( f_opt );
  char* contents = GC_MALLOC( ( n + 1 ) * sizeof( char ) );
  fseek( f_opt, 0, SEEK_SET );
  fread( contents, 1, n, f_opt );
  fclose( f_opt );
  contents[ n ] = '\0';
  cJSON* json = cJSON_Parse( contents );
  optim_params opt_params = cJSON_to_optim_params( json );
  cJSON_Delete( json );
  return opt_params;
}

/*
 * Algorithms
 */

/*
 * best_vector_res_aux: uses precomputed derivatives.
 * inputs:
 * - [n]: the dimension of the ambient space,
 * - [pos_cfd_num]: the number of positive confidence expressions in the
 *   conclusion of the proof,
 * - [exp_res]: an array of [pos_cfd_num] expressions mapping resources to
 *   confidence in the total proof (its variables are 0 to [n]-1),
 * - [derivatives]: a 2-dimensional array of size [pos_cfd_num] * [n]
 *   representing the partial derivatives of [exp_res],
 * - [sigma]: a vector of size [n] of doubles representing resources.
 * output: a vector of doubles of size [n] representing the direction.
 */
double* best_vector_res_aux( int n, int pos_cfd_num, expression* exp_res,
                             expression** derivatives, double* sigma ) {
  int max_ind = 0; // value stored in [exp_is_max] if the expression is maximal
  int max_size = 0;
  double val_max = 0;
  int* exp_is_max = GC_MALLOC( pos_cfd_num * sizeof( expression ) );
  // this loop puts in exp_max all expressions that are maximal at sigma
  for ( int i = 0; i < pos_cfd_num; ++i ) {
    int cur_size = max_size;
    if ( cur_size < exp_size( exp_res[ i ] ) )
      cur_size = exp_size( exp_res[ i ] );
    double val_cur = exp_eval( exp_res[ i ], sigma );
    int c = dcompare( val_cur, val_max, cur_size );
    if ( c == 0 ) {
      max_size = cur_size;
      exp_is_max[ i ] = max_ind;
      val_max = val_cur;
    } else if ( c > 0 ) {
      max_size = exp_size( exp_res[ i ] );
      exp_is_max[ i ] = ++max_ind;
      val_max = val_cur;
    } else {
      exp_is_max[ i ] = -1;
    }
  }
  double* best_direction = GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < n; ++i ) best_direction[ i ] = 0;
  double best_norm = 0;
  double* cur_direction  = GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < pos_cfd_num; ++i ) {
    if ( exp_is_max[ i ] == max_ind ) {
      for ( int j = 0; j < n; ++j ) {
        cur_direction[ j ] = exp_eval( derivatives[ i ][ j ], sigma );
      }
      double cur_norm = square_norm( n, cur_direction );
      if ( cur_norm > best_norm ) {
        best_norm = cur_norm;
        best_direction = cur_direction;
        cur_direction = GC_MALLOC( n * sizeof( double ) );
      }
    }
  }
  return best_direction;
}

double* best_vector_res( int n, proof p, expression* cfd_res,
                         double* sigma ) {
  expression* exp_res = GC_MALLOC( p->conclusion->pos_cfd_num *
                                   sizeof( expression ) );
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
    exp_res[ i ] = exp_array_composition( p->conclusion->ccl_pos_cfds[ i ],
                                          cfd_res );
  int num_max = 0;
  int max_size = 0;
  double val_max = 0;
  expression* exp_max = GC_MALLOC( p->conclusion->pos_cfd_num *
                                   sizeof( expression ) );
  // this loop puts in exp_max all expressions that are maximal at sigma
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i ) {
    int cur_size = max_size;
    if ( cur_size < exp_size( exp_res[ i ] ) )
      cur_size = exp_size( exp_res[ i ] );
    double val_cur = exp_eval( exp_res[ i ], sigma );
    int c = dcompare( val_cur, val_max, cur_size );
    if ( c == 0 ) {
      max_size = cur_size;
      exp_max[ num_max++ ] = exp_res[ i ];
      val_max = val_cur;
    } else if ( c > 0 ) {
      num_max = 0;
      max_size = exp_size( exp_res[ i ] );
      exp_max[ num_max++ ] = exp_res[ i ];
      val_max = val_cur;
    }
  }
  double* best_direction = GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < n; ++i ) best_direction[ i ] = 0;
  double best_norm = 0;
  double* cur_direction  = GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < num_max; ++i ) {
    for ( int j = 0; j < n; ++j ) {
      cur_direction[ j ] = exp_eval( exp_derivative( exp_max[ i ], j ), sigma );
    }
    double cur_norm = square_norm( n, cur_direction );
    if ( cur_norm > best_norm ) {
      best_norm = cur_norm;
      best_direction = cur_direction;
      cur_direction = GC_MALLOC( n * sizeof( double ) );
    }
  }
  return best_direction;
}

double* resource_repartition_ga( int n, proof p, expression* cfd_res,
                                 double* sigma, double res,
                                 gradient_ascent_params params ) {
  int iters = 0;
  expression* exp_res = GC_MALLOC( p->conclusion->pos_cfd_num *
                                   sizeof( expression ) );
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
    exp_res[ i ] = exp_array_composition( p->conclusion->ccl_pos_cfds[ i ],
                                          cfd_res );
  expression** derivatives = GC_MALLOC( p->conclusion->pos_cfd_num *
                                        sizeof( expression* ) );
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i ) {
    derivatives[ i ] = GC_MALLOC( n * sizeof( expression ) );
    for ( int j = 0; j < n; ++j )
      derivatives[ i ][ j ] =
        exp_simplification( exp_derivative( exp_res[ i ], j ) );
  }
  double* cur_sigma = GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < n; ++i )
    cur_sigma[ i ] = sigma[ i ];
  double total_res = l1_norm( n, sigma ) + res;
  double alpha, norm_dsigma;
  double norm_cur_sigma = l1_norm( n, cur_sigma );
  double* dsigma;
  bool stop = false;
  while ( !stop ) {
    iters++;
    dsigma = best_vector_res_aux( n, p->conclusion->pos_cfd_num, exp_res,
                                  derivatives, cur_sigma );
    alpha = params->step_coef;
    norm_dsigma = l1_norm( n, dsigma );
    if ( norm_cur_sigma + alpha > total_res ) {
      alpha = total_res - norm_cur_sigma;
      if ( alpha < 0 ) {
        alpha = 0;
      }
      stop = true;
    }
    if ( norm_dsigma == 0 ) {
      printf( "sigma = ( " );
      for ( int i = 0; i < n; ++i )
        printf( "%f ", sigma[ i ] );
      printf( ")\n" );
      printf( "cur_sigma = ( " );
      for ( int i = 0; i < n; ++i )
        printf( "%f ", cur_sigma[ i ] );
      printf( ")\n" );
      for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
        for ( int j = 0; j < n; ++j ) {
          printf( "derivative[ %d ][ %d ] = ", i, j );
          exp_printf( derivatives[ i ][ j ] );
          printf( "\n" );
          printf( "eval = %f\n", exp_eval( derivatives[ i ][ j ], cur_sigma ) );
        }
      printf( "Division by 0.\n" );
      printf( "dsigma = " );
      for ( int i = 0; i < n; ++i )
        printf( "%f ", dsigma[ i ] );
      printf( "\n" );
      exit( 1 );
    }
    for ( int i = 0; i < n; ++i ) {
      cur_sigma[ i ] += alpha * dsigma[ i ] / norm_dsigma;
      norm_cur_sigma += alpha * dsigma[ i ] / norm_dsigma;
    }
    if ( iters % 1000000 == 0 ) {
      fprintf( stderr, "Too many iterations in resource_repartition_ga.\n" );
      exit( 1 );
    }
  }
  for ( int i = 0; i < n; ++i )
    cur_sigma[ i ] -= sigma[ i ];
  return cur_sigma;
}

double* resource_repartition_gahc( int n, proof p, expression* cfd_res,
                                   double* sigma, double res,
                                   gradient_ascent_params ga_params,
                                   hill_climbing_params hc_params ) {
  // warning: we use < on doubles (rather than dcompare) here for hill climbing
  // to work better
  expression* exp_res = GC_MALLOC( p->conclusion->pos_cfd_num *
                                   sizeof( expression ) );
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
    exp_res[ i ] = exp_array_composition( p->conclusion->ccl_pos_cfds[ i ],
                                          cfd_res );
  double* res_all = resource_repartition_ga( n, p, cfd_res, sigma, res,
                                             ga_params );
  for ( int i = 0; i < n; ++i )
    res_all[ i ] += sigma[ i ];
  double x, y, l2_norm, r, best = 0;
  // dir: the delta between current point and next point to test
  // is chosen by choosing a direction uniformly for the first n-1 components
  // and staying on the hyperplane r_1 + ... + r_n = res
  double* dir = GC_MALLOC( n * sizeof( double ) );
  // test: the next point to test
  double* test = GC_MALLOC( n * sizeof( double ) );
  // best_res would be used if we sometimes chose worse solutions
  // (e.g., in simulated annealing)
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
    if ( best < exp_eval( exp_res[ i ], res_all ) )
      best = exp_eval( exp_res[ i ], res_all );
  // starting hill climbing
  for ( int iters = 0; iters < hc_params->iters; ++iters ) {
    // pick a random direction on S^(n-2) = random direction for n-1 vectors
    for ( int i = 0; i < n / 2; ++i ) {
      x = ( double ) rand() / RAND_MAX;
      y = ( double ) rand() / RAND_MAX;
      box_muller( x, y, dir + 2 * i );
    }
    if ( n % 2 == 1 ) {
      x = ( double ) rand() / RAND_MAX;
      y = ( double ) rand() / RAND_MAX;
      box_muller( x, y, dir + n - 2 ); // overwrites the penultimate value,
                                       // but who cares?
    }
    dir[ n - 1 ] = 0;
    for ( int i = 0; i < n - 1; ++i ) {
      dir[ n - 1 ] -= dir[ i ];
    }
    for ( int i = 0; i < n; ++i )
      l2_norm += dir[ i ] * dir[ i ];
    l2_norm = sqrt( l2_norm );
    // pick a random radius
    r = pow( ( double ) rand() / RAND_MAX, 1 / ( n - 1 ) );
    r *= hc_params->step_size / l2_norm;
    // rescale the point if it goes outside the bounds (the total amount of
    // resource allocated to a component is below the initial amount)
    for ( int i = 0; i < n; ++i )
      if ( res_all[ i ] + dir[ i ] * r < sigma[ i ] )
        r = ( sigma[ i ] - res_all[ i ] ) / dir[ i ];
    // scale the direction and compute a new point
    for ( int i = 0; i < n; ++i ) {
      dir[ i ] *= r;
      test[ i ] = res_all[ i ] + dir[ i ];
    }
    // evaluate the new point and make it the new known best if it's better
    for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
      if ( exp_eval( exp_res[ i ], test ) > best ) {
        best = exp_eval( exp_res[ i ], test );
        for ( int j = 0; j < n; ++j )
          res_all[ j ] = test[ j ];
        break;
      }
  }
  return res_all;
}

double* resource_repartition_sa( int n, proof p, expression* cfd_res,
                                 double* sigma, double res,
                                 simulated_annealing_params sa_params ) {
  // warning: we use < on doubles (rather than dcompare) here for optimisation
  // to work better
  double* neighbour = GC_MALLOC( n * sizeof( double ) );
  double temp, energy, energy_new, energy_old, energy_best;

  // defining initial point
  double* point = ( * sa_params->init ) ( n, sigma, res );
  double* best = GC_MALLOC( n * sizeof( double ) );
  for ( int i = 0; i < n; ++i )
    best[ i ] = point[ i ];

  // expressions mapping resources to confidence
  expression* exp_res = GC_MALLOC( p->conclusion->pos_cfd_num *
                                   sizeof( expression ) );
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
    exp_res[ i ] = exp_array_composition( p->conclusion->ccl_pos_cfds[ i ],
                                          cfd_res );

  // computing energy of initial point
  energy_old = ( * sa_params->energy ) ( exp_eval( exp_res[ 0 ], point ) );
  for ( int i = 1; i < p->conclusion->pos_cfd_num; ++i ) {
    energy = ( * sa_params->energy ) ( exp_eval( exp_res[ i ], point ) );
    if ( energy_old < energy )
      energy_old = energy;
  }
  energy_best = energy_old;

  // main loop
  for ( int k = 0; k < sa_params->max_step; ++k ) {
    // compute temperature
    temp = ( * sa_params->temp ) ( k+1 );
    // pick a neighbour
    neighbour = ( * sa_params->neighbour ) ( n, point );
    energy_new =
      ( * sa_params->energy ) ( exp_eval( exp_res[ 0 ], neighbour ) );
    for ( int i = 1; i < p->conclusion->pos_cfd_num; ++i ) {
      energy = ( * sa_params->energy ) ( exp_eval( exp_res[ i ], point ) );
      if ( energy < energy_new )
        energy = energy_new;
    }
    // replace current point by neighbour probabilistically
    if ( ( * sa_params->prob ) ( energy_old, energy_new, temp ) >=
         ( double ) rand() / RAND_MAX ) {
      for ( int i = 0; i < n; ++i )
        point[ i ] = neighbour[ i ];
      energy_old = energy_new;
      if ( energy_old < energy_best ) {
        for ( int i = 0; i < n; ++i )
          best[ i ] = point[ i ];
        energy_best = energy_old;
      }
    }
  }

  for ( int i = 0; i < n; ++i )
    best[ i ] -= sigma[ i ];

  return best;
}

double* resource_repartition_sahc( int n, proof p, expression* cfd_res,
                                   double* sigma, double res,
                                   simulated_annealing_params sa_params,
                                   hill_climbing_params hc_params ) {
  // warning: we use < on doubles (rather than dcompare) here for hill climbing
  // to work better
  expression* exp_res = GC_MALLOC( p->conclusion->pos_cfd_num *
                                   sizeof( expression ) );
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
    exp_res[ i ] = exp_array_composition( p->conclusion->ccl_pos_cfds[ i ],
                                          cfd_res );
  double* res_all = resource_repartition_sa( n, p, cfd_res, sigma, res,
                                             sa_params );
  for ( int i = 0; i < n; ++i )
    res_all[ i ] += sigma[ i ];
  double x, y, l2_norm, r, best = 0;
  // dir: the delta between current point and next point to test
  // is chosen by choosing a direction uniformly for the first n-1 components
  // and staying on the hyperplane r_1 + ... + r_n = res
  double* dir = GC_MALLOC( n * sizeof( double ) );
  // test: the next point to test
  double* test = GC_MALLOC( n * sizeof( double ) );
  // best_res would be used if we sometimes chose worse solutions
  // (e.g., in simulated annealing)
  for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
    if ( best < exp_eval( exp_res[ i ], res_all ) )
      best = exp_eval( exp_res[ i ], res_all );
  // starting hill climbing
  for ( int iters = 0; iters < hc_params->iters; ++iters ) {
    // pick a random direction on S^(n-2) = random direction for n-1 vectors
    for ( int i = 0; i < n / 2; ++i ) {
      x = ( double ) rand() / RAND_MAX;
      y = ( double ) rand() / RAND_MAX;
      box_muller( x, y, dir + 2 * i );
    }
    if ( n % 2 == 1 ) {
      x = ( double ) rand() / RAND_MAX;
      y = ( double ) rand() / RAND_MAX;
      box_muller( x, y, dir + n - 2 ); // overwrites the penultimate value,
                                       // but who cares?
    }
    dir[ n - 1 ] = 0;
    for ( int i = 0; i < n - 1; ++i ) {
      dir[ n - 1 ] -= dir[ i ];
    }
    for ( int i = 0; i < n; ++i )
      l2_norm += dir[ i ] * dir[ i ];
    l2_norm = sqrt( l2_norm );
    // pick a random radius
    r = pow( ( double ) rand() / RAND_MAX, 1 / ( n - 1 ) );
    r *= hc_params->step_size / l2_norm;
    // rescale the point if it goes outside the bounds (the total amount of
    // resource allocated to a component is below the initial amount)
    for ( int i = 0; i < n; ++i )
      if ( res_all[ i ] + dir[ i ] * r < sigma[ i ] )
        r = ( sigma[ i ] - res_all[ i ] ) / dir[ i ];
    // scale the direction and compute a new point
    for ( int i = 0; i < n; ++i ) {
      dir[ i ] *= r;
      test[ i ] = res_all[ i ] + dir[ i ];
    }
    // evaluate the new point and make it the new known best if it's better
    for ( int i = 0; i < p->conclusion->pos_cfd_num; ++i )
      if ( exp_eval( exp_res[ i ], test ) > best ) {
        best = exp_eval( exp_res[ i ], test );
        for ( int j = 0; j < n; ++j )
          res_all[ j ] = test[ j ];
        break;
      }
  }
  for ( int i = 0; i < n; ++i )
    res_all[ i ] -= sigma[ i ];
  return res_all;
}

double* resource_repartition( int n, proof p, expression* cfd_res,
                              double* sigma, double res, optim_params params ) {
  switch ( params->opt_algo_type ) {
    case GRAD_ASC:
      return resource_repartition_ga( n, p, cfd_res, sigma, res,
                                      params->ga_params );
      break;
    case GRAD_ASC_HILL_CLIMB:
      return resource_repartition_gahc( n, p, cfd_res, sigma, res,
                                        params->ga_params, params->hc_params );
      break;
    case SIM_ANNEAL:
      return resource_repartition_sa( n, p, cfd_res, sigma, res,
                                      params->sa_params );
      break;
    case SIM_ANNEAL_HILL_CLIMB:
      return resource_repartition_sahc( n, p, cfd_res, sigma, res,
                                        params->sa_params, params->hc_params );
      break;
    default:
      fprintf( stderr,
               "resource_repartition: Unknown optimisation algorithm.\n" );
      exit( 1 );
      break;
  }
}
