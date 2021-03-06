/*
 * testcolor.cpp
 * Part of http://github.com/gfiumara/color by Gregory Fiumara.
 * See LICENSE for details.
 */

#include <cmath>
#include <color.h>
#include <iostream>
#include <string>
#include <vector>

static size_t line = 0;
static std::vector<bool> passFail;

void check(bool res, std::string msg) {
  passFail.push_back(res);
  std::cout << line << ": " << msg << '\t' << (passFail[line] ? "PASS" : "FAIL")
            << std::endl;
  ++line;
}

void check(double a, double b, const std::string &msg, double delta = 1e-3) {
  // std::cout << std::setprecision(10) << a << " " << b << std::endl;
  check(std::abs(a - b) < delta, msg);
}

/**
 * @brief
 * Run the test dataset run from the color paper.
 *
 * @return
 * EXIT_SUCCESS if all tests pass, EXIT_FAILURE otherwise.
 */
int testcolor();

int main(int argc, char *argv[]) { return (testcolor()); }

int testcolor() {
  unsigned int line = 1;
  std::vector<bool> passFail;

  color::LAB lab1 = {100, 0, 0};
  color::RGB rgb1 = color::LABtoRGB(lab1);
  color::RGB rgb_expected = {1, 1, 1};

  check(std::abs(rgb1.r - rgb_expected.r) < 1e-3, "LAB->RGB");
  check(std::abs(rgb1.g - rgb_expected.g) < 1e-3, "LAB->RGB");
  check(std::abs(rgb1.b - rgb_expected.b) < 1e-3, "LAB->RGB");

  // std::cout << color::LABtoRGB(color::LAB{50, 0, 0}) << std::endl;

  color::RGB rgb2 = color::LABtoRGB(color::LAB{50, 1000, 0});
  check(rgb2.r > 1, "RGB limit");
  check(rgb2.g < 0, "RGB limit");
  check(rgb2.b < 0, "RGB limit");

  color::RGB rgb3 = color::LABtoRGB(color::LAB{50, -1000, 0});
  check(rgb3.r < 0, "RGB limit");

  color::RGB rgb4 = color::LABtoRGB(color::LAB{50, 0, 1000});
  check(rgb4.b < 0, "RGB limit");

  color::RGB rgb5 = color::LABtoRGB(color::LAB{50, 0, -1000});
  check(rgb5.r < 0, "RGB limit");
  check(rgb5.g < 0, "RGB limit");
  check(rgb5.b > 1, "RGB limit");

  color::LAB lab3{50, 10, 20};
  auto lab3a = XYZtoLAB(LABtoXYZ(lab3));
  check(lab3a.l, lab3.l, "LAB->XYZ->LAB");
  check(lab3a.a, lab3.a, "LAB->XYZ->LAB");
  check(lab3a.b, lab3.b, "LAB->XYZ->LAB");

  check(color::LABtoLCH(color::LAB{50, 20, 0}).h, 0, "LCH hue");
  check(color::LABtoLCH(color::LAB{50, 0, 20}).h, 90, "LCH hue");
  check(color::LABtoLCH(color::LAB{50, -20, 0}).h, 180, "LCH hue");
  check(color::LABtoLCH(color::LAB{50, 0, -20}).h, 270, "LCH hue");

  check(color::LCHtoLAB(color::LCH{50, 20, 45}).a, 20. / sqrt(2), "LCH -> LAB");
  check(color::LCHtoLAB(color::LCH{50, 20, 45}).b, 20. / sqrt(2), "LCH -> LAB");
  check(color::LCHtoLAB(color::LCH{50, 20, 225}).a, -20. / sqrt(2),
        "LCH -> LAB");
  check(color::LCHtoLAB(color::LCH{50, 20, 225}).b, -20. / sqrt(2),
        "LCH -> LAB");

  auto d65 = color::IlluminantDKelvin(6500, 1);
  check(d65.x, 0.95047, "D65");
  check(d65.y, 1, "D65");
  check(d65.z, 1.08833, "D65");

  std::cout << std::endl;
  int ret = EXIT_SUCCESS;
  for (unsigned int i = 0; i < passFail.size(); i++) {
    if (!passFail[i]) {
      std::cout << "Test failed on line " << std::to_string(i + 1) << std::endl;
      ret = EXIT_FAILURE;
    }
  }

  return (ret);
}
