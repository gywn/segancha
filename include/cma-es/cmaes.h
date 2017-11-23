/**
 * @file cmaes.h
 * @author Nikolaus Hansen, ported to C++ by Alexander Fabisch
 *
 * \mainpage
 * CMA-ES for non-linear function minimization.
 *
 * Copyright of C implementation by Nikolaus Hansen (e-mail:
 * hansen .AT. bionik.tu-berlin.de, hansen .AT. lri.fr), ported to C++ by
 * <a href="mailto:afabisch@googlemail.com"> Alexander Fabisch</a>.
 *
 * \section lgpl License
 *
 * Copyright 1996, 2003, 2007, 2011 Nikolaus Hansen, Alexander Fabisch
 *
 * This file is part of CMA-ESpp.
 *
 * CMA-ESpp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CMA-ESpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CMA-ESpp. If not, see <http://www.gnu.org/licenses/>.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * \section purpose General Purpose
 *
 * The CMA-ES (Evolution Strategy with Covariance Matrix Adaptation) is a
 * robust search/optimization method. The goal is to minimize a given
 * objective function, \f$f: R^N \rightarrow R\f$. The CMA-ES should be
 * applied, if e.g. BFGS and/or conjugate gradient methods fail due to a
 * rugged search landscape (e.g. discontinuities, outliers, noise, local
 * optima, etc.). Learning the covariance matrix in the CMA-ES is similar
 * to learning the inverse Hessian matrix in a quasi-Newton method. On
 * smooth landscapes the CMA-ES is roughly ten times slower than BFGS,
 * assuming derivatives are not directly available. For up to \f$N=10\f$
 * parameters the simplex direct search method (Nelder & Mead) is
 * sometimes faster, but less robust than CMA-ES.  On considerably hard
 * problems the search (a single run) is expected to take between
 * \f$100\cdot N\f$ and \f$300\cdot N^2\f$ function evaluations. But you
 * might be lucky...
 *
 * \section application Application Remark
 *
 * The adaptation of the covariance matrix (e.g. by the CMA) is
 * equivalent to a general linear transformation of the problem
 * variables. Nevertheless, every problem specific knowledge about the
 * best problem transformation should be exploited before starting the
 * search procedure and an appropriate a priori transformation should be
 * applied to the problem. In particular a decision should be taken
 * whether variables, which are positive by nature, should be taken in
 * the log scale. A hard lower variable bound can also be realized by
 * taking the square. All variables should be re-scaled such that they
 * "live" in a similar search range width (for example, but not
 * necessarily between zero and one), such that the initial standard
 * deviation can be chosen the same for all variables.
 *
 *
 * \section links Links
 *  - http://www.lri.fr/~hansen/cmaesintro.html
 *  - http://www.lri.fr/~hansen/publications.html
 *
 * \section tut Tutorial
 * 	- http://www.lri.fr/~hansen/cmatutorial.pdf
 *
 * \section references References
 *
 * - Hansen, N, and S. Kern (2004).  Evaluating the CMA Evolution
 *   Strategy on Multimodal Test Functions. In: Eighth International
 *   Conference on Parallel Problem Solving from Nature PPSN VIII,
 *   Proceedings, pp. 282-291, Berlin: Springer
 *
 * - Hansen, N., S.D. Müller and P. Koumoutsakos (2003): Reducing the
 *   Time Complexity of the Derandomized Evolution Strategy with
 *   Covariance Matrix Adaptation (CMA-ES). Evolutionary Computation,
 *   11(1).
 *
 * - Hansen, N. and A. Ostermeier (2001). Completely Derandomized
 *   Self-Adaptation in Evolution Strategies. Evolutionary Computation,
 *   9(2), pp. 159-195.
 *
 * - Hansen, N. and A. Ostermeier (1996). Adapting arbitrary normal
 *   mutation distributions in evolution strategies: The covariance
 *   matrix adaptation. In Proceedings of the 1996 IEEE International
 *   Conference on Evolutionary Computation, pp. 312-317.
 */

#pragma once

#include "parameters.h"
#include "random.h"
#include "timings.h"
#include "utils.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/**
 * @class Individual
 *
 * @field x vector of parameters of native scalar type (float, double, etc.)
 * @field value linearly ordered object of the the object function value, which
 * supports all comparison operators, additive(+/-) group operations and
 * initialization through 0 or std::numeric_limit<Scalar>::max()
 */
template <typename Scalar, typename Ordered = Scalar> class Individual {
public:
  std::vector<Scalar> x;
  Ordered value;
};

/**
 * @class CMAES
 * Evolution Strategies with Covariance Matrix Adaptation. The public interface
 * of the optimization algorithm.
 */
template <typename Scalar, typename Ordered = Scalar> class CMAES {
public:
  /**
   * Keys for get().
   */
  enum GetScalar {
    NoScalar = 0,
    AxisRatio = 1,
    Eval = 2,
    Evaluations = 2,
    FctValue = 3,
    FuncValue = 3,
    FunValue = 3,
    Fitness = 3,
    FBestEver = 4,
    Generation = 5,
    Iteration = 5,
    MaxEval = 6,
    MaxFunEvals = 6,
    StopMaxFunEvals = 6,
    MaxGen = 7,
    MaxIter = 7,
    StopMaxIter = 7,
    MaxAxisLength = 8,
    MinAxisLength = 9,
    MaxStdDev = 10,
    MinStdDev = 11,
    Dim = 12,
    Dimension = 12,
    Lambda = 13,
    SampleSize = 13,
    PopSize = 13,
    Sigma = 14
  };

  /**
   * Keys for getVector().
   */
  enum GetVector {
    NoVector = 0,
    DiagC = 1,
    DiagD = 2,
    StdDev = 3,
    XBestEver = 4,
    XBest = 5,
    XMean = 6
  };

  /**
   * Keys for writeToFile().
   */
  enum WriteKey {
    WCNone = 0,
    WKResume = 1,
    WKXMean = 2,
    WKC = 4,
    WKAll = 8,
    WKFewInfo = 16,
    WKFew = 32,
    WKEval = 64,
    WKFitness = 128,
    WKFBestEver = 256,
    WKCGeneration = 512,
    WKSigma = 1024,
    WKLambda = 2048,
    WKB = 4096,
    WKXBest = 8192,
    WKClock = 16384,
    WKDim = 32768
  };

private:
  //! Implementation version.
  std::string version;
  //!< Random number generator.
  Random<Scalar> rand;
  //!< CMA-ES parameters.
  Parameters<Scalar, Ordered> params;

  //! Step size.
  Scalar sigma;
  //! Mean x vector, "parent".
  Scalar *xmean;
  //! Best sample ever.
  Individual<Scalar, Ordered> xBestEver;
  Scalar evalsBestEver;
  //! x-vectors, lambda offspring.
  Individual<Scalar, Ordered> *population;
  //! Sorting index of sample population.
  size_t *index;
  //! History of last historySize function values.
  Ordered *funcValueHistory;
  size_t historySize;

  Scalar chiN;
  //! Lower triangular matrix: i>=j for C[i][j].
  Scalar **C;
  //! Matrix with normalize eigenvectors in columns.
  Scalar **B;
  //! Axis lengths.
  Scalar *rgD;

  //! Anisotropic evolution path (for covariance).
  Scalar *pc;
  //! Isotropic evolution path (for step length).
  Scalar *ps;
  //! Last mean.
  Scalar *xold;
  //! Output vector.
  // Scalar *output;
  //! B*D*z.
  Scalar *BDz;
  //! Temporary (random) vector used in different places.
  Scalar *tempRandom;
  //! Objective function values of the population.
  Ordered *functionValues;
  //!< Public objective function value array returned by init().
  // Ordered *publicFitness;

  //! Generation number.
  size_t gen;
  //! Algorithm state.
  enum { INITIALIZED, SAMPLED, UPDATED } state;

  // repeatedly used for output
  Scalar maxdiagC;
  size_t maxdiagCi;
  Scalar mindiagC;
  size_t mindiagCi;
  Scalar maxEW;
  Scalar minEW;

  bool eigensysIsUptodate;
  bool doCheckEigen; //!< control via signals.par
  Scalar genOfEigensysUpdate;

  Scalar dMaxSignifKond;
  Scalar dLastMinEWgroesserNull;

  bool isResumeDone;

  time_t printtime;
  time_t writetime; //!< ideally should keep track for each output file
  time_t firstwritetime;
  time_t firstprinttime;

  //! stopCriteria[].first: criterion satisfied, .second: message
  std::vector<std::pair<bool, std::string>> stopCriteria;

  // std::string getTimeStr(void) {
  //   time_t tm = time(0);
  //   std::string timeStr(ctime(&tm));
  //   return timeStr.substr(0, 24);
  // }

  /**
   * Calculating eigenvalues and vectors.
   * @param rgtmp (input) N+1-dimensional vector for temporal use.
   * @param diag (output) N eigenvalues.
   * @param Q (output) Columns are normalized eigenvectors.
   */
  void eigen(Scalar *diag, Scalar **Q, Scalar *rgtmp) {
    assert(rgtmp && "eigen(): input parameter rgtmp must be non-NULL");

    if (C != Q) // copy C to Q
    {
      for (size_t i = 0; i < params.N; ++i)
        for (size_t j = 0; j <= i; ++j)
          Q[i][j] = Q[j][i] = C[i][j];
    }

    householder(Q, diag, rgtmp);
    ql(diag, rgtmp, Q);
  }

  /**
   * Exhaustive test of the output of the eigendecomposition, needs O(n^3)
   * operations writes to error file.
   * @return number of detected inaccuracies
   */
  int checkEigen(Scalar *diag, Scalar **Q) {
    // compute Q diag Q^Scalar and Q Q^Scalar to check
    int res = 0;
    for (int i = 0; i < params.N; ++i)
      for (int j = 0; j < params.N; ++j) {
        Scalar cc = 0., dd = 0.;
        for (int k = 0; k < params.N; ++k) {
          cc += diag[k] * Q[i][k] * Q[j][k];
          dd += Q[i][k] * Q[j][k];
        }
        // check here, is the normalization the right one?
        const bool cond1 = fabs(cc - C[i > j ? i : j][i > j ? j : i]) /
                               sqrt(C[i][i] * C[j][j]) >
                           Scalar(1e-10);
        const bool cond2 =
            fabs(cc - C[i > j ? i : j][i > j ? j : i]) > Scalar(3e-14);
        if (cond1 && cond2) {
          std::stringstream s;
          s << i << " " << j << ": " << cc << " "
            << C[i > j ? i : j][i > j ? j : i] << ", "
            << cc - C[i > j ? i : j][i > j ? j : i];
          if (params.logWarnings)
            params.logStream << "eigen(): imprecise result detected " << s.str()
                             << std::endl;
          ++res;
        }
        if (std::fabs(dd - (i == j)) > Scalar(1e-10)) {
          std::stringstream s;
          s << i << " " << j << " " << dd;
          if (params.logWarnings)
            params.logStream
                << "eigen(): imprecise result detected (Q not orthog.)"
                << s.str() << std::endl;
          ++res;
        }
      }
    return res;
  }

  /**
   * Symmetric tridiagonal QL algorithm, iterative.
   * Computes the eigensystem from a tridiagonal matrix in roughtly 3N^3
   * operations code adapted from Java JAMA package, function tql2.
   * @param d input: Diagonale of tridiagonal matrix. output: eigenvalues.
   * @param e input: [1..n-1], off-diagonal, output from Householder
   * @param V input: matrix output of Householder. output: basis of
   *          eigenvectors, according to d
   */
  void ql(Scalar *d, Scalar *e, Scalar **V) {
    const int n = params.N;
    Scalar f(0);
    Scalar tst1(0);
    const Scalar eps(2.22e-16); // 2.0^-52.0 = 2.22e-16

    // shift input e
    Scalar *ep1 = e;
    for (Scalar *ep2 = e + 1, *const end = e + n; ep2 != end; ep1++, ep2++)
      *ep1 = *ep2;
    *ep1 = Scalar(0); // never changed again

    for (int l = 0; l < n; l++) {
      // find small subdiagonal element
      Scalar &el = e[l];
      Scalar &dl = d[l];
      const Scalar smallSDElement = std::fabs(dl) + std::fabs(el);
      if (tst1 < smallSDElement)
        tst1 = smallSDElement;
      const Scalar epsTst1 = eps * tst1;
      int m = l - 1;
      while (m < n - 1) {
        ++m;
        if (std::fabs(e[m]) <= epsTst1)
          break;
      }

      // if m == l, d[l] is an eigenvalue, otherwise, iterate.
      if (m > l) {
        do {
          Scalar h, g = dl;
          Scalar &dl1r = d[l + 1];
          Scalar p = (dl1r - g) / (Scalar(2) * el);
          Scalar r = myhypot(p, Scalar(1));

          // compute implicit shift
          if (p < 0)
            r = -r;
          const Scalar pr = p + r;
          dl = el / pr;
          h = g - dl;
          const Scalar dl1 = el * pr;
          dl1r = dl1;
          for (int i = l + 2; i < n; i++)
            d[i] -= h;
          f += h;

          // implicit QL transformation.
          p = d[m];
          Scalar c(1);
          Scalar c2(1);
          Scalar c3(1);
          const Scalar el1 = e[l + 1];
          Scalar s(0);
          Scalar s2(0);
          for (int i = m - 1; i >= l; i--) {
            c3 = c2;
            c2 = c;
            s2 = s;
            const Scalar &ei = e[i];
            g = c * ei;
            h = c * p;
            r = myhypot(p, ei);
            e[i + 1] = s * r;
            s = ei / r;
            c = p / r;
            const Scalar &di = d[i];
            p = c * di - s * g;
            d[i + 1] = h + s * (c * g + s * di);

            // accumulate transformation.
            for (int k = 0; k < n; k++) {
              Scalar &Vki1 = V[k][i + 1];
              h = Vki1;
              Scalar &Vki = V[k][i];
              Vki1 = s * Vki + c * h;
              Vki *= c;
              Vki -= s * h;
            }
          }
          p = -s * s2 * c3 * el1 * el / dl1;
          el = s * p;
          dl = c * p;
        } while (std::fabs(el) > epsTst1);
      }
      dl += f;
      el = 0.0;
    }
  }

  /**
   * Householder transformation of a symmetric matrix V into tridiagonal form.
   * Code slightly adapted from the Java JAMA package, function private tred2().
   * @param V input: symmetric nxn-matrix. output: orthogonal transformation
   *          matrix: tridiag matrix == V* V_in* V^t.
   * @param d output: diagonal
   * @param e output: [0..n-1], off diagonal (elements 1..n-1)
   */
  void householder(Scalar **V, Scalar *d, Scalar *e) {
    const int n = params.N;

    for (int j = 0; j < n; j++) {
      d[j] = V[n - 1][j];
    }

    // Householder reduction to tridiagonal form

    for (int i = n - 1; i > 0; i--) {
      // scale to avoid under/overflow
      Scalar scale = 0.0;
      Scalar h = 0.0;
      for (Scalar *pd = d, *const dend = d + i; pd != dend; pd++) {
        scale += std::fabs(*pd);
      }
      if (scale == 0.0) {
        e[i] = d[i - 1];
        for (int j = 0; j < i; j++) {
          d[j] = V[i - 1][j];
          V[i][j] = 0.0;
          V[j][i] = 0.0;
        }
      } else {
        // generate Householder vector
        for (Scalar *pd = d, *const dend = d + i; pd != dend; pd++) {
          *pd /= scale;
          h += *pd * *pd;
        }
        Scalar &dim1 = d[i - 1];
        Scalar f = dim1;
        Scalar g = f > 0 ? -std::sqrt(h) : std::sqrt(h);
        e[i] = scale * g;
        h = h - f * g;
        dim1 = f - g;
        memset((void *)e, 0, (size_t)i * sizeof(Scalar));

        // apply similarity transformation to remaining columns
        for (int j = 0; j < i; j++) {
          f = d[j];
          V[j][i] = f;
          Scalar &ej = e[j];
          g = ej + V[j][j] * f;
          for (int k = j + 1; k <= i - 1; k++) {
            Scalar &Vkj = V[k][j];
            g += Vkj * d[k];
            e[k] += Vkj * f;
          }
          ej = g;
        }
        f = 0.0;
        for (int j = 0; j < i; j++) {
          Scalar &ej = e[j];
          ej /= h;
          f += ej * d[j];
        }
        Scalar hh = f / (h + h);
        for (int j = 0; j < i; j++) {
          e[j] -= hh * d[j];
        }
        for (int j = 0; j < i; j++) {
          Scalar &dj = d[j];
          f = dj;
          g = e[j];
          for (int k = j; k <= i - 1; k++) {
            V[k][j] -= f * e[k] + g * d[k];
          }
          dj = V[i - 1][j];
          V[i][j] = 0.0;
        }
      }
      d[i] = h;
    }

    // accumulate transformations
    const int nm1 = n - 1;
    for (int i = 0; i < nm1; i++) {
      Scalar h;
      Scalar &Vii = V[i][i];
      V[n - 1][i] = Vii;
      Vii = 1.0;
      h = d[i + 1];
      if (h != 0.0) {
        for (int k = 0; k <= i; k++) {
          d[k] = V[k][i + 1] / h;
        }
        for (int j = 0; j <= i; j++) {
          Scalar g = 0.0;
          for (int k = 0; k <= i; k++) {
            Scalar *Vk = V[k];
            g += Vk[i + 1] * Vk[j];
          }
          for (int k = 0; k <= i; k++) {
            V[k][j] -= g * d[k];
          }
        }
      }
      for (int k = 0; k <= i; k++) {
        V[k][i + 1] = 0.0;
      }
    }
    for (int j = 0; j < n; j++) {
      Scalar &Vnm1j = V[n - 1][j];
      d[j] = Vnm1j;
      Vnm1j = 0.0;
    }
    V[n - 1][n - 1] = 1.0;
    e[0] = 0.0;
  }

  /**
   * Dirty index sort.
   * @param col should support [] indexing
   */
  template <typename Collection>
  void sortIndex(const Collection &col, size_t *iindex, size_t n) {
    size_t i, j;
    for (i = 1, iindex[0] = 0; i < n; ++i) {
      for (j = i; j > 0; --j) {
        if (col[iindex[j - 1]] < col[i])
          break;
        iindex[j] = iindex[j - 1]; // shift up
      }
      iindex[j] = i;
    }
  }

  void adaptC2(const int hsig) {
    const int N = params.N;
    bool diag = params.diagonalCov == 1 || params.diagonalCov >= gen;

    if (params.ccov != Scalar(0)) {
      // definitions for speeding up inner-most loop
      const Scalar mucovinv = Scalar(1) / params.mucov;
      const Scalar commonFactor =
          params.ccov * (diag ? (N + Scalar(1.5)) / Scalar(3) : Scalar(1));
      const Scalar ccov1 = std::min(commonFactor * mucovinv, Scalar(1));
      const Scalar ccovmu =
          std::min(commonFactor * (Scalar(1) - mucovinv), Scalar(1) - ccov1);
      const Scalar sigmasquare = sigma * sigma;
      const Scalar onemccov1ccovmu = Scalar(1) - ccov1 - ccovmu;
      const Scalar longFactor =
          (Scalar(1) - hsig) * params.ccumcov * (Scalar(2) - params.ccumcov);

      eigensysIsUptodate = false;

      // update covariance matrix
      for (size_t i = 0; i < N; ++i)
        for (size_t j = diag ? i : 0; j <= i; ++j) {
          Scalar &Cij = C[i][j];
          Cij = onemccov1ccovmu * Cij +
                ccov1 * (pc[i] * pc[j] + longFactor * Cij);
          for (int k = 0; k < params.mu; ++k) { // additional rank mu update
            const std::vector<Scalar> &rgrgxindexk = population[index[k]].x;
            Cij += ccovmu * params.weights[k] * (rgrgxindexk[i] - xold[i]) *
                   (rgrgxindexk[j] - xold[j]) / sigmasquare;
          }
        }
      // update maximal and minimal diagonal value
      maxdiagC = mindiagC = C[0][0];
      for (size_t i = 1; i < N; ++i) {
        const Scalar &Cii = C[i][i];
        if (maxdiagC < Cii) {
          maxdiagC = Cii;
          maxdiagCi = i;
        } else if (mindiagC > Cii) {
          mindiagC = Cii;
          mindiagCi = i;
        }
      }
    }
  }

  /**
   * Treats minimal standard deviations and numeric problems. Increases sigma.
   */
  void testMinStdDevs(void) {
    if (!this->params.rgDiffMinChange)
      return;

    for (int i = 0; i < params.N; ++i)
      while (this->sigma * std::sqrt(this->C[i][i]) <
             this->params.rgDiffMinChange[i])
        this->sigma *=
            std::exp(Scalar(0.05) + this->params.cs / this->params.damps);
  }

  /**
   * Adds the mutation sigma*B*(D*z).
   * @param x Search space vector.
   * @param eps Mutation factor.
   */
  void addMutation(std::vector<Scalar> &x, Scalar eps = 1.0) {
    for (int i = 0; i < params.N; ++i)
      tempRandom[i] = rgD[i] * rand.gauss();
    for (int i = 0; i < params.N; ++i) {
      Scalar sum = 0.0;
      for (int j = 0; j < params.N; ++j)
        sum += B[i][j] * tempRandom[j];
      x[i] = xmean[i] + eps * sigma * sum;
    }
  }

  /**
   * This hack reads key words from input key for data to be written to
   * a file, see file signals.par as input file. The length of the keys
   * is mostly fixed. If the key phrase does not match the expectation the
   * output might be strange.
   */
  void writeToStream(int key, std::ostream &file) {
    if (key & WKResume) {
      file << std::endl << "# resume " << params.N << std::endl;
      file << "xmean" << std::endl;
      writeToStream(WKXMean, file);
      file << "path for sigma" << std::endl;
      for (int i = 0; i < params.N; ++i)
        file << ps[i] << (i == params.N - 1 ? "\n" : "\t");
      file << "path for C" << std::endl;
      for (int i = 0; i < params.N; ++i)
        file << pc[i] << (i == params.N - 1 ? "\n" : "\t");
      file << "sigma " << sigma << std::endl;
      // note than B and D might not be up-to-date
      file << "covariance matrix" << std::endl;
      writeToStream(WKC, file);
    }
    if (key & WKXMean) {
      for (int i = 0; i < params.N; ++i)
        file << (i == 0 ? "" : "\t") << xmean[i];
      file << std::endl;
    }
    if (key & WKC) {
      for (int i = 0; i < params.N; ++i)
        for (int j = 0; j <= i; ++j) {
          file << C[i][j];
          if (j == i)
            file << std::endl;
          else
            file << '\t';
        }
      file << std::endl;
    }
    if (key & WKAll) {
      time_t ti = time(0);
      file << std::endl
           << "# --------- " << asctime(localtime(&ti)) << std::endl;
      file << " N " << params.N << std::endl;
      file << "function evaluations " << (long)countevals << std::endl;
      file << "elapsed (CPU) time [s] " << std::setprecision(2)
           << eigenTimings.totaltotaltime << std::endl;
      file << "function value f(x)=" << population[index[0]].value << std::endl;
      file << "maximal standard deviation " << sigma * std::sqrt(maxdiagC)
           << std::endl;
      file << "minimal standard deviation " << sigma * std::sqrt(mindiagC)
           << std::endl;
      file << "sigma " << sigma << std::endl;
      file << "axisratio "
           << (maxElement(rgD, (size_t)params.N) /
               minElement(rgD, (size_t)params.N))
           << std::endl;
      file << "xbestever found after " << std::setprecision(0) << evalsBestEver
           << "evaluations, function value " << xBestEver.value << std::endl;
      for (size_t i = 0; i < params.N; ++i)
        file << " " << std::setw(12) << xBestEver.x[i]
             << (i % 5 == 4 || i == params.N - 1 ? '\n' : ' ');
      file << "xbest (of last generation, function value "
           << population[index[0]].value << ")" << std::endl;
      for (size_t i = 0; i < params.N; ++i)
        file << " " << std::setw(12) << population[index[0]].x[i]
             << (i % 5 == 4 || i == params.N - 1 ? '\n' : ' ');
      file << "xmean" << std::endl;
      for (size_t i = 0; i < params.N; ++i)
        file << " " << std::setw(12) << xmean[i]
             << (i % 5 == 4 || i == params.N - 1 ? '\n' : ' ');
      file << "Standard deviation of coordinate axes (sigma*sqrt(diag(C)))"
           << std::endl;
      for (size_t i = 0; i < params.N; ++i)
        file << " " << std::setw(12) << sigma * std::sqrt(C[i][i])
             << (i % 5 == 4 || i == params.N - 1 ? '\n' : ' ');
      file << "Main axis lengths of mutation ellipsoid (sigma*diag(D))"
           << std::endl;
      for (size_t i = 0; i < params.N; ++i)
        tempRandom[i] = rgD[i];
      std::sort(tempRandom, tempRandom + params.N);
      for (size_t i = 0; i < params.N; ++i)
        file << " " << std::setw(12)
             << sigma * tempRandom[(size_t)params.N - 1 - i]
             << (i % 5 == 4 || i == params.N - 1 ? '\n' : ' ');
      file << "Longest axis (b_i where d_ii=max(diag(D))" << std::endl;
      size_t k = maxIndex(rgD, (size_t)params.N);
      for (size_t i = 0; i < params.N; ++i)
        file << " " << std::setw(12) << B[i][k]
             << (i % 5 == 4 || i == params.N - 1 ? '\n' : ' ');
      file << "Shortest axis (b_i where d_ii=max(diag(D))" << std::endl;
      k = minIndex(rgD, (size_t)params.N);
      for (size_t i = 0; i < params.N; ++i)
        file << " " << std::setw(12) << B[i][k]
             << (i % 5 == 4 || i == params.N - 1 ? '\n' : ' ');
      file << std::endl;
    }
    if (key & WKFewInfo) {
      file << " Iter\tFevals\tFunction Value\tSigma\tMaxCoorDev\tMinCoorDev\t"
           << "AxisRatio\tMinDii\tTime in eig" << std::endl;
      file << std::endl;
    }
    if (key & WKFew) {
      file << (int)gen << "\t" << (int)countevals << "\t"
           << functionValues[index[0]] << "\t\t" << sigma << "  "
           << sigma * std::sqrt(maxdiagC) << "\t" << sigma * std::sqrt(mindiagC)
           << "\t" << std::scientific << std::setprecision(2)
           << std::sqrt(maxEW / minEW) << "\t" << std::sqrt(minEW) << "  "
           << eigenTimings.totaltotaltime;
      file << std::endl;
    }
    if (key & WKEval) {
      file << countevals;
      file << std::endl;
    }
    if (key & WKFitness) {
      for (int i = 0; i < params.N; ++i)
        file << (i == 0 ? "" : "\t") << functionValues[index[i]];
      file << std::endl;
    }
    if (key & WKFBestEver) {
      file << xBestEver.value << std::endl;
    }
    if (key & WKCGeneration) {
      file << gen << std::endl;
    }
    if (key & WKSigma) {
      file << sigma << std::endl;
    }
    if (key & WKLambda) {
      file << params.lambda << std::endl;
    }
    if (key & WKB) {
      size_t *iindex = new size_t[params.N];
      sortIndex(rgD, iindex, (size_t)params.N);
      for (int i = 0; i < params.N; ++i)
        for (int j = 0; j < params.N; ++j) {
          file << B[j][iindex[params.N - 1 - i]];
          if (j != params.N - 1)
            file << '\t';
          else
            file << std::endl;
        }
      delete[] iindex;
      iindex = 0;
      file << std::endl;
    }
    if (key & WKXBest) {
      for (size_t i = 0; i < params.N; ++i)
        file << (i == 0 ? "" : "\t") << population[index[0]].x[i];
      file << std::endl;
    }
    if (key & WKClock) {
      eigenTimings.update();
      file << eigenTimings.totaltotaltime << " " << eigenTimings.tictoctime
           << std::endl;
    }
    if (key & WKDim) {
      file << params.N;
      file << std::endl;
    }
  }

public:
  Scalar countevals; //!< objective function evaluations
  Timing eigenTimings;

  CMAES() : version("1.1alpha") {}

  /**
   * Releases the dynamically allocated memory, including that of the return
   * value of init().
   */
  ~CMAES() {
    delete[] pc;
    delete[] ps;
    delete[] tempRandom;
    delete[] BDz;
    delete[] xmean;
    delete[] xold;
    // delete[]-- xBestEver;
    // delete[]-- output;
    delete[] rgD;
    for (int i = 0; i < params.N; ++i) {
      delete[] C[i];
      delete[] B[i];
    }
    // for (int i = 0; i < params.lambda; ++i)
    //   delete[]-- population[i];
    delete[] C;
    delete[] B;
    delete[] index;
    // delete[] publicFitness;
    delete[] population;
    delete[] functionValues;
    delete[] funcValueHistory;
  }

  /**
   * Allocate ressources and initializes the CMA-ES algorithm.
   * @param parameters The CMA-ES parameters.
   */
  void init(const Parameters<Scalar, Ordered> &parameters) {
    params = parameters;

    stopCriteria.resize(10); // There are ten stop criteria
    // stopMessage = "";

    Scalar trace(0);
    for (int i = 0; i < params.N; ++i)
      trace += params.rgInitialStds[i] * params.rgInitialStds[i];
    sigma = std::sqrt(trace / params.N);

    chiN = std::sqrt((Scalar)params.N) *
           (Scalar(1) - Scalar(1) / (Scalar(4) * params.N) +
            Scalar(1) / (Scalar(21) * params.N * params.N));
    eigensysIsUptodate = true;
    doCheckEigen = false;
    genOfEigensysUpdate = 0;
    isResumeDone = false;

    Scalar dtest;
    for (dtest = Scalar(1); dtest != 0 && dtest < Scalar(1.1) * dtest;
         dtest *= Scalar(2))
      if (dtest == dtest + Scalar(1))
        break;
    dMaxSignifKond =
        dtest / Scalar(1e2); // not sure whether this is really save,
                             // 100 does not work well enough

    gen = 0;
    countevals = 0;
    state = INITIALIZED;
    dLastMinEWgroesserNull = Scalar(1);
    printtime = writetime = firstwritetime = firstprinttime = 0;

    pc = new Scalar[params.N];
    ps = new Scalar[params.N];
    tempRandom = new Scalar[params.N + 1];
    BDz = new Scalar[params.N];
    xmean = new Scalar[params.N];
    // xmean[0] = params.N;
    // ++xmean;
    xold = new Scalar[params.N];
    // xold[0] = params.N;
    // ++xold;
    xBestEver.x.resize((size_t)params.N);
    // xBestEver = new Scalar[params.N + 3];
    // xBestEver[0] = params.N;
    // ++xBestEver;
    // xBestEver[params.N] = std::numeric_limits<Scalar>::max();
    xBestEver.value = std::numeric_limits<Scalar>::max();
    // output = new Scalar[params.N + 2];
    // output[0] = params.N;
    // ++output;
    rgD = new Scalar[params.N];
    C = new Scalar *[params.N];
    B = new Scalar *[params.N];
    // publicFitness = new Ordered[params.lambda];
    functionValues = new Ordered[params.lambda];
    // functionValues[0] = params.lambda;
    // ++functionValues;
    historySize =
        10 + (size_t)ceil(3. * 10. * (double)params.N / params.lambda);
    funcValueHistory = new Ordered[historySize];
    // funcValueHistory[0] = (Scalar)historySize;
    // funcValueHistory++;

    for (int i = 0; i < params.N; ++i) {
      C[i] = new Scalar[i + 1];
      B[i] = new Scalar[params.N];
    }
    index = new size_t[params.lambda];
    for (size_t i = 0; i < params.lambda; ++i)
      index[i] = i;
    population = new Individual<Scalar, Ordered>[params.lambda];
    for (int i = 0; i < params.lambda; ++i) {
      population[i].x.resize((size_t)params.N);
      // population[i] = new Scalar[params.N + 2];
      // population[i][0] = params.N;
      // population[i]++;
      // for (int j = 0; j < params.N; j++)
      //   population[i][j] = 0.0;
      population[i].value = std::numeric_limits<Scalar>::max();
    }

    // initialize newed space
    for (int i = 0; i < params.lambda; i++) {
      functionValues[i] = std::numeric_limits<Scalar>::max();
    }
    for (int i = 0; i < historySize; i++) {
      funcValueHistory[i] = std::numeric_limits<Scalar>::max();
    }
    for (int i = 0; i < params.N; ++i)
      for (int j = 0; j < i; ++j)
        C[i][j] = B[i][j] = B[j][i] = 0.;

    for (int i = 0; i < params.N; ++i) {
      B[i][i] = Scalar(1);
      C[i][i] = rgD[i] =
          params.rgInitialStds[i] * std::sqrt((double)params.N / trace);
      C[i][i] *= C[i][i];
      pc[i] = ps[i] = Scalar(0);
    }
    minEW = minElement(rgD, (size_t)params.N);
    minEW = minEW * minEW;
    maxEW = maxElement(rgD, (size_t)params.N);
    maxEW = maxEW * maxEW;

    maxdiagC = C[0][0];
    for (int i = 1; i < params.N; ++i)
      if (maxdiagC < C[i][i])
        maxdiagC = C[i][i];
    mindiagC = C[0][0];
    for (int i = 1; i < params.N; ++i)
      if (mindiagC > C[i][i])
        mindiagC = C[i][i];

    for (int i = 0; i < params.N; ++i)
      xmean[i] = xold[i] = params.xstart[i];
    // use in case xstart as typicalX
    if (params.typicalXcase)
      for (int i = 0; i < params.N; ++i)
        xmean[i] += sigma * rgD[i] * rand.gauss();

    if (params.resumefile != "")
      resumeDistribution(params.resumefile);

    // return publicFitness;
  }

  /**
   * Well, says hello.
   * @return eg. "CMA-ES-1.1alpha lambda=263, mu=131, mu_eff=68.6358,
   * dimension=14, diagonalIterations=0"
   */
  std::string sayHello() {
    std::stringstream stream;
    stream << "CMA-ES-" << version << " lambda=" << params.lambda
           << ", mu=" << params.mu << ", mu_eff=" << params.mueff
           << ", dimension=" << params.N
           << ", diagonalIterations=" << (long)params.diagonalCov;
    return stream.str();
  }

  /**
   * Allows to restart with saved internal state (distribution) variables (use
   * writeToFile() for saving). Keyword "resume" followed by a filename in
   * initials.par invokes this function during initialization. Searches in
   * filename for the last occurrence of word "resume", followed by a dimension
   * number, and reads the subsequent values for xmean, evolution paths ps and
   * pc, sigma and covariance matrix.  Note that init() needs to be called
   * before calling resume_distribution() explicitely.  In the former all the
   * remaining (strategy-)parameters are set. It can be useful to edit the
   * written parameters, in particular to increase sigma, before resume.
   *
   * Not all internal state parameters are recovered. In particular generation
   * number and xbestever are not restored. For covariance matrices with large
   * condition numbers the writing precision of 6 digits is not sufficient and
   * resume will lead to poor result.
   * @param filename A file, that was written presumably by writeToFile().
   */
  void resumeDistribution(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open())
      throw std::runtime_error("resumeDistribution(): could not open '" +
                               filename + "'");

    std::streampos lastResume = 0;
    std::string entry = "";
    while (!file.eof()) {
      file >> entry;
      if (entry == "resume") {
        lastResume = file.tellg();
        break;
      }
    }
    file.clear();
    file.seekg(lastResume);

    int n = 0;
    file >> n;
    if (n != params.N)
      throw std::runtime_error(
          "resumeDistribution(): Dimension numbers do not match");

    // find next "xmean" entry
    while (!file.eof()) {
      file >> entry;
      if (entry == "xmean")
        break;
    }
    // read xmean
    if (file.eof())
      throw std::runtime_error("resumeDistribution(): 'xmean' not found");
    for (int i = 0; i < n; i++)
      file >> xmean[i];
    file.clear();
    file.seekg(lastResume);

    // find next "path for sigma" entry
    while (!file.eof()) {
      file >> entry;
      if (entry == "path") {
        std::string temp = "";
        file >> temp;
        entry += " " + temp;
        file >> temp;
        entry += " " + temp;
        if (entry == "path for sigma")
          break;
      }
    }
    // read ps
    if (file.eof())
      throw std::runtime_error(
          "resumeDistribution(): 'path for sigma' not found");
    for (int i = 0; i < n; i++)
      file >> ps[i];
    file.clear();
    file.seekg(lastResume);

    // find next "path for C" entry
    while (!file.eof()) {
      file >> entry;
      if (entry == "path") {
        std::string temp = "";
        file >> temp;
        entry += " " + temp;
        file >> temp;
        entry += " " + temp;
        if (entry == "path for C")
          break;
      }
    }
    // read pc
    if (file.eof())
      throw std::runtime_error("resumeDistribution(): 'path for C' not found");
    for (int i = 0; i < n; i++)
      file >> pc[i];
    file.clear();
    file.seekg(lastResume);

    // find next "sigma" entry
    while (!file.eof()) {
      file >> entry;
      if (entry == "sigma")
        break;
    }
    // read pc
    if (file.eof())
      throw std::runtime_error("resumeDistribution(): 'sigma' not found");
    file >> sigma;
    file.clear();
    file.seekg(lastResume);

    // find next "covariance matrix" entry
    while (!file.eof()) {
      file >> entry;
      if (entry == "covariance") {
        std::string temp = "";
        file >> temp;
        entry += " " + temp;
        if (entry == "covariance matrix")
          break;
      }
    }
    // read C
    if (file.eof())
      throw std::runtime_error(
          "resumeDistribution(): 'covariance matrix' not found");
    for (int i = 0; i < params.N; ++i)
      for (int j = 0; j <= i; ++j)
        file >> C[i][j];

    eigensysIsUptodate = false;
    isResumeDone = true;
    updateEigensystem(true);
  }

  /**
   * @return A pointer to a "population" of lambda N-dimensional multivariate
   * normally distributed samples.
   */
  Individual<Scalar, Ordered> *samplePopulation() {
    bool diag = params.diagonalCov == 1 || params.diagonalCov >= gen;

    // calculate eigensystem
    if (!eigensysIsUptodate) {
      if (!diag)
        updateEigensystem(false);
      else {
        for (int i = 0; i < params.N; ++i)
          rgD[i] = std::sqrt(C[i][i]);
        minEW = square(minElement(rgD, (size_t)params.N));
        maxEW = square(maxElement(rgD, (size_t)params.N));
        eigensysIsUptodate = true;
        eigenTimings.start();
      }
    }

    testMinStdDevs();

    for (int iNk = 0; iNk < params.lambda;
         ++iNk) { // generate scaled random vector D*z
      std::vector<Scalar> &rgrgxink = population[iNk].x;
      for (size_t i = 0; i < params.N; ++i)
        if (diag)
          rgrgxink[i] = xmean[i] + sigma * rgD[i] * rand.gauss();
        else
          tempRandom[i] = rgD[i] * rand.gauss();
      if (!diag)
        for (size_t i = 0; i < params.N; ++i) // add mutation sigma*B*(D*z)
        {
          Scalar sum = 0.0;
          for (int j = 0; j < params.N; ++j)
            sum += B[i][j] * tempRandom[j];
          rgrgxink[i] = xmean[i] + sigma * sum;
        }
    }

    if (state == UPDATED || gen == 0)
      ++gen;
    state = SAMPLED;

    return population;
  }

  /**
   * Can be called after samplePopulation() to resample single solutions of the
   * population as often as desired. Useful to implement a box constraints
   * (boundary) handling.
   * @param i Index to an element of the returned value of samplePopulation().
   *          population[index] will be resampled where \f$0\leq i<\lambda\f$
   *          must hold.
   * @return A pointer to the resampled "population".
   */
  Individual<Scalar, Ordered> *reSampleSingle(int i) {
    assert(i >= 0 && i < params.lambda &&
           "reSampleSingle(): index must be between 0 and sp.lambda");
    std::vector<Scalar> &x = population[i].x;
    addMutation(x);
    return population;
  }

  /**
   * Can be called after samplePopulation() to resample single solutions. In
   * general, the function can be used to sample as many independent
   * mean+sigma*Normal(0,C) distributed vectors as desired.
   *
   * Input x can be a pointer to an element of the vector returned by
   * samplePopulation() but this is inconsistent with the const qualifier of the
   * returned value and therefore rather reSampleSingle() should be used.
   * @param x Solution vector that gets sampled a new value. If x == NULL new
   *          memory is allocated and must be released by the user using
   *          delete[].
   * @return A pointer to the resampled solution vector, equals input x for
   *         x != NULL on input.
   */
  std::vector<Scalar> &sampleSingleInto(std::vector<Scalar> &x) {
    // if (!x)
    //   x = new Scalar[params.N];
    addMutation(x);
    return x;
  }

  /**
   * Can be called after samplePopulation() to resample single solutions. In
   * general, the function can be used to sample as many independent
   * mean+sigma*Normal(0,C) distributed vectors as desired.
   * @param x Element of the return value of samplePopulation(), that is
   *          pop[0..\f$\lambda\f$]. This solution vector of the population gets
   *          sampled a new value.
   * @return A pointer to the resampled "population" member.
   */
  Scalar const *reSampleSingleOld(Scalar *x) {
    assert(x && "reSampleSingleOld(): Missing input x");
    addMutation(x);
    return x;
  }

  /**
   * Used to reevaluate a slightly disturbed solution for an uncertaintly
   * measurement. In case if x == NULL on input, the memory of the returned x
   * must be released.
   * @param x Solution vector that gets sampled a new value. If x == NULL new
   *          memory is allocated and must be released by the user using
   *          delete[] x.
   * @param pxmean Mean vector \f$\mu\f$ for perturbation.
   * @param eps Scale factor \f$\epsilon\f$ for perturbation:
   *            \f$x \sim \mu + \epsilon \sigma N(0,C)\f$.
   * @return A pointer to the perturbed solution vector, equals input x for
   *         x != NULL.
   */
  Scalar *perturbSolutionInto(Scalar *x, Scalar const *pxmean, Scalar eps) {
    if (!x)
      x = new Scalar[params.N];
    assert(pxmean && "perturbSolutionInto(): pxmean was not given");
    addMutation(x, eps);
    return x;
  }

  /**
   * Core procedure of the CMA-ES algorithm. Sets a new mean value and estimates
   * the new covariance matrix and a new step size for the normal search
   * distribution.
   * @param fitnessValues An iterable of \f$\lambda\f$ function values.
   * @return Mean value of the new distribution.
   */
  template <typename OrderedCollection>
  Scalar *updateDistribution(const OrderedCollection &fitnessValues) {
    const int N = params.N;
    bool diag = params.diagonalCov == 1 || params.diagonalCov >= gen;

    assert(state != UPDATED &&
           "updateDistribution(): You need to call "
           "samplePopulation() before update can take place.");
    // assert(fitnessValues &&
    //        "updateDistribution(): No fitness function value array input.");

    if (state == SAMPLED) // function values are delivered here
      countevals += params.lambda;
    else if (params.logWarnings)
      params.logStream << "updateDistribution(): unexpected state" << std::endl;

    // assign function values
    for (size_t i = 0; i < params.lambda; ++i) {
      // std::cout << i << ": " << fitnessValues[i] << std::endl;
      population[i].value = functionValues[i] = fitnessValues[i];
    }

    // Generate index
    // std::cout << "[";
    // for (size_t i = 0; i < params.lambda; ++i) {
    //   std::cout << index[i] << ",";
    // }
    // std::cout << "] => [";
    sortIndex(fitnessValues, index, (size_t)params.lambda);
    // for (size_t i = 0; i < params.lambda; ++i) {
    //   std::cout << index[i] << ",";
    // }
    // std::cout << "]" << std::endl;

    // Test if function values are identical, escape flat fitness
    if (fitnessValues[index[0]] ==
        fitnessValues[index[(int)params.lambda / 2]]) {
      sigma *= std::exp(Scalar(0.2) + params.cs / params.damps);
      if (params.logWarnings) {
        params.logStream
            << "Warning: sigma increased due to equal function values"
            << std::endl
            << "   Reconsider the formulation of the objective function";
      }
    }

    // update function value history
    for (size_t i = historySize - 1; i > 0; --i)
      funcValueHistory[i] = funcValueHistory[i - 1];
    funcValueHistory[0] = fitnessValues[index[0]];

    // update xbestever
    // std::cout << population[index[0]].value << std::endl;
    if (xBestEver.value > population[index[0]].value || gen == 1) {
      // std::cout << xBestEver.value << " -> " << population[index[0]].value <<
      // std::endl;
      xBestEver = population[index[0]];
      // std::cout << xBestEver.value << std::endl;
      evalsBestEver = countevals;
      // for (int i = 0; i < N; ++i) {
      //   // xBestEver.x[i] = population[index[0]].x[i]; /* TODO: use
      //   std::vector copy assign */
      //   // xBestEver.value = population[index[0]].value;
      // }
    }

    const Scalar sqrtmueffdivsigma = std::sqrt(params.mueff) / sigma;
    // calculate xmean and rgBDz~N(0,C)
    for (size_t i = 0; i < N; ++i) {
      xold[i] = xmean[i];
      xmean[i] = 0.;
      for (int iNk = 0; iNk < params.mu; ++iNk)
        xmean[i] += params.weights[iNk] * population[index[iNk]].x[i];
      BDz[i] = sqrtmueffdivsigma * (xmean[i] - xold[i]);
    }

    // calculate z := D^(-1)* B^(-1)* rgBDz into rgdTmp
    for (int i = 0; i < N; ++i) {
      Scalar sum;
      if (diag)
        sum = BDz[i];
      else {
        sum = 0.;
        for (int j = 0; j < N; ++j)
          sum += B[j][i] * BDz[j];
      }
      tempRandom[i] = sum / rgD[i];
    }

    // cumulation for sigma (ps) using B*z
    const Scalar sqrtFactor = std::sqrt(params.cs * (Scalar(2) - params.cs));
    const Scalar invps = Scalar(1) - params.cs;
    for (int i = 0; i < N; ++i) {
      Scalar sum;
      if (diag)
        sum = tempRandom[i];
      else {
        sum = Scalar(0);
        Scalar *Bi = B[i];
        for (int j = 0; j < N; ++j)
          sum += Bi[j] * tempRandom[j];
      }
      ps[i] = invps * ps[i] + sqrtFactor * sum;
    }

    // calculate norm(ps)^2
    Scalar psxps(0);
    for (int i = 0; i < N; ++i) {
      const Scalar &rgpsi = ps[i];
      psxps += rgpsi * rgpsi;
    }

    // cumulation for covariance matrix (pc) using B*D*z~N(0,C)
    int hsig = std::sqrt(psxps) /
                   std::sqrt(Scalar(1) - std::pow(Scalar(1) - params.cs,
                                                  Scalar(2) * (Scalar)gen)) /
                   chiN <
               Scalar(1.4) + Scalar(2) / (N + 1);
    const Scalar ccumcovinv = 1. - params.ccumcov;
    const Scalar hsigFactor =
        hsig * std::sqrt(params.ccumcov * (Scalar(2) - params.ccumcov));
    for (int i = 0; i < N; ++i)
      pc[i] = ccumcovinv * pc[i] + hsigFactor * BDz[i];

    // update of C
    adaptC2(hsig);

    // update of sigma
    sigma *= std::exp(((std::sqrt(psxps) / chiN) - Scalar(1)) * params.cs /
                      params.damps);

    state = UPDATED;
    return xmean;
  }

  /**
   * Request a scalar parameter from CMA-ES.
   * @param key Key of the requested scalar.
   * @return The desired value.
   */
  Scalar get(GetScalar key) {
    switch (key) {
    case AxisRatio:
      return maxElement(rgD, (size_t)params.N) /
             minElement(rgD, (size_t)params.N);
    case Eval:
      return countevals;
    case Generation:
      return gen;
    case MaxEval:
      return params.stopMaxFunEvals;
    case MaxIter:
      return std::ceil(params.stopMaxIter);
    case MaxAxisLength:
      return sigma * std::sqrt(maxEW);
    case MinAxisLength:
      return sigma * std::sqrt(minEW);
    case MaxStdDev:
      return sigma * std::sqrt(maxdiagC);
    case MinStdDev:
      return sigma * std::sqrt(mindiagC);
    case Dimension:
      return params.N;
    case SampleSize:
      return params.lambda;
    case Sigma:
      return sigma;
    default:
      throw std::runtime_error("get(): No match found for key");
    }
  }

  /**
   * Request a parameter of object function type from CMA-ES.
   * @param key Key of the requested value.
   * @return The desired value.
   */
  Ordered getValue(GetScalar key) {
    switch (key) {
    case Fitness:
      return functionValues[index[0]];
    case FBestEver:
      return xBestEver.value;
    default:
      throw std::runtime_error("getValue(): No match found for key");
    }
  }

  /**
   * Request a vector parameter from CMA-ES.
   * @param key Key of the requested vector.
   * @return Pointer to the desired value array. Its content might be
   *         overwritten during the next call to any member functions other
   *         than get().
   */
  const std::vector<Scalar> getVector(GetVector key) {
    switch (key) {
    case DiagC: {
      std::vector<Scalar> o((size_t)params.N);
      for (size_t i = 0; i < params.N; ++i)
        o[i] = C[i][i];
      return o;
    }
    case DiagD:
      return std::vector<Scalar>(rgD, rgD + params.N);
    case StdDev: {
      std::vector<Scalar> o((size_t)params.N);
      for (size_t i = 0; i < params.N; ++i)
        o[i] = sigma * std::sqrt(C[i][i]);
      return o;
    }
    case XBestEver:
      return xBestEver.x;
    case XBest:
      return population[index[0]].x;
    case XMean:
      return std::vector<Scalar>(xmean, xmean + params.N);
    default:
      throw std::runtime_error("getVector(): No match found for key");
    }
  }

  /**
   * Request a vector parameter from CMA-ES.
   * @param key Key of the requested vector.
   * @return Pointer to the desired value array with unlimited reading and
   *         writing access to its elements. The memory must be explicitly
   *         released using delete[].
   */
  // Scalar *getNew(GetVector key) { return getInto(key, nullptr); }

  /**
   * Request a vector parameter from CMA-ES.
   * @param key Key of the requested vector.
   * @param res Memory of size N == dimension, where the desired values are
   *            written into. For mem == NULL new memory is allocated as with
   *            calling getNew() and must be released by the user at some point.
   */
  // Scalar *getInto(GetVector key, Scalar *res) {
  //   const std::vector<Scalar> res0 = getVector(key);
  //   if (!res)
  //     res = new Scalar[params.N];
  //   for (size_t i = 0; i < params.N; ++i)
  //     res[i] = res0[i];
  //   return res;
  // }

  /**
   * Some stopping criteria can be set in initials.par, with names starting
   * with stop... Internal stopping criteria include a maximal condition number
   * of about 10^15 for the covariance matrix and situations where the numerical
   * discretisation error in x-space becomes noticeably. You can get a message
   * that contains the matched stop criteria via getStopMessage().
   * @return Does any stop criterion match?
   */
  unsigned long testForTermination(bool makeMessage = false) {
    Ordered range;
    Scalar fac;
    int iAchse, iKoo;
    int diag = params.diagonalCov == 1 || params.diagonalCov >= gen;
    int N = params.N;
    // std::stringstream message;

    // if (stopMessage != "") {
    //   message << stopMessage << std::endl;
    // }

    // 0. function value reached
    {
      stopCriteria[0].first =
          (gen > 1 || state > SAMPLED) && params.stStopFitness.flg &&
          functionValues[index[0]] <= params.stStopFitness.val;
      if (stopCriteria[0].first || makeMessage) {
        std::stringstream message;
        message << "Fitness: function value " << functionValues[index[0]]
                << " <= stopFitness (" << params.stStopFitness.val << ")";
        stopCriteria[0].second = message.str();
      }
    }

    // 1. TolFun
    {
      range = std::max(maxElement(funcValueHistory,
                                  std::min((size_t)gen, historySize)),
                       maxElement(functionValues, (size_t)params.lambda)) -
              std::min(minElement(funcValueHistory,
                                  std::min((size_t)gen, historySize)),
                       minElement(functionValues, (size_t)params.lambda));

      stopCriteria[1].first = gen > 0 && range <= params.stopTolFun;
      if (stopCriteria[1].first || makeMessage) {
        std::stringstream message;
        message << "TolFun: function value differences " << range
                << " < stopTolFun=" << params.stopTolFun;
        stopCriteria[1].second = message.str();
      }
    }

    // 2. TolFunHist
    {
      if (gen > historySize) {
        range = maxElement(funcValueHistory, historySize) -
                minElement(funcValueHistory, historySize);
        if (range <= params.stopTolFunHist)
          stopCriteria[2].first = true;
      } else {
        stopCriteria[2].first = false;
      }
      if (stopCriteria[2].first || makeMessage) {
        std::stringstream message;
        message << "TolFunHist: history of function value changes " << range
                << " stopTolFunHist=" << params.stopTolFunHist;
        stopCriteria[2].second = message.str();
      }
    }

    // 3. TolX
    {
      int cTemp = 0;
      for (int i = 0; i < N; ++i) {
        cTemp += (sigma * std::sqrt(C[i][i]) < params.stopTolX) ? 1 : 0;
        cTemp += (sigma * pc[i] < params.stopTolX) ? 1 : 0;
      }
      stopCriteria[3].first = cTemp == 2 * N;
      if (stopCriteria[3].first || makeMessage) {
        std::stringstream message;
        message << "TolX: object variable changes below " << params.stopTolX;
        stopCriteria[3].second = message.str();
      }
    }

    // 4. TolUpX
    {
      stopCriteria[4].first = false;
      for (int i = 0; i < N; ++i) {
        if (sigma * std::sqrt(C[i][i]) >
            params.stopTolUpXFactor * params.rgInitialStds[i]) {
          stopCriteria[4].first = true;
          break;
        }
      }
      if (stopCriteria[4].first || makeMessage) {
        std::stringstream message;
        message << "TolUpX: standard deviation increased by more than "
                << params.stopTolUpXFactor
                << ", larger initial standard deviation recommended.";
        stopCriteria[4].second = message.str();
      }
    }

    // 5. Condition of C greater than dMaxSignifKond
    {
      stopCriteria[5].first = maxEW >= minEW * dMaxSignifKond;
      if (stopCriteria[5].first || makeMessage) {
        std::stringstream message;
        message << "ConditionNumber: maximal condition number "
                << dMaxSignifKond << " reached. maxEW=" << maxEW
                << ",minEW=" << minEW << ",maxdiagC=" << maxdiagC
                << ",maxdiagCi=" << maxdiagCi << ",mindiagC=" << mindiagC
                << ",mindiagCi=" << mindiagCi;
        stopCriteria[5].second = message.str();
      }
    }

    // 6. Principal axis i has no effect on xmean, ie. x == x + 0.1* sigma*
    // rgD[i]* B[i]
    {
      if (!diag) {
        std::vector<std::pair<double, int>> no_effect;
        for (iAchse = 0; iAchse < N; ++iAchse) {
          fac = 0.1 * sigma * rgD[iAchse];
          for (iKoo = 0; iKoo < N; ++iKoo) {
            if (xmean[iKoo] != xmean[iKoo] + fac * B[iKoo][iAchse])
              break;
          }
          if (iKoo == N) {
            stopCriteria[6].first = true;
            no_effect.emplace_back(fac / 0.1, iAchse);
            break;
          }
        }
        if (stopCriteria[6].first || makeMessage) {
          std::stringstream message;
          message << "NoEffectAxis: ";
          for (const auto &p : no_effect)
            message << "[axis " << p.second << " std dev 0.1*" << p.first
                    << "] ";
          stopCriteria[6].second = message.str();
        }
      } else if (makeMessage) {
        stopCriteria[6].second = "NoEffectAxis: (empty)";
      }
    }

    // 7. Component of xmean is not changed anymore
    {
      for (iKoo = 0; iKoo < N; ++iKoo) {
        if (xmean[iKoo] ==
            xmean[iKoo] + sigma * std::sqrt(C[iKoo][iKoo]) / Scalar(5)) {
          stopCriteria[7].first = true;
          break;
        }
      }
      if (stopCriteria[7].first) {
        std::stringstream message;
        message << "NoEffectCoordinate: standard deviation 0.2*"
                << (sigma * std::sqrt(C[iKoo][iKoo])) << " in coordinate "
                << iKoo << " without effect";
        stopCriteria[7].second = message.str();
      } else if (makeMessage) {
        stopCriteria[7].second = "NoEffectCoordinate: (empty)";
      }
    }

    // 8. Maximum function evaluations counts is reached
    {
      stopCriteria[8].first = countevals >= params.stopMaxFunEvals;
      if (stopCriteria[8].first || makeMessage) {
        std::stringstream message;
        message << "MaxFunEvals: conducted function evaluations " << countevals
                << " >= " << params.stopMaxFunEvals;
        stopCriteria[8].second = message.str();
      }
    }

    // 9. Maximum generation counts is reached
    {
      stopCriteria[9].first = gen >= params.stopMaxIter;
      if (stopCriteria[9].first || makeMessage) {
        std::stringstream message;
        message << "MaxIter: number of iterations " << gen
                << " >= " << params.stopMaxIter;
        stopCriteria[9].second = message.str();
      }
    }

    size_t crit_n = stopCriteria.size();
    unsigned long flags = 0;
    for (size_t i = 0; i < crit_n; ++i)
      flags |= (unsigned long)stopCriteria[i].first << i;
    return flags;
  }

  /**
   * A message that contains a detailed description of the matched stop
   * criteria.
   */
  std::string getStopMessage() {
    testForTermination(true); /* generate all messages */
    std::stringstream message;
    for (const std::pair<bool, std::string> &p : stopCriteria)
      message << "[" << (p.first ? "x" : " ") << "] " << p.second << std::endl;
    return message.str();
  }

  /**
   * @param filename Output file name.
   * @param key Key of type WriteKey that indicates the content that should be
   *            written. You can combine multiple keys with |.
   */
  void writeToFile(int key, const std::string &filename) {
    std::ofstream file;
    file.open(filename.c_str(), std::ios_base::app);

    if (file.is_open()) {
      if (gen > 0 || filename.substr(0, 11) != "outcmaesfit")
        writeToStream(key, file); /* do not write fitness for gen==0 */
      file.close();
    } else {
      throw std::runtime_error("writeToFile(): could not open '" + filename +
                               "'");
    }
  }

  /**
   * Conducts the eigendecomposition of C into B and D such that
   * \f$C = B \cdot D \cdot D \cdot B^Scalar\f$ and \f$B \cdot B^Scalar = I\f$
   * and D diagonal and positive.
   * @param force For force == true the eigendecomposion is conducted even if
   *              eigenvector and values seem to be up to date.
   */
  void updateEigensystem(bool force) {
    eigenTimings.update();

    if (!force) {
      if (eigensysIsUptodate)
        return;
      // return on modulo generation number
      if (gen < genOfEigensysUpdate + params.updateCmode.modulo)
        return;
      // return on time percentage
      if (params.updateCmode.maxtime < 1.00 &&
          eigenTimings.tictoctime >
              params.updateCmode.maxtime * eigenTimings.totaltime &&
          eigenTimings.tictoctime > 0.0002)
        return;
    }

    eigenTimings.tic();
    eigen(rgD, B, tempRandom);
    eigenTimings.toc();

    // find largest and smallest eigenvalue, they are supposed to be sorted
    // anyway
    minEW = minElement(rgD, (size_t)params.N);
    maxEW = maxElement(rgD, (size_t)params.N);

    if (doCheckEigen) // needs O(n^3)! writes, in case, error message in error
                      // file
      checkEigen(rgD, B);

    for (int i = 0; i < params.N; ++i)
      rgD[i] = std::sqrt(rgD[i]);

    eigensysIsUptodate = true;
    genOfEigensysUpdate = gen;
  }

  /**
   * Distribution mean could be changed before samplePopulation(). This might
   * lead to unexpected behaviour if done repeatedly.
   * @param newxmean new mean, if it is NULL, it will be set to the current mean
   * @return new mean
   */
  Scalar const *setMean(const Scalar *newxmean) {
    assert(state != SAMPLED && "setMean: mean cannot be set inbetween the calls"
                               "of samplePopulation and updateDistribution");

    if (newxmean && newxmean != xmean)
      for (int i = 0; i < params.N; ++i)
        xmean[i] = newxmean[i];
    else
      newxmean = xmean;

    return newxmean;
  }
};
