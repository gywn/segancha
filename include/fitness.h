#pragma once

#include "color.h"
#include "lexi.h"
#include <ostream>
#include <vector>

inline double offRange(double x, double a, double b);

double offRGB(const color::LAB &lab);

LexiProduct<double> fitnessFunc(const std::vector<color::LAB> &lab);

class PerceptionResult {
public:
  unsigned long flags;
  double L;
  std::vector<color::RGB> rgb;
  LexiProduct<double> fitness;
};

std::ostream &operator<<(std::ostream &os, const PerceptionResult &res);

PerceptionResult perceptionL(color::LAB foreground, color::LAB background,
                             size_t M, bool quiet = false);