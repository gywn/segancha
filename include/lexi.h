#ifndef LEXI_PRODUCT_H_
#define LEXI_PRODUCT_H_

#include <algorithm>
#include <ostream>
#include <vector>

/*
support all compaison
support minus
LexiProduct(0) <= any lexi <= LexiProduct(std::numeric_limits<Scalar>::max())
*/
template <typename Scalar> class LexiProduct {
  using Product = std::vector<Scalar>;

public:
  LexiProduct() : LexiProduct(0) {}
  LexiProduct(Scalar val) : prd{val} {}
  LexiProduct(const Product &prd2) : prd(prd2) {}
  LexiProduct(Product &&prd2) : prd(std::move(prd2)) {}

  LexiProduct &operator=(const Scalar value) {
    prd = std::vector<Scalar>{value};
    return *this;
  }
  LexiProduct &operator=(const Product &prd2) {
    prd = prd2;
    return *this;
  }
  LexiProduct &operator=(Product &&prd2) {
    prd = std::move(prd2);
    return *this;
  }

  bool operator==(const LexiProduct<Scalar> &lexi2) const {
    return lexicmp(*this, lexi2) == 0;
  }
  bool operator!=(const LexiProduct<Scalar> &lexi2) const {
    return lexicmp(*this, lexi2) != 0;
  }
  bool operator<(const LexiProduct<Scalar> &lexi2) const {
    return lexicmp(*this, lexi2) < 0;
  }
  bool operator<=(const LexiProduct<Scalar> &lexi2) const {
    return lexicmp(*this, lexi2) <= 0;
  }
  bool operator>(const LexiProduct<Scalar> &lexi2) const {
    return lexicmp(*this, lexi2) > 0;
  }
  bool operator>=(const LexiProduct<Scalar> &lexi2) const {
    return lexicmp(*this, lexi2) >= 0;
  }

  LexiProduct<Scalar> operator-(const LexiProduct<Scalar> &lexi2) const {
    const size_t n = std::min(this->prd.size(), lexi2.prd.size());
    Product d(n);
    for (size_t i = 0; i < n; ++i) {
      d[i] = this->prd[i] - lexi2.prd[i];
    }
    return LexiProduct<Scalar>(std::move(d));
  }

  Scalar const &operator[](size_t i) const { return this->prd[i]; }

  Scalar &operator[](size_t i) { return this->prd[i]; }

private:
  Product prd;

  inline Scalar lexicmp(const LexiProduct<Scalar> &lhs,
                        const LexiProduct<Scalar> &rhs) const {
    const Scalar lencmp = (Scalar)(lhs.prd.size() - rhs.prd.size());
    const size_t n = (lencmp < 0 ? lhs : rhs).prd.size();
    for (size_t i = 0; i < n; ++i) {
      const Scalar d = lhs.prd[i] - rhs.prd[i];
      if (d != 0) {
        return d;
      }
    }
    return lencmp;
  }

  friend std::ostream &operator<<(std::ostream &os, const LexiProduct &lexi) {
    const size_t n = lexi.prd.size();
    os << "[";
    for (size_t i = 0; i < n - 1; ++i) {
      os << lexi.prd[i] << ",";
    }
    os << lexi.prd[n - 1] << "]";
    return os;
  }
};

#endif