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

LexiProduct<double> fitnessFunc(const std::vector<color::LAB> &lab) {
  // number of colors
  size_t M = lab.size();
  std::vector<double> penalty(M);
  for (size_t i = 0; i < M; ++i) {
    penalty[i] =
        offRGB(lab[i]) * 300 * M; /* 300 is the approx. diameter of ab-plane */
  }

  // number of combinations between colors, except the last two
  size_t K = M * (M - 1) / 2 - 1; 
  static size_t cached_K = 0;
  static std::vector<size_t> cached_ref; /* for almost-sorted O(n) sorting */
  static std::vector<std::pair<size_t, size_t>> ref_combi;

  // Cache combination order
  if (cached_K != K) {
    cached_K = K;
    cached_ref.resize(K);
    for (size_t i = 0; i < K; ++i) {
      cached_ref[i] = i;
    }
    ref_combi.resize(K);
    size_t combi_i = 0;
    for (size_t i = 0; i < M - 2; ++i) {
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

void fill_lab(const std::vector<double> &x, std::vector<color::LAB> &lab,
              size_t M) {
  for (size_t j = 0; j < M; ++j) {
    lab[j] = color::LAB{x[2 * M], x[2 * j], x[2 * j + 1]};
  }
}

/*
 * @param foreground
 * @param background
 * @param M numbers of colors
 * @param quiet write info to stdout
 * @return PerceptionResult
 */
PerceptionResult perceptionL(color::LAB foreground, color::LAB background,
                             size_t M, bool quiet) {
  CMAES<double, LexiProduct<double>> evo;
  Individual<double, LexiProduct<double>> *pop;

  const size_t N = M * 2 + 1; //!< number of variables
  std::vector<double> xstart(N);
  const double initL = (foreground.l + background.l) / 2.;
  std::vector<double> stddev(N, std::min(100. - initL, initL));
  xstart[2 * M] = initL; // luminocity
  stddev[2 * M] = 100.;  // luminocity

  Parameters<double, LexiProduct<double>> parameters;
  parameters.lambda = (int)(300. * log(N)); /* 100x default */
  // parameters.stopTolFun = {0, 0, 1e-13};
  // parameters.stopTolFunHist = {0, 0, 1e-13};
  parameters.init((int)N, xstart.data(), stddev.data());
  evo.init(parameters);

  if (!quiet)
    std::cout << evo.sayHello() << std::endl;

  std::vector<color::LAB> lab(M + 2);
  lab[M] = foreground;
  lab[M + 1] = background;

  std::vector<LexiProduct<double>> populationFitness((size_t)parameters.lambda);
  unsigned long flags = 0;
  while (!(flags = evo.testForTermination())) {
    pop = evo.samplePopulation(); // Do not change content of pop
    for (size_t i = 0; i < parameters.lambda; ++i) {
      fill_lab(pop[i].x, lab, M);
      populationFitness[i] = fitnessFunc(lab);
    }
    evo.updateDistribution(populationFitness);
  }

  const auto xfinal = evo.getVector(CMAES<double, LexiProduct<double>>::XMean);
  fill_lab(xfinal, lab, M);
  std::sort(lab.begin(), lab.begin() + (long)M, [](const color::LAB &c1, const color::LAB &c2) {
    const double h1 = std::atan2(-c1.b, -c1.a);
    const double h2 = std::atan2(-c2.b, -c2.a);
    return h1 < h2;
  });
  auto fitness = evo.getValue(CMAES<double, LexiProduct<double>>::FBestEver);

  if (!quiet)
    std::cout << "Stop: flags " << flags << std::endl << evo.getStopMessage();

  std::vector<color::RGB> rgb(M);
  for (size_t i = 0; i < M; ++i) {
    rgb[i] = color::LABtoRGB(lab[i]);
  }

  return PerceptionResult{flags, xfinal[2 * M], std::move(rgb),
                          std::move(fitness)};
}