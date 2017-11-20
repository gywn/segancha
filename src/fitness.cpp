#include <algorithm>
#include <cma-es/cmaes.h>
#include <cmath>
#include <color.h>
#include <fitness.h>
#include <iostream>
#include <lexi.h>
#include <utility>

inline double offRange(double x, double a, double b) {
  if (x < a) {
    return a - x;
  }
  if (x > b) {
    return x - b;
  }
  return 0;
}

double offRGB(const color::LAB &lab) {
  const color::RGB rgb{color::LABtoRGB(lab)};
  const auto dr = offRange(rgb.r, 0, 1);
  const auto dg = offRange(rgb.g, 0, 1);
  const auto db = offRange(rgb.b, 0, 1);

  return std::sqrt(dr * dr + dg * dg + db * db);
}

LexiProduct<double> fitnessFunc(double L, const std::vector<double> &x) {
  // number of colors
  size_t M = x.size() / 2;
  std::vector<color::LAB> lab(M);
  // off-RGB penalty
  std::vector<double> penalty(M);
  for (size_t i = 0; i < M; ++i) {
    lab[i] = color::LAB{L, x[2 * i], x[2 * i + 1]};
    penalty[i] =
        offRGB(lab[i]) * 300 * M; /* 300 is the approx. diameter of ab-plane */
  }

  size_t K = M * (M - 1) / 2; /* number of combinations between colors */
  static size_t cached_K = 0;
  static std::vector<size_t> cached_ref; /* for almost-sorted O(n) sorting */
  static std::vector<std::pair<size_t, size_t>> ref_combi;

  if (cached_K != K) {
    cached_K = K;
    cached_ref.resize(K);
    for (size_t i = 0; i < K; ++i) {
      cached_ref[i] = i;
    }
    ref_combi.resize(K);
    size_t combi_i = 0;
    for (size_t i = 0; i < M - 1; ++i) {
      for (size_t j = i + 1; j < M; ++j) {
        ref_combi[combi_i] = std::pair<size_t, size_t>(i, j);
        ++combi_i;
      }
    }
  }

  std::vector<std::pair<double, size_t>> fitness_ref(K);
  for (size_t i = 0; i < K; ++i) {
    const auto ref = cached_ref[i];
    const auto &refs = ref_combi[ref];
    fitness_ref[i] = std::pair<double, size_t>(
        penalty[refs.first] + penalty[refs.second] -
            color::CIEDE2000(lab[refs.first], lab[refs.second]),
        ref);
  }
  std::sort(
      fitness_ref.begin(), fitness_ref.end(),
      [](const std::pair<double, size_t> &a,
         const std::pair<double, size_t> &b) { return a.first > b.first; });
  std::vector<double> fitness(K);
  for (size_t i = 0; i < K; ++i) {
    const auto &pair = fitness_ref[i];
    fitness[i] = pair.first;
    cached_ref[i] = pair.second;
  }
  return LexiProduct<double>(std::move(fitness));
}

/*
 * @param L luminosity
 * @param M numbers of colors
 * @param quiet write info to stdout
 * @return pair(RGBs, bestFitness)
 */
std::pair<std::vector<color::RGB>, LexiProduct<double>>
perceptionL(double L, size_t M, bool quiet) {
  CMAES<double, LexiProduct<double>> evo;
  Individual<double, LexiProduct<double>> *pop;

  const size_t N = M * 2;           //!< number of variables
  const size_t K = M * (M - 1) / 2; //!< number of combinations
  Parameters<double, LexiProduct<double>> parameters;
  parameters.lambda = (int)(100. * log(N));
  // parameters.diagonalCov = 1;
  // parameters.stopTolFun = 1e-12;
  // parameters.stopTolFunHist = 1e-13;
  const std::vector<double> xstart(N, 0.), stddev(N, std::min(100. - L, L));
  parameters.init((int)N, xstart.data(), stddev.data());
  evo.init(parameters);

  if (!quiet)
    std::cout << evo.sayHello() << std::endl;

  std::vector<LexiProduct<double>> populationFitness((size_t)parameters.lambda);
  while (!evo.testForTermination()) {
    pop = evo.samplePopulation(); // Do not change content of pop
    for (size_t i = 0; i < evo.get(CMAES<double, LexiProduct<double>>::Lambda);
         ++i)
      populationFitness[i] = fitnessFunc(50., pop[i].x);
    evo.updateDistribution(populationFitness);
  }

  if (!quiet)
    std::cout << "Stop:" << std::endl << evo.getStopMessage();

  auto x = evo.getVector(CMAES<double, LexiProduct<double>>::XMean);
  auto fitness = evo.getValue(CMAES<double, LexiProduct<double>>::FBestEver);

  std::vector<color::RGB> rgb(M);
  for (size_t i = 0; i < M; ++i) {
    rgb[i] = color::LABtoRGB(color::LAB{50., x[2 * i], x[2 * i + 1]});
  }

  return std::pair<std::vector<color::RGB>, LexiProduct<double>>(
      std::move(rgb), std::move(fitness));
}