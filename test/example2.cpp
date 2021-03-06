/**
 * @file example2.cpp
 * Implements additional restarts with increasing population
 * (Auger & Hansen 2005).
 */

#include <cma-es/cmaes.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double **OrthogonalBasis(int DIM);
double f_rosenbrock(const std::vector<double> &x);
double f_rand(const std::vector<double> &x);
double f_constant(const std::vector<double> &x);
double f_kugelmin1(const std::vector<double> &x);
double f_sphere(const std::vector<double> &x);
double f_stepsphere(const std::vector<double> &x);
double f_cigar(const std::vector<double> &x);
double f_cigtab(const std::vector<double> &x);
double f_tablet(const std::vector<double> &x);
double f_elli(const std::vector<double> &x);
double f_ellirot(const std::vector<double> &x);
double f_elli100(const std::vector<double> &x);
double f_ellinumtest(const std::vector<double> &x);
double f_parabR(const std::vector<double> &x);
double f_sharpR(const std::vector<double> &x);
double f_diffpow(const std::vector<double> &x);
double f_diffpowrot(const std::vector<double> &x);
double f_gleichsys5(const std::vector<double> &x);

std::vector<double> optimize(double (*pFun)(const std::vector<double> &),
                             int number_of_restarts,
                             double increment_factor_for_population_size);

int main(int, char **) {
  typedef double (*pfun_t)(const std::vector<double> &);
  size_t maxTest = 99;
  std::vector<pfun_t> rgpFun(
      maxTest); // vector (range) of pointer to objective function
  double incpopsize = 2;

  // Put together objective functions
  rgpFun[0] = f_sphere;
  rgpFun[1] = f_elli;
  rgpFun[2] = f_cigar;
  rgpFun[3] = f_cigtab;
  rgpFun[4] = f_tablet;
  rgpFun[5] = f_rosenbrock;
  rgpFun[6] = f_parabR;
  rgpFun[7] = f_sharpR;
  rgpFun[8] = f_diffpow;
  rgpFun[9] = f_kugelmin1;
  rgpFun[10] = f_ellinumtest;
  rgpFun[11] = f_elli100;
  rgpFun[18] = f_gleichsys5;
  rgpFun[19] = f_rand;
  rgpFun[20] = f_constant;
  rgpFun[21] = f_stepsphere;
  rgpFun[22] = f_ellirot;
  rgpFun[23] = f_diffpowrot;

  int nbrestarts = 0;
  for (size_t nb = 0; nb < maxTest; ++nb) {
    // Optimize function
    pfun_t f = rgpFun[nb];
    if (f == nullptr) {
      continue;
    }
    const auto x = optimize(f, nbrestarts, incpopsize);
  }

  return 0;
}

/**
 * Somewhat extended interface for optimizing pFun with CMAES implementing a
 * restart procedure with increasing population size.
 */
std::vector<double> optimize(double (*pFun)(const std::vector<double> &),
                             int nrestarts, double incpopsize) {
  CMAES<double> evo;               // the optimizer
  Individual<double, double> *pop; // sampled population
  double fbestever = 0;
  std::vector<double> xbestever; // store best solution
  double fmean;
  int irun,
      lambda = 0,     // offspring population size, 0 invokes default
      countevals = 0; // used to set for restarts

  for (irun = 0; irun < nrestarts + 1; ++irun) {
    /* Parameters can be set in two ways. Here as input parameter to evo.init()
       and as value read from signals.par by calling evo.readSignals
       explicitely.
     */
    const int dim = 22;
    double xstart[dim];
    for (int i = 0; i < dim; i++)
      xstart[i] = 0.5;
    double stddev[dim];
    for (int i = 0; i < dim; i++)
      stddev[i] = 0.3;
    Parameters<double, double> parameters;
    // You can resume a previous run by specifying a file that contains the
    // resume data:
    // parameters.resumefile = "resumeevo2.dat";
    parameters.logWarnings = true; // warnings will be printed on std::cerr
    parameters.stopTolX = 1e-11;
    parameters.updateCmode.maxtime = 1.0;
    parameters.lambda = lambda;
    parameters.init(dim, xstart, stddev);

    evo.init(parameters); // allocs fitvals
    std::cout << evo.sayHello() << std::endl;
    evo.countevals = countevals; // a hack, effects the output and termination

    std::vector<double> fitvals((size_t)parameters.lambda);
    while (!evo.testForTermination()) {
      // Generate population of new candidate solutions
      pop = evo.samplePopulation(); // do not change content of pop

      /* Here optionally handle constraints etc. on pop. You may
       * call evo.reSampleSingle(i) to resample the i-th
       * vector pop[i], see below.  Do not change pop in any other
       * way. You may also copy and modify (repair) pop[i] only
       * for the evaluation of the fitness function and consider
       * adding a penalty depending on the size of the
       * modification.
       */

      // Compute fitness value for each candidate solution
      for (size_t i = 0; i < evo.get(CMAES<double, double>::PopSize); ++i) {
        /* You may resample the solution i until it lies within the
           feasible domain here, e.g. until it satisfies given
           box constraints (variable boundaries). The function
           is_feasible() needs to be user-defined.
           Assumptions: the feasible domain is convex, the optimum
           is not on (or very close to) the domain boundary,
           initialX is feasible (or in case typicalX +-
           2*initialStandardDeviations is feasible) and
           initialStandardDeviations is (are) sufficiently small to prevent
           quasi-infinite looping.
          */
        /* while (!is_feasible(pop[i]))
             evo.reSampleSingle(i);
        */
        fitvals[i] = (*pFun)(pop[i].x);
      }

      // update search distribution
      evo.updateDistribution(fitvals);

      fflush(stdout);
    }

    lambda =
        (int)(incpopsize *
              evo.get(CMAES<double, double>::Lambda)); // needed for the restart
    countevals = (int)evo.get(CMAES<double, double>::Eval); // dito

    // print some "final" output
    std::cout << (int)evo.get(CMAES<double, double>::Generation)
              << " generations, " << (int)evo.get(CMAES<double, double>::Eval)
              << " fevals (" << evo.eigenTimings.totaltime
              << " sec): f(x)=" << evo.getValue(CMAES<double, double>::Fitness)
              << std::endl
              << "  (axis-ratio="
              << evo.get(CMAES<double, double>::MaxAxisLength) /
                     evo.get(CMAES<double, double>::MinAxisLength)
              << ", max/min-stddev="
              << evo.get(CMAES<double, double>::MaxStdDev) << "/"
              << evo.get(CMAES<double, double>::MinStdDev) << ")" << std::endl
              << "Stop (run " << (irun + 1) << "):" << std::endl
              << evo.getStopMessage();

    // write resume data
    evo.writeToFile(CMAES<double, double>::WKResume, "resumeevo2.dat");

    // keep best ever solution
    if (irun == 0 ||
        evo.getValue(CMAES<double, double>::FBestEver) < fbestever) {
      fbestever = evo.getValue(CMAES<double, double>::FBestEver);
      xbestever = evo.getVector(
          CMAES<double, double>::XBestEver); // allocates memory if needed
    }
    // best estimator for the optimum is xmean, therefore check
    const std::vector<double> xmean =
        evo.getVector(CMAES<double, double>::XMean);
    if ((fmean = (*pFun)(xmean)) < fbestever) {
      fbestever = fmean;
      xbestever = evo.getVector(CMAES<double, double>::XMean);
    }

    // abandon restarts if target fitness value was achieved or MaxFunEvals
    // reached
    std::string stop = evo.getStopMessage();
    if (strncmp(stop.c_str(), "Fitness", 7) == 0 ||
        strncmp(stop.c_str(), "MaxFunEvals", 11) == 0)
      break;
    if (strncmp(stop.c_str(), "Manual", 6) == 0) {
      printf("Press RETURN to start next run\n");
      fflush(stdout);
      getchar();
    }
  }

  return xbestever; // was dynamically allocated, should be deleted in the end
}

double f_rand(const std::vector<double> &) {
  double d = (double)rand() / RAND_MAX;
  while (d == 0.)
    d = (double)rand() / RAND_MAX;
  return d;
}

double f_constant(const std::vector<double> &) { return 1; }

static double SQR(double d) { return d * d; }

double f_stepsphere(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();
  for (i = 0; i < DIM; ++i)
    sum += floor(x[i] * x[i]);
  return sum;
}

double f_sphere(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();
  for (i = 0; i < DIM; ++i)
    sum += x[i] * x[i];
  return sum;
}

double f_cigar(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();

  for (i = 1; i < DIM; ++i)
    sum += x[i] * x[i];
  sum *= 1e6;
  sum += x[0] * x[0];
  return sum;
}

double f_cigtab(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();

  sum = x[0] * x[0] + 1e8 * x[DIM - 1] * x[DIM - 1];
  for (i = 1; i < DIM - 1; ++i)
    sum += 1e4 * x[i] * x[i];
  return sum;
}

double f_tablet(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();

  sum = 1e6 * x[0] * x[0];
  for (i = 1; i < DIM; ++i)
    sum += x[i] * x[i];
  return sum;
}

/* a hack, memory is never released */
double **OrthogonalBasis(size_t DIM) {
  static size_t b_dim;
  static double **b;
  double sp;
  int i, j, k;
  Random<double> R(2);

  if (b_dim != 0) { // Initialization was done
    if (b_dim != DIM) {
      printf("function OrthogonalBasis cannot change dimensionality in file "
             "example2.cpp");
      exit(0);
    }
    return b;
  }

  // Otherwise initialize basis b

  // allocate b
  b = (double **)calloc(DIM, sizeof(double *));
  if (!b) {
    printf("calloc failed in function OrthogonalBasis in file example2.cpp");
    exit(0);
  }
  for (i = 0; i < DIM; ++i) {
    b[i] = (double *)calloc(DIM, sizeof(double));
    if (!b[i]) {
      printf("calloc failed in function Orthogonalbasis in file example2.cpp");
      exit(0);
    }
  }
  b_dim = DIM;

  // generate orthogonal basis
  for (i = 0; i < DIM; ++i) {
    // sample components gaussian
    for (j = 0; j < DIM; ++j)
      b[i][j] = R.gauss();
    // substract projection of previous vectors
    for (j = i - 1; j >= 0; --j) {
      for (sp = 0., k = 0; k < DIM; ++k)
        sp += b[i][k] * b[j][k]; // scalar product
      for (k = 0; k < DIM; ++k)
        b[i][k] -= sp * b[j][k]; // substract
    }
    // normalize
    for (sp = 0., k = 0; k < DIM; ++k)
      sp += b[i][k] * b[i][k]; // squared norm
    for (k = 0; k < DIM; ++k)
      b[i][k] /= sqrt(sp);
  }

  return b;
}

double f_ellirot(const std::vector<double> &x) {
  size_t i, k;
  double sum = 0., y;
  size_t DIM = x.size();
  double **B = OrthogonalBasis(DIM);

  if (DIM == 1)
    return x[0] * x[0];
  for (i = 0; i < DIM; ++i) {
    for (y = 0., k = 0; k < DIM; ++k)
      y += B[i][k] * x[k];
    sum += exp(log(1e6) * 2. * (double)(i) / (DIM - 1)) * y * y;
  }
  return sum;
}

double f_elli(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();

  if (DIM == 1)
    return x[0] * x[0];
  for (i = 0; i < DIM; ++i)
    sum += exp(log(1000.) * 2. * (double)(i) / (DIM - 1)) * x[i] * x[i];
  return sum;
}

double f_elli100(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();

  if (DIM == 1)
    return x[0] * x[0];
  for (i = 0; i < DIM; ++i)
    sum += exp(log(100.) * 2. * (double)(i) / (DIM - 1)) * x[i] * x[i];
  return sum;
}

double f_diffpow(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();

  if (DIM == 1)
    return x[0] * x[0];
  for (i = 0; i < DIM; ++i)
    sum += pow(fabs(x[i]), 2. + 10 * (double)(i) / (DIM - 1));
  return sum;
}

double f_diffpowrot(const std::vector<double> &x) {
  size_t i, k;
  double sum = 0., y;
  size_t DIM = x.size();
  double **B = OrthogonalBasis(DIM);

  if (DIM == 1)
    return x[0] * x[0];
  for (i = 0; i < DIM; ++i) {
    for (y = 0., k = 0; k < DIM; ++k)
      y += B[i][k] * x[k];
    sum += pow(fabs(y), 2. + 10 * (double)(i) / (DIM - 1));
  }
  return sum;
}

double f_kugelmin1(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();

  for (i = 1; i < DIM; ++i)
    sum += x[i] * x[i];
  return sum;
}

/**
 * Rosenbrock's Function, generalized.
 */
double f_rosenbrock(const std::vector<double> &x) {
  double qualitaet;
  size_t i;
  size_t DIM = x.size();
  qualitaet = 0.0;

  for (i = DIM - 1; i > 0; --i)
    qualitaet += 100. * SQR(SQR(x[i - 1]) - x[i]) + SQR(1. - x[i - 1]);
  return (qualitaet);
}

double f_parabR(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();
  for (i = 1; i < DIM; ++i)
    sum += x[i] * x[i];
  return -x[0] + 100. * sum;
}

double f_sharpR(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();
  for (i = 1; i < DIM; ++i)
    sum += x[i] * x[i];
  return -x[0] + 100 * sqrt(sum);
}

double f_ellinumtest(const std::vector<double> &x) {
  size_t i;
  double sum = 0.;
  size_t DIM = x.size();
  static double maxVerhaeltnis = 0.;
  if (maxVerhaeltnis == 0.) {
    for (maxVerhaeltnis = 1.;
         maxVerhaeltnis < 1e99 && maxVerhaeltnis < 2. * maxVerhaeltnis;
         maxVerhaeltnis *= 2.)
      if (maxVerhaeltnis == maxVerhaeltnis + 1.)
        break;
    maxVerhaeltnis *= 10.;
    maxVerhaeltnis = sqrt(maxVerhaeltnis);
  }
  if (DIM < 3)
    return x[0] * x[0];
  for (i = 1; i < DIM; ++i)
    sum += exp(log(maxVerhaeltnis) * 2. * (double)(i - 1) / (DIM - 2)) * x[i] *
           x[i];
  return sum;
}

/**
 * 5-dimensional system of equations (Juergen Bremer).
 * For each row the following equation holds:
 *
 *  c_1*x[1] + c_2*x[2] + c_3*x[3] + c_4*x[4] + c_5*x[5] + c_0 = 0
 *
 * This is the reason why the square of the left side is in the quality
 * function.
 */
double f_gleichsys5(const std::vector<double> &x) {
  double qualitaet = 0.0;

  static double koeff[5][6] = {/* c_1,   c_2,  c_3,   c_4,  c_5,   c_0 */
                               {4, 191, 27, 199, 21, 172},
                               {191, 10883, 1413, 5402, 684, -8622},
                               {27, 1413, 191, 1032, 118, -94},
                               {199, 5402, 1032, 29203, 2331, 78172},
                               {21, 684, 118, 2331, 199, 5648}};
  size_t i, j;
  double sum;

  for (i = 0; i < 5; ++i) {
    sum = koeff[i][5];
    for (j = 0; j < 5; ++j) {
      sum += koeff[i][j] * x[j];
    }
    qualitaet += sum * sum;
  }
  return qualitaet;
}
