#include <cmath>
#include <color.h>
#include <fitness.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

static size_t line = 0;
static std::vector<bool> passFail;

void check(bool res, const std::string &msg) {
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

  check(offRGB(color::LAB{200, 0, 0}), 2.147648383, "offRGB");

  check(fitnessFunc(std::vector<color::LAB>{{50, 0, 0},
                                            {50, 10, 10},
                                            {50, -10, 10},
                                            {50, -10, -10},
                                            {50, 10, -10}})[0],
        -12.8001, "fitnessFunc");

  check(fitnessFunc(std::vector<color::LAB>{
            {50, -200, -200}, {50, 200, 200}, {50, 0, 0}, {50, 0, 0}})[0],
        3337.715201, "fitnessFunc (off boundary)");

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
