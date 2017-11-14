/*
 * color.h
 * Part of http://github.com/gfiumara/color by Gregory Fiumara.
 * See LICENSE for details.
 */

#ifndef GPF_COLOR_H_
#define GPF_COLOR_H_

#include <ostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /* pi */
#endif

/** Namespace containing all necessary objects and methods for color */
namespace color {
/***********************************************************************
 * Types.
 **********************************************************************/

/** A color in CIELAB colorspace */
struct LAB {
  /** Lightness */
  double l;
  /** Color-opponent a dimension */
  double a;
  /** Color-opponent b dimension */
  double b;
};
/** Convenience definition for struct LAB */
using LAB = struct LAB;

/** A color in CIEXYZ colorspace */
struct XYZ {
  double x;
  double y;
  double z;
};
/** Convenience definition for struct XYZ */
using XYZ = struct XYZ;

/** A color in CIERGB colorspace */
struct RGB {
  double r;
  double g;
  double b;
};
/** Convenience definition for struct RGB */
using RGB = struct RGB;

/***********************************************************************
 * Operations.
 **********************************************************************/

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

/***********************************************************************
 * Conversions.
 **********************************************************************/

/**
 * @brief
 * Convert degrees to radians.
 *
 * @param deg
 * Angle in degrees.
 *
 * @return
 * deg in radians.
 */
constexpr double deg2Rad(const double deg);

/**
 * @brief
 * Convert radians to degrees.
 *
 * @param rad
 * Angle in radians.
 *
 * @return
 * rad in degrees.
 */
constexpr double rad2Deg(const double rad);

/*******************************************************************************
 * Conversions.
 ******************************************************************************/

} // namespace color
/**
 * @brief
 * LAB output stream operator.
 *
 * @param s
 * Output stream.
 * @param labColor
 * Color to output.
 *
 * @return
 * s with labColor appended.
 */
std::ostream &operator<<(std::ostream &s, const color::LAB &labColor);

#endif /* GPF_COLOR_H_ */
