#include <iostream>
#include <cassert>
#include <lexi.h>

int main() {
  LexiProduct<int> p1, p2, p2a, p3, p4;
  p1 = std::vector<int>{11, 3, 4};
  p2 = std::vector<int>{11, 3, 7};
  p2a = std::vector<int>{11, 3, 7};
  p3 = std::vector<int>{11, 3, 7, 1};
  p4 = std::vector<int>{11, 2, 7, 1};

  assert(p2 == p2a);
  assert(p1 != p2);
  assert(p1 < p2);
  assert(p1 <= p2);
  assert(p2 > p1);
  assert(p2 >= p1);
  assert(p2 >= p2a);
  assert(p2 <= p2a);
  assert(p2 < p3);
  assert(p3 > p2);
  assert(p2 > p4);

  std::cout << "testlexi: All tests passed" << std::endl;
};