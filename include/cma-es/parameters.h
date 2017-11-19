#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>

template <typename Scalar, typename Comparable> class CMAES;

/**
 * @class Parameters
 * Holds all parameters that can be adjusted by the user.
 */
template <typename Scalar, typename Comparable> class Parameters {
  friend class CMAES<Scalar, Comparable>;

public:
  /* Input parameters. */
  //! Problem dimension, must stay constant.
  int N;
  //! Initial search space vector.
  Scalar *xstart;
  //! A typical value for a search space vector.
  Scalar *typicalX;
  //! Indicates that the typical x is the initial point.
  bool typicalXcase;
  //! Initial standard deviations.
  Scalar *rgInitialStds;
  Scalar *rgDiffMinChange;

  /* Termination parameters. */
  //! Maximal number of objective function evaluations.
  Scalar stopMaxFunEvals;
  Scalar facmaxeval;
  //! Maximal number of iterations.
  Scalar stopMaxIter;
  //! Minimal fitness value. Only activated if flg is true.
  struct {
    bool flg;
    Scalar val;
  } stStopFitness;
  //! Minimal value difference.
  Comparable stopTolFun;
  //! Minimal history value difference.
  Comparable stopTolFunHist;
  //! Minimal search space step size.
  Scalar stopTolX;
  //! Defines the maximal condition number.
  Scalar stopTolUpXFactor;

  /* internal evolution strategy parameters */
  /**
   * Population size. Number of samples per iteration, at least two,
   * generally > 4.
   */
  int lambda;
  /**
   * Number of individuals used to recompute the mean.
   */
  int mu;
  Scalar mucov;
  /**
   * Variance effective selection mass, should be lambda/4.
   */
  Scalar mueff;
  /**
   * Weights used to recombinate the mean sum up to one.
   */
  Scalar *weights;
  /**
   * Damping parameter for step-size adaption, d = inifinity or 0 means adaption
   * is turned off, usually close to one.
   */
  Scalar damps;
  /**
   * cs^-1 (approx. n/3) is the backward time horizon for the evolution path
   * ps and larger than one.
   */
  Scalar cs;
  Scalar ccumcov;
  /**
   * ccov^-1 (approx. n/4) is the backward time horizon for the evolution path
   * pc and larger than one.
   */
  Scalar ccov;
  Scalar diagonalCov;
  struct {
    Scalar modulo;
    Scalar maxtime;
  } updateCmode;
  Scalar facupdateCmode;

  /**
   * Determines the method used to initialize the weights.
   */
  enum Weights {
    UNINITIALIZED_WEIGHTS,
    LINEAR_WEIGHTS,
    EQUAL_WEIGHTS,
    LOG_WEIGHTS
  } weightMode;

  //! File that contains an optimization state that should be resumed.
  std::string resumefile;

  //! Set to true to activate logging warnings.
  bool logWarnings;
  //! Output stream that is used to log warnings, usually std::cerr.
  std::ostream &logStream;

  Parameters()
      : N(-1), xstart(0), typicalX(0), typicalXcase(false), rgInitialStds(0),
        rgDiffMinChange(0), stopMaxFunEvals(-1), facmaxeval(1.0),
        stopMaxIter(-1.0), stopTolFun(1e-12), stopTolFunHist(1e-13),
        stopTolX(0), // 1e-11*insigma would also be reasonable
        stopTolUpXFactor(1e3), lambda(-1), mu(-1), mucov(-1), mueff(-1),
        weights(0), damps(-1), cs(-1), ccumcov(-1), ccov(-1), facupdateCmode(1),
        weightMode(UNINITIALIZED_WEIGHTS), resumefile(""), logWarnings(false),
        logStream(std::cerr) {
    stStopFitness.flg = false;
    stStopFitness.val = -std::numeric_limits<Scalar>::max();
    updateCmode.modulo = -1;
    updateCmode.maxtime = -1;
  }

  Parameters(const Parameters &parameters) { assign(parameters); }

  ~Parameters() {
    if (xstart)
      delete[] xstart;
    if (typicalX)
      delete[] typicalX;
    if (rgInitialStds)
      delete[] rgInitialStds;
    if (rgDiffMinChange)
      delete[] rgDiffMinChange;
    if (weights)
      delete[] weights;
  }

  Parameters &operator=(const Parameters &parameters) {
    assign(parameters);
    return *this;
  }

  /**
   * @param dimension Dimension of the search space \f$N\f$. No default
   *                  available, must be defined here or you have to set the
   *                  member manually.
   * @param inxstart Initial point in search space \f$x_0\f$, default (NULL) is
   *                 \f$(0.5,\ldots,0.5)^Scalar + N(0, initialStdDev^2) \in
   * R^N\f$. This must be an array of size \f$N\f$.
   * @param inrgsigma Coordinatewise initial standard deviation of the sample
   *                  distribution (\f$\sigma \cdot \sqrt{C_{ii}} =
   *                  initialStdDev[i]\f$). The expected initial distance
   *                  between initialX and the optimum per coordinate should be
   *                  roughly initialStdDev. The entries should not differ by
   *                  several orders of magnitude. Default (NULL) is
   *                  \f$(0.3,\ldots,0.3)^Scalar \in R^N\f$. This must be an
   * array of size \f$N\f$.
   */
  void init(int dimension = 0, const Scalar *inxstart = 0,
            const Scalar *inrgsigma = 0) {
    if (logWarnings) {
      if (!(xstart || inxstart || typicalX))
        logStream << "Warning: initialX undefined. typicalX = 0.5...0.5."
                  << std::endl;
      if (!(rgInitialStds || inrgsigma))
        logStream << "Warning: initialStandardDeviations undefined. 0.3...0.3."
                  << std::endl;
    }

    if (dimension <= 0 && N <= 0)
      throw std::runtime_error("Problem dimension N undefined.");
    else if (dimension > 0)
      N = dimension;

    if (weightMode == UNINITIALIZED_WEIGHTS)
      weightMode = LOG_WEIGHTS;

    diagonalCov = 0; // default is 0, but this might change in future

    if (!xstart) {
      xstart = new Scalar[N];
      if (inxstart) {
        for (int i = 0; i < N; ++i)
          xstart[i] = inxstart[i];
      } else if (typicalX) {
        typicalXcase = true;
        for (int i = 0; i < N; ++i)
          xstart[i] = typicalX[i];
      } else {
        typicalXcase = true;
        for (int i = 0; i < N; i++)
          xstart[i] = 0.5;
      }
    }

    if (!rgInitialStds) {
      rgInitialStds = new Scalar[N];
      if (inrgsigma) {
        for (int i = 0; i < N; ++i)
          rgInitialStds[i] = inrgsigma[i];
      } else {
        for (int i = 0; i < N; ++i)
          rgInitialStds[i] = Scalar(0.3);
      }
    }

    supplementDefaults();
  }

private:
  void assign(const Parameters &p) {
    N = p.N;

    if (xstart)
      delete[] xstart;
    if (p.xstart) {
      xstart = new Scalar[N];
      for (int i = 0; i < N; i++)
        xstart[i] = p.xstart[i];
    }

    if (typicalX)
      delete[] typicalX;
    if (p.typicalX) {
      typicalX = new Scalar[N];
      for (int i = 0; i < N; i++)
        typicalX[i] = p.typicalX[i];
    }

    typicalXcase = p.typicalXcase;

    if (rgInitialStds)
      delete[] rgInitialStds;
    if (p.rgInitialStds) {
      rgInitialStds = new Scalar[N];
      for (int i = 0; i < N; i++)
        rgInitialStds[i] = p.rgInitialStds[i];
    }

    if (rgDiffMinChange)
      delete[] rgDiffMinChange;
    if (p.rgDiffMinChange) {
      rgDiffMinChange = new Scalar[N];
      for (int i = 0; i < N; i++)
        rgDiffMinChange[i] = p.rgDiffMinChange[i];
    }

    stopMaxFunEvals = p.stopMaxFunEvals;
    facmaxeval = p.facmaxeval;
    stopMaxIter = p.stopMaxIter;

    stStopFitness.flg = p.stStopFitness.flg;
    stStopFitness.val = p.stStopFitness.val;

    stopTolFun = p.stopTolFun;
    stopTolFunHist = p.stopTolFunHist;
    stopTolX = p.stopTolX;
    stopTolUpXFactor = p.stopTolUpXFactor;

    lambda = p.lambda;
    mu = p.mu;
    mucov = p.mucov;
    mueff = p.mueff;

    if (weights)
      delete[] weights;
    if (p.weights) {
      weights = new Scalar[mu];
      for (int i = 0; i < mu; i++)
        weights[i] = p.weights[i];
    }

    damps = p.damps;
    cs = p.cs;
    ccumcov = p.ccumcov;
    ccov = p.ccov;
    diagonalCov = p.diagonalCov;

    updateCmode.modulo = p.updateCmode.modulo;
    updateCmode.maxtime = p.updateCmode.maxtime;

    facupdateCmode = p.facupdateCmode;

    weightMode = p.weightMode;

    resumefile = p.resumefile;
  }

  /**
   * Supplements default parameter values.
   */
  void supplementDefaults() {
    Scalar n = (Scalar)N;
    if (lambda < 2)
      lambda = 4 + (int)(3.0 * log(n));
    if (mu <= 0)
      mu = lambda / 2;
    if (!weights)
      setWeights(weightMode);

    if (cs > 0)
      cs *= (mueff + 2.) / (n + mueff + 3.);
    if (cs <= 0 || cs >= 1)
      cs = (mueff + 2.) / (n + mueff + 3.);

    if (ccumcov <= 0 || ccumcov > 1)
      ccumcov = 4. / (n + 4);

    if (mucov < 1)
      mucov = mueff;
    Scalar t1 = 2. / ((n + 1.4142) * (n + 1.4142));
    Scalar t2 = (2. * mueff - 1.) / ((n + 2.) * (n + 2.) + mueff);
    t2 = (t2 > 1) ? 1 : t2;
    t2 = (1. / mucov) * t1 + (1. - 1. / mucov) * t2;
    if (ccov >= 0)
      ccov *= t2;
    if (ccov < 0 || ccov > 1)
      ccov = t2;

    if (diagonalCov < 0)
      diagonalCov = 2 + 100. * n / sqrt((double)lambda);

    if (stopMaxFunEvals <= 0)
      stopMaxFunEvals = facmaxeval * 900 * (n + 3) * (n + 3);
    else
      stopMaxFunEvals *= facmaxeval;

    if (stopMaxIter <= 0)
      stopMaxIter = ceil((double)(stopMaxFunEvals / lambda));

    if (damps < Scalar(0))
      damps = Scalar(1);
    damps = damps *
                (Scalar(1) +
                 Scalar(2) * std::max(Scalar(0), std::sqrt((mueff - Scalar(1)) /
                                                           (n + Scalar(1))) -
                                                     Scalar(1))) *
                (Scalar)std::max(
                    Scalar(0.3),
                    Scalar(1) - // modify for short runs
                        n / (Scalar(1e-6) +
                             std::min(stopMaxIter, stopMaxFunEvals / lambda))) +
            cs;

    if (updateCmode.modulo < 0)
      updateCmode.modulo = 1. / ccov / n / 10.;
    updateCmode.modulo *= facupdateCmode;
    if (updateCmode.maxtime < 0)
      updateCmode.maxtime = 0.20; // maximal 20% of CPU-time
  }

  /**
   * Initializes the offspring weights.
   */
  void setWeights(Weights mode) {
    if (weights)
      delete[] weights;
    weights = new Scalar[mu];
    switch (mode) {
    case LINEAR_WEIGHTS:
      for (int i = 0; i < mu; ++i)
        weights[i] = mu - i;
      break;
    case EQUAL_WEIGHTS:
      for (int i = 0; i < mu; ++i)
        weights[i] = 1;
      break;
    case LOG_WEIGHTS:
    default:
      for (int i = 0; i < mu; ++i)
        weights[i] = log(mu + 1.) - log(i + 1.);
      break;
    }

    // normalize weights vector and set mueff
    Scalar s1 = 0, s2 = 0;
    for (int i = 0; i < mu; ++i) {
      s1 += weights[i];
      s2 += weights[i] * weights[i];
    }
    mueff = s1 * s1 / s2;
    for (int i = 0; i < mu; ++i)
      weights[i] /= s1;

    if (mu < 1 || mu > lambda ||
        (mu == lambda && weights[0] == weights[mu - 1]))
      throw std::runtime_error("setWeights(): invalid setting of mu or lambda");
  }
};
