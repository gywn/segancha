/**
 * @file example1.cpp
 * Very short example source code. The purpose of the example codes is to be
 * edited/extended.
 */

#include <cma-es/cmaes.h>
#include <iostream>
#include <lexi.h>
#include <stdlib.h>
#include <vector>

using Y = LexiProduct<double>;
// using Y = double;

/**
 * The objective (fitness) function to be minized. "cigtab" function.
 */
Y fitfun(const std::vector<double> &x, int N) {
  double sum = 1e4 * x[0] * x[0] + 1e-4 * x[1] * x[1];
  for (size_t i = 2; i < N; ++i)
    sum += x[i] * x[i];
  return std::vector<double>{0., sum};
}

/**
 * The optimization loop.
 */
int main(int, char **) {
  CMAES<double, Y> evo;
  Y *arFunvals;
  Individual<double, Y> *pop;
  double *xfinal;

  // Initialize everything
  const int dim = 22;
  double xstart[dim];
  for (int i = 0; i < dim; i++)
    xstart[i] = 0.5;
  double stddev[dim];
  for (int i = 0; i < dim; i++)
    stddev[i] = 0.3;
  Parameters<double, Y> parameters;
  parameters.stopTolFun = std::vector<double>{0, 1e-12};
  parameters.stopTolFunHist = std::vector<double>{0, 1e-13};
  // TODO Adjust parameters here
  parameters.init(dim, xstart, stddev);
  arFunvals = evo.init(parameters);

  std::cout << evo.sayHello() << std::endl;

  // Iterate until stop criterion holds
  while (!evo.testForTermination()) {
    // Generate lambda new search points, sample population
    pop = evo.samplePopulation(); // Do not change content of pop

    /* Here you may resample each solution point pop[i] until it
       becomes feasible, e.g. for box constraints (variable
       boundaries). function is_feasible(...) needs to be
       user-defined.
       Assumptions: the feasible domain is convex, the optimum is
       not on (or very close to) the domain boundary, initialX is
       feasible and initialStandardDeviations are sufficiently small
       to prevent quasi-infinite looping.
    */
    /* for (i = 0; i < evo.get(CMAES<double>::PopSize); ++i)
         while (!is_feasible(pop[i]))
           evo.reSampleSingle(i);
    */

    // evaluate the new search points using fitfun from above
    for (int i = 0; i < evo.get(CMAES<double, Y>::Lambda); ++i)
      arFunvals[i] =
          fitfun(pop[i].x, (int)evo.get(CMAES<double, Y>::Dimension));

    // update the search distribution used for sampleDistribution()
    double const *xmean = evo.updateDistribution(arFunvals);
    // for (size_t i = 0; i < (int)evo.get(CMAES<double, Y>::Dimension); ++i) {
    //   std::cout << xmean[i] << ",";
    // }
    // std::cout << std::endl;
  }
  std::cout << "Stop:" << std::endl << evo.getStopMessage();
  evo.writeToFile(CMAES<double, Y>::WKResume,
                  "resumeevo1.dat"); // write resumable state of CMA-ES

  // get best estimator for the optimum, xmean
  xfinal = evo.getNew(CMAES<double, Y>::XMean); // "XBestEver"
                                                // might be
                                                // used as
                                                // well

  // do something with final solution and finally release memory
  delete[] xfinal;

  return 0;
}
