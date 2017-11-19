#include <iostream>
#include <lexi.h>
#include <string>

void check(bool b, const std::string &msg) {
  if (!b) {
    std::cout << msg << std::endl;
  }
}

int main() {
  LexiProduct<int> p1, p2, dp, p2a, p3, p4;
  p1 = std::vector<int>{11, 3, 4};
  p2 = std::vector<int>{11, 3, 7};
  dp = std::vector<int>{11, 3, 7};
  p2a = std::vector<int>{11, 3, 7};
  p3 = std::vector<int>{11, 3, 7, 1};
  p4 = std::vector<int>{11, 2, 7, 1};

  check(p2 == p2a, "p2 == p2a not passed");
  check(p1 != p2, "p1 != p2 not passed");
  check(p1 < p2, "p1 < p2 not passed");
  check(p1 <= p2, "p1 <= p2 not passed");
  check(p2 > p1, "p2 > p1 not passed");
  check(p2 >= p1, "p2 >= p1 not passed");
  check(p2 >= p2a, "p2 >= p2a not passed");
  check(p2 <= p2a, "p2 <= p2a not passed");
  check(p2 < p3, "p2 < p3 not passed");
  check(p3 > p2, "p3 > p2 not passed");
  check(p2 > p4, "p2 > p4 not passed");
  check(p1 - p2 <
            LexiProduct<int>(std::vector<int>{std::numeric_limits<int>::max()}),
        "p1 - p2 < [max] not passed");

  LexiProduct<double> f1, f2;
  f1 = std::vector<double>{3.5};
  f2 = std::vector<double>{3.8};

  check(f1 < f2, "f1 < f2 not passed");
};