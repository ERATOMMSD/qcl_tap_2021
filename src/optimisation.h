/********************************************************************
 * optimisation.h
 *
 * Header of optimisation.c
 *
 * Author: Clovis Eberhart
 ********************************************************************/

#ifndef __CCL_OPTIMISATION_H__
#define __CCL_OPTIMISATION_H__

#include <string.h>
#include <cjson/cJSON.h>

/*********
 * Types *
 *********/

#define OPT_ALGO_CASES 1

/* Different possible cases for the optimisation algorithm:
 * - gradient ascent
 * - gradient ascent followed by hill climbing
 * - simulated annealing
 * - simulated annealing followed by hill climbing
 */
enum CASE_OPT_ALGO {
  GRAD_ASC,
  GRAD_ASC_HILL_CLIMB,
  SIM_ANNEAL,
  SIM_ANNEAL_HILL_CLIMB
  // more algorithms...?
};

/* The type structure of parameters for gradient ascent:
 * - [step_coef]: multiplicative coefficient of the size of a step of
 *   gradient ascent,
 * - [stop_error]: error under which we believe to have found the maximum,
 */
typedef struct gradient_ascent_params_s {
  double step_coef;
  // double stop_error;
} *gradient_ascent_params;

/* The type structure of parameters for hill climbing:
 * - [step_size]: size of a hill climbing step,
 * - [iters]: maximum number of hill climbing iterations.
 */
typedef struct hill_climbing_params_s {
  double step_size;
  int iters;
} *hill_climbing_params;

/* The type structure of parameters for simulated annealing:
 * - [max_step]: the total number of steps,
 * - [init]: generates the initial point, based on a given point and the amount
 *   of confidence to spend,
 * - [temp]: a function that returns a temperature based on the current step
 *   and total number of steps,
 * - [energy]: maps confidence value (to maximise) to energy (to minimise),
 * - [prob]: the probability to go from a point to another, based on their
 *   energies,
 * - [neighbour]: generates a neighbour around a given point.
 */
typedef struct simulated_annealing_params_s {
  int max_step;
  double* ( * init ) ( int n, double* point, double all );
  double ( * temp ) ( int k );
  double ( * energy ) ( double cfd );
  double ( * prob ) ( double energy_old, double energy_new, double temp );
  double* ( * neighbour ) ( int n, double* point );
  // could add:
  // - energy taking point and confidence functions as input
  // - resarting
  // - control of neighbourhood size
  // two possible strategies for initial point:
  // - random
  // - balanced: each coordinate gets 1/n of allowance (from current point)
  //   -> currently implemented
  // - completely balanced (each coord gets 1/n of total allowance) could
  //   violate constraints
} *simulated_annealing_params;

/* The type structure for optimisation algorithms:
 * - the type of the algorithm [opt_algo_type],
 * - all necessary parameters for the different algorithms.
 */
typedef struct optim_params_s {
  enum CASE_OPT_ALGO opt_algo_type;
  gradient_ascent_params ga_params;
  hill_climbing_params hc_params;
  simulated_annealing_params sa_params;
} *optim_params;

/************
 * Creation *
 ************/

/* sa_params_from_hyperparams_simple: creates simulated annealing parameters
 * from hyperparameters, according to the following strategy:
 * - [init] implements the "balanced" strategy,
 * - [temp] decreases linearly until it reaches zero, and stays at zero for some
 *   time,
 * - [energy] (to minimise) is the opposite of confidence (to maximise),
 * - [prob] is the usual function (as in Metropolis-Hastings),
 * - [neighbour] is a Gaussian around the current point,
 * with these observations:
 * - if the Gaussian has variance var^2, then the expected value of the distance
 *   from the initial point to the point reached after n steps is
 *   var sqrt( 2n / pi ), and the diameter of the slice of hyperplan we are
 *   looking at is sqrt( 2 ) res, so var should be larger than
 *   sqrt( pi / max_step ) res
 * - the maximal energy a point can have is - f( sigma ), and the minimum is -1,
 *   so if we want the probability of moving anywhere to be at least 1 - p_init
 *   at temp_max, then temp_max should be around ( 1 - f( sigma ) ) / p_init.
 * inputs:
 * - [max_step]: the number of steps
 * - [p_init]: a lower bound on probability of transitioning at the start of the
 *   algorithm,
 * - [k0]: the number of steps with temperature zero,
 * - [n]: the size of sigma,
 * - [sigma]: the starting point.
 * side effect: initialises constants used in
 * [sa_params_from_hyperparams_simple].
 */
void sa_params_from_hyperparams_simple_set( int max_step, double p_init, int k0,
                                            int n, double* sigma );
void sa_params_from_hyperparams_simple_set_with_lambda(
  int max_step, double p_init, int k0, int n, double* sigma, double lambda,
  double function_constant );

extern simulated_annealing_params sa_params_from_hyperparams_simple;

optim_params cJSON_to_optim_params( cJSON* json );
optim_params filename_to_optim_params( char* filename );

/**************
 * Algorithms *
 **************/

/* best_direction: computes the direction to increase confidence in a
 * proof most effectively.
 * inputs:
 * - a proof [p],
 * - the number [n] of variables the expression of [p] depends on
 *   (numbered from 0 to [n]-1),
 * - the point at which we want to compute the gradient (as an array
 *   [sigma] of doubles of size [n] representing confidences).
 * output: a vector of size [n] of doubles describing the direction.
 */
// double* best_direction( int n, proof p, double* sigma );

/* best_vector_res: computes the direction to increase confidence
 * in a proof most effectively when resources are taken into account.
 * inputs:
 * - a proof [p],
 * - the number [n] of variables the expression of [p] depends on
 *   (numbered from 0 to [n]-1),
 * - an array of expressions [cfd_res] describing the confidence
 *   reached by spending some amount of resources on the hypothesis,
 * - the point at which we want to compute the gradient (as an array
 *   [sigma] of doubles of size [n] representing resources).
 * output: a vector of size [n] of doubles describing the direction (in
 * resources).
 */
double* best_vector_res( int n, proof p, expression* cfd_res, double* sigma );

/* resource_repartition: gives the most effective resource repartition to
 * increase confidence in a proof.
 * inputs:
 * - a proof [p],
 * - the number [n] of variables the expression of [p] depends on
 *   (numbered from 0 to [n]-1),
 * - an array of expressions [cfd_res] describing the confidence reached by
 *   spending some amount of resources on the hypothesis,
 * - the point at which we want to compute the gradient (as an array
 *   [sigma] of doubles of size [n] representing resources),
 * - a double [res] representing the amount of resources to be spent,
 * - optimisation parameters [params].
 * output: a vector of size [n] of doubles describing the mount of resources to
 * be spent on each hypothesis.
 */
double* resource_repartition( int n, proof p, expression* cfd_res,
                              double* sigma, double res, optim_params params );

#endif // __CCL_OPTIMISATION_H__
