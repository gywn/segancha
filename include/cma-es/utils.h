/**
 * @file utils.h
 * Contains some utility functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <string>

template <typename Scalar> inline Scalar square(Scalar d) { return d * d; }

template <typename Scalar>
inline Scalar maxElement(const Scalar *rgd, size_t len) {
  return *std::max_element(rgd, rgd + len);
}

template <typename Scalar>
inline Scalar minElement(const Scalar *rgd, size_t len) {
  return *std::min_element(rgd, rgd + len);
}

template <typename Scalar>
inline size_t maxIndex(const Scalar *rgd, size_t len) {
  return (size_t)(std::max_element(rgd, rgd + len) - rgd);
}

template <typename Scalar>
inline size_t minIndex(const Scalar *rgd, size_t len) {
  return (size_t)(std::min_element(rgd, rgd + len) - rgd);
}

/** sqrt(a^2 + b^2) numerically stable. */
template <typename Scalar> Scalar myhypot(Scalar a, Scalar b) {
  const Scalar fabsa = std::fabs(a), fabsb = std::fabs(b);
  if (fabsa > fabsb) {
    const Scalar r = b / a;
    return fabsa * std::sqrt(Scalar(1) + r * r);
  } else if (b != Scalar(0)) {
    const Scalar r = a / b;
    return fabsb * std::sqrt(Scalar(1) + r * r);
  } else
    return Scalar(0);
}
