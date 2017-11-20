#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <cma-es/cmaes.h>
#include <color.h>
#include <fitness.h>
#include <lexi.h>

int main(int argc, char *argv[]) {
  const auto pair = perceptionL(50., 7);

  for (size_t i = 0; i < pair.first.size(); ++i) {
    std::cout << pair.first[i] << " ";
  }
  std::cout << std::endl << pair.second << std::endl;
  return 0;
}