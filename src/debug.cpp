#include <color.h>

std::ostream &operator<<(std::ostream &s, const color::LAB &lab) {
  return (s << "LAB(" << lab.l << "," << lab.a << "," << lab.b << ")");
}

std::ostream &operator<<(std::ostream &s, const color::XYZ &xyz) {
  return (s << "XYZ(" << xyz.x << "," << xyz.y << "," << xyz.z << ")");
}

std::ostream &operator<<(std::ostream &s, const color::RGB &rgb) {
  return (s << "RGB(" << rgb.r << "," << rgb.g << "," << rgb.b << ")");
}