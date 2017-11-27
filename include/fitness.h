#pragma once

#include "color.h"
#include "lexi.h"
#include <ostream>
#include <vector>

inline double offRange(double x, double a, double b);

double offRGB(const color::LAB &lab);
double offRGB(const color::LCH &lch);

double offChroma(const color::LAB &lab, double C);
double offChroma(const color::LCH &lch, double C);

/**
 * @param lab a vector of color::LAB, inter-distances of lab[freeM:] are ignored
 * @param M 0 < M <= lab.size(), set to lab.size() if 0
 * @param maxC maximal chroma, < 0 to be ignored
 * @return a lexicographical product of distances
 */
LexiProduct<double> fitnessFunc(const std::vector<color::LAB> &lab,
                                size_t M = 0, double maxC = -1.);

struct PerceptionResult {
  unsigned long flags;
  double L;
  double maxC;
  std::vector<color::LAB> lab;
  std::vector<color::RGB> rgb;
  LexiProduct<double> fitness;
};
using PerceptionResult = struct PerceptionResult;

std::ostream &operator<<(std::ostream &os, const PerceptionResult &res);

/*
 * @param M numbers of free colors
 * @param L luminocity constraint, < 0 to be ignored
 * @param maxC maximal chroma, < 0 to be ignored
 * @param fixed vector of fixed colors, optional
 * @param quiet don't write info to stdout
 * @return PerceptionResult
 */
PerceptionResult perception(size_t M, double L = -1, double maxC = -1,
                            std::vector<color::LAB> const *fixed = nullptr,
                            bool quiet = false);

color::LCH maxChroma(const color::LCH &lch, double maxC = -1);
