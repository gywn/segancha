#define _USE_MATH_DEFINES
#include <cmath>
#include <color.h>
#include <constant.h>

namespace color {
double sgn(double val) { return (0. < val) - (val < 0.); }
constexpr inline double pow3(double x) { return x * x * x; }

RGB XYZtoRGB(const XYZ &xyz) {
  double r = xyz.x * A11 + xyz.y * A12 + xyz.z * A13;
  double g = xyz.x * A21 + xyz.y * A22 + xyz.z * A23;
  double b = xyz.x * A31 + xyz.y * A32 + xyz.z * A33;

  r = ((abs(r) > 0.0031308) ? sgn(r) * (1.055 * pow(abs(r), 1 / 2.4) - 0.055) : (12.92 * r));
  g = ((abs(g) > 0.0031308) ? sgn(g) * (1.055 * pow(abs(g), 1 / 2.4) - 0.055) : (12.92 * g));
  b = ((abs(b) > 0.0031308) ? sgn(b) * (1.055 * pow(abs(b), 1 / 2.4) - 0.055) : (12.92 * b));

  return RGB{r, g, b};
}

XYZ LABtoXYZ(const LAB &lab) {
  double y = (lab.l + 16.0) / 116.0;
  double x = lab.a / 500.0 + y;
  double z = y - lab.b / 200.0;

  double x3 = pow3(x);
  double y3 = pow3(y);
  double z3 = pow3(z);

  x = ((x3 > 0.008856) ? x3 : ((x - 16.0 / 116.0) / 7.787)) * PCS_X;
  y = ((y3 > 0.008856) ? y3 : ((y - 16.0 / 116.0) / 7.787)) * PCS_Y;
  z = ((z3 > 0.008856) ? z3 : ((z - 16.0 / 116.0) / 7.787)) * PCS_Z;

  return XYZ{x, y, z};
}

RGB LABtoRGB(const LAB &lab) { return XYZtoRGB(LABtoXYZ(lab)); }

bool offRGB(const LAB &lab) {
  const RGB rgb{LABtoRGB(lab)};

  return rgb.r >= 0 && rgb.r <= 1 && rgb.g >= 0 && rgb.g <= 1 && rgb.b >= 0 &&
         rgb.b <= 1;
}

} // namespace color
