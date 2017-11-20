#ifndef FITNESS_H_
#define FITNESS_H_

#include "color.h"
#include "lexi.h"
#include <vector>

inline double offRange(double x, double a, double b);

double offRGB(const color::LAB &lab);

LexiProduct<double> fitnessFunc(double L, const std::vector<double> &x);

std::pair<std::vector<color::RGB>, LexiProduct<double>>
perceptionL(double L, size_t M, bool quiet = false);

#endif