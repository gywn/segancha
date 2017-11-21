#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <cma-es/cmaes.h>
#include <color.h>
#include <fitness.h>
#include <fstream>
#include <lexi.h>

unsigned compress(double x, unsigned max = 256) {
  return std::min((unsigned)std::max(0., x * max), max - 1);
}

int main(int argc, char *argv[]) {
  const size_t M = 7;
  const color::LAB foreground{0, 0., 0.};
  const color::LAB background{100, 0., 0.};
  const auto result = perceptionL(foreground, background, M);
  std::cout << result << std::endl;

  // Write a HTML demo
  const color::RGB frgb(color::LABtoRGB(foreground));
  const color::RGB brgb(color::LABtoRGB(background));
  std::ofstream myfile;
  myfile.open("index.html");
  myfile << "<!DOCTYPE html><html><head><style>body {background-color: rgb("
         << compress(brgb.r) << "," << compress(brgb.g) << ","
         << compress(brgb.b)
         << ");} div {margin: 10px; float: left; "
            "width: 30px; height: 30px;}</style></head><body>";
  myfile << "<div style=\"background-color: rgb(" << compress(frgb.r) << ","
         << compress(frgb.g) << "," << compress(frgb.b) << ");\"></div>";
  for (const auto &rgb : result.rgb) {
    myfile << "<div style=\"background-color: rgb(" << compress(rgb.r) << ","
           << compress(rgb.g) << "," << compress(rgb.b) << ");\"></div>";
  }
  myfile << "</body></html>";
  myfile.close();
  return 0;
}