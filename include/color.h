/*
 * color.h
 * Part of http://github.com/gfiumara/color by Gregory Fiumara.
 * See LICENSE for details.
 */

#pragma once

#include <ostream>

namespace color {
/** A color in CIELAB colorspace */
struct LAB {
  double l;
  double a;
  double b;
};
using LAB = struct LAB;

/** A color in LCH colorspace */
struct LCH {
  double l;
  double c;
  double h;
};
using LCH = struct LCH;

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

/** A color in CMY colorspace */
struct CMY {
  double c;
  double m;
  double y;
};
using CMY = struct CMY;

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

constexpr double deg2Rad(const double deg) { return (deg * (M_PI / 180.0)); }
constexpr double rad2Deg(const double rad) { return ((180.0 / M_PI) * rad); }

RGB XYZtoRGB(const XYZ &xyz);
XYZ LABtoXYZ(const LAB &lab);
LAB XYZtoLAB(const XYZ &xyz);
CMY RGBtoCMY(const RGB &rgb);

inline RGB LABtoRGB(const LAB &lab) { return XYZtoRGB(LABtoXYZ(lab)); }

inline LCH LABtoLCH(const LAB &lab) {
  double theta = atan2(lab.b, lab.a);
  if (theta < 0)
    theta += 2 * M_PI;
  return LCH{lab.l, sqrt(lab.a * lab.a + lab.b * lab.b), color::rad2Deg(theta)};
}

inline LAB LCHtoLAB(const LCH &lch) {
  const double theta = color::deg2Rad(lch.h);
  return LAB{lch.l, lch.c * cos(theta), lch.c * sin(theta)};
}

// @param T temperature in Kelvin 4000 < T < 25000
// @param y luminosity in XYZ
XYZ IlluminantDKelvin(double T, double y);

// @param cx chromaticity x in xyY
// @param y luminosity in XYZ
XYZ IlluminantDChromaticity(double cx, double y);

} // namespace color

std::ostream &operator<<(std::ostream &s, const color::LAB &lab);
std::ostream &operator<<(std::ostream &s, const color::LCH &lch);
std::ostream &operator<<(std::ostream &s, const color::XYZ &xyz);
std::ostream &operator<<(std::ostream &s, const color::RGB &rgb);
std::ostream &operator<<(std::ostream &s, const color::CMY &rgb);
