#ifndef FITNESS_H_
#define FITNESS_H_

#include "color.h"
#include "lexi.h"
#include <ostream>
#include <vector>

inline double offRange(double x, double a, double b);

double offRGB(const color::LAB &lab);

LexiProduct<double> fitnessFunc(const std::vector<color::LAB> &lab);

class PerceptionResult {
public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const PerceptionResult &res) {
    os << "{" << std::endl << "  flags: " << res.flags << std::endl;
    os << "  L: " << res.L << std::endl;
    os << "  rgb: [" << std::endl;
    for (const auto &rgb : res.rgb)
      os << "    0: " << rgb << std::endl;
    os << "  ]" << std::endl;
    os << "  fitness: " << res.fitness << std::endl << "}";
    return os;
  }

  unsigned long flags;
  double L;
  std::vector<color::RGB> rgb;
  LexiProduct<double> fitness;
};

PerceptionResult perceptionL(color::LAB foreground, color::LAB background,
                             size_t M, bool quiet = false);

#endif