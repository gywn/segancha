#include <vector>

template <typename T> class LexiProduct {
  using Product = std::vector<T>;

public:
  LexiProduct() : LexiProduct(0) {}
  LexiProduct(size_t size) : prd(size) {}
  LexiProduct &operator=(const Product &prd2) {
    prd = prd2;
    return *this;
  }
  LexiProduct &operator=(Product &&prd2) {
    prd = std::move(prd2);
    return *this;
  }

  bool operator==(const LexiProduct<T> &lexi2) {
    return lexicmp(*this, lexi2) == 0;
  }
  bool operator!=(const LexiProduct<T> &lexi2) {
    return lexicmp(*this, lexi2) != 0;
  }
  bool operator<(const LexiProduct<T> &lexi2) {
    return lexicmp(*this, lexi2) < 0;
  }
  bool operator<=(const LexiProduct<T> &lexi2) {
    return lexicmp(*this, lexi2) <= 0;
  }
  bool operator>(const LexiProduct<T> &lexi2) {
    return lexicmp(*this, lexi2) > 0;
  }
  bool operator>=(const LexiProduct<T> &lexi2) {
    return lexicmp(*this, lexi2) >= 0;
  }

private:
  Product prd;

  int lexicmp(const LexiProduct<T> &lhs, const LexiProduct<T> &rhs) {
    const int lencmp = lhs.prd.size() - rhs.prd.size();
    const size_t n = (lencmp < 0 ? lhs : rhs).prd.size();
    for (size_t i = 0; i < n; ++i) {
      const T d = lhs.prd[i] - rhs.prd[i];
      if (d != 0) {
        return d;
      }
    }
    return lencmp;
  }
};
