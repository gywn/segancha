#include <algorithm>
#include <cma-es/cmaes.h>
#include <cmath>
#include <color.h>
#include <fitness.h>
#include <iostream>
#include <lexi.h>
#include <utility>

inline double offRange(double x, double a, double b) {
  if (x < a)
    return a - x;
  if (x > b)
    return x - b;
  return 0;
}

double offRGB(const color::LAB &lab) {
  const color::RGB rgb{color::LABtoRGB(lab)};
  const auto dr = offRange(rgb.r, 0, 1);
  const auto dg = offRange(rgb.g, 0, 1);
  const auto db = offRange(rgb.b, 0, 1);

  return std::sqrt(dr * dr + dg * dg + db * db);
}

class Combination {
public:
  size_t i;
  size_t j;
};

LexiProduct<double> fitnessFunc(const std::vector<color::LAB> &lab, size_t M) {
  size_t totalM = lab.size();
  std::vector<double> penalty(totalM);
  if (M == 0)
    M = totalM;
  for (size_t i = 0; i < M; ++i)
    // 300 is the approx. diameter of ab-plane
    penalty[i] = offRGB(lab[i]) * 300 * totalM;

  // number of combinations between free/fixed colors
  size_t K = (M * (2 * totalM - M - 1)) / 2;
  static size_t cached_K = 0;
  static size_t cached_M = 0;
  static std::vector<size_t> cached_ref; /* for almost-sorted O(n) sorting */
  static std::vector<Combination> ref_combi;

  // Cache combination order
  if (!(cached_K == K && cached_M == M)) {
    cached_K = K;
    cached_M = M;
    cached_ref.resize(K);
    for (size_t i = 0; i < K; ++i)
      cached_ref[i] = i;
    ref_combi.resize(K);
    size_t combi_i = 0;
    for (size_t i = 0; i < M; ++i) {
      for (size_t j = i + 1; j < totalM; ++j) {
        ref_combi[combi_i] = Combination{i, j};
        ++combi_i;
      }
    }
  }

  std::vector<std::pair<double, size_t>> fitness_ref(K);
  for (size_t i = 0; i < K; ++i) {
    const auto ref = cached_ref[i];
    const auto &combi = ref_combi[ref];
    fitness_ref[i] = std::pair<double, size_t>(
        penalty[combi.i] + penalty[combi.j] -
            color::CIEDE2000(lab[combi.i], lab[combi.j]),
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

void fill_lab(const std::vector<double> &x, double L,
              std::vector<color::LAB> &lab, size_t M) {
  for (size_t j = 0; j < M; ++j)
    lab[j] = color::LAB{L, x[2 * j], x[2 * j + 1]};
}

std::ostream &operator<<(std::ostream &os, const PerceptionResult &res) {
  os << "{" << std::endl << "  flags: " << res.flags << std::endl;
  os << "  L: " << res.L << std::endl;
  os << "  rgb: [" << std::endl;
  for (const auto &rgb : res.rgb)
    os << "    " << rgb << std::endl;
  os << "  ]" << std::endl;
  os << "  fitness: " << res.fitness << std::endl << "}";
  return os;
}

PerceptionResult perception(size_t M, double L,
                            std::vector<color::LAB> const *fixed, bool quiet) {
  bool freeL = L < 0;
  bool noFixed = fixed == nullptr || fixed->size() == 0;
  size_t fixedM = fixed == nullptr ? 0 : fixed->size();

  CMAES<double, LexiProduct<double>> evo;
  Individual<double, LexiProduct<double>> *pop;

  const size_t N = M * 2 + freeL; //!< number of variables
  std::vector<double> xstart(N);
  const double initL = noFixed
                           ? 50
                           : accumulate(fixed->begin(), fixed->end(), 0,
                                        [](double accu, const color::LAB &c) {
                                          return accu + c.l;
                                        }) /
                                 (double)fixedM;
  std::vector<double> stddev(N, std::min(100. - initL, initL));
  if (freeL) {
    xstart[2 * M] = initL;
    stddev[2 * M] = 100.;
  }

  if (!quiet)
    std::cout << "freeM=" << M << ", fixedM=" << fixedM << ", initL=" << initL
              << std::endl;

  Parameters<double, LexiProduct<double>> parameters;
  parameters.lambda = (int)(300. * log(N)); /* 100x default */
  // parameters.stopTolFun = {0, 0, 1e-13};
  // parameters.stopTolFunHist = {0, 0, 1e-13};
  parameters.init((int)N, xstart.data(), stddev.data());
  evo.init(parameters);

  if (!quiet)
    std::cout << evo.sayHello() << std::endl;

  std::vector<color::LAB> lab(M + fixedM);
  if (!noFixed)
    std::copy(fixed->begin(), fixed->end(), lab.begin() + (long)M);

  std::vector<LexiProduct<double>> populationFitness((size_t)parameters.lambda);
  unsigned long flags = 0;
  while (!(flags = evo.testForTermination())) {
    pop = evo.samplePopulation(); // Do not change content of pop
    for (size_t i = 0; i < parameters.lambda; ++i) {
      fill_lab(pop[i].x, freeL ? pop[i].x[2 * M] : L, lab, M);
      populationFitness[i] = fitnessFunc(lab, M);
    }
    evo.updateDistribution(populationFitness);
  }

  const auto xfinal = evo.getVector(CMAES<double, LexiProduct<double>>::XMean);
  fill_lab(xfinal, freeL ? xfinal[2 * M] : L, lab, M);
  std::sort(lab.begin(), lab.begin() + (long)M,
            [](const color::LAB &c1, const color::LAB &c2) {
              const double h1 = std::atan2(-c1.b, -c1.a);
              const double h2 = std::atan2(-c2.b, -c2.a);
              return h1 < h2;
            });
  auto fitness = evo.getValue(CMAES<double, LexiProduct<double>>::FBestEver);

  if (!quiet)
    std::cout << "Stop: flags " << flags << std::endl << evo.getStopMessage();

  std::vector<color::RGB> rgb(M);
  for (size_t i = 0; i < M; ++i)
    rgb[i] = color::LABtoRGB(lab[i]);

  return PerceptionResult{flags, freeL ? xfinal[2 * M] : L, std::move(rgb),
                          std::move(fitness)};
}