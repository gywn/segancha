/*
 * color.h
 * Part of http://github.com/gfiumara/color by Gregory Fiumara.
 * See LICENSE for details.
 */

#ifndef GPF_COLOR_H_
#define GPF_COLOR_H_

#include <ostream>

namespace color {
/** A color in CIELAB colorspace */
struct LAB {
  double l;
  double a;
  double b;
};
using LAB = struct LAB;

/** A color in CIEXYZ colorspace */
struct XYZ {
  double x;
  double y;
  double z;
};
using XYZ = struct XYZ;

/** A color in CIERGB colorspace */
struct RGB {
  double r;
  double g;
  double b;
};
using RGB = struct RGB;

/**
 * @brief
 * Obtain Delta-E 2000 value.
 * @details
 * Based on the paper "The color Color-Difference Formula:
 * Implementation Notes, Supplementary Test Data, and Mathematical
 * Observations" by Gaurav Sharma, Wencheng Wu, and Edul N. Dalal,
 * from http://www.ece.rochester.edu/~gsharma/ciede2000/.
 *
 * @param lab1
 * First color in LAB colorspace.
 * @param lab2
 * Second color in LAB colorspace.
 *
 * @return
 * Delta-E difference between lab1 and lab2.
 */
double CIEDE2000(const LAB &lab1, const LAB &lab2);

constexpr double deg2Rad(const double deg);
constexpr double rad2Deg(const double rad);

RGB XYZtoRGB(const XYZ &xyz);
XYZ LABtoXYZ(const LAB &lab);
RGB LABtoRGB(const LAB &lab);

bool offRGB(const LAB &lab);

} // namespace color

std::ostream &operator<<(std::ostream &s, const color::LAB &labColor);
std::ostream &operator<<(std::ostream &s, const color::XYZ &labColor);
std::ostream &operator<<(std::ostream &s, const color::RGB &labColor);

#endif /* GPF_COLOR_H_ */
