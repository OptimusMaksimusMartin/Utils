#ifndef FLOATCMP_H_
#define FLOATCMP_H_

#include <cmath>
#include <type_traits>
#include <memory>
template <class T>
class Float {
  static_assert(std::is_floating_point<T>::value, "not floating point");

  template <class fltType>
  static bool floatCompareEps(fltType f1, fltType f2) {
    static_assert(std::is_floating_point<T>::value, "not floating point");
    float difAbs = std::abs(f1 - f2);
    if (difAbs <= epsilon_) return true;
    return difAbs <= epsilon_ * std::max(std::abs(f1), std::abs(f2));
  }

  template <class fltType>
  static bool floatGreaterThanEps(fltType a, fltType b) {
    static_assert(std::is_floating_point<T>::value, "not floating point");
    return (a - b) > (std::max(fabsf(a), std::abs(b)) * epsilon_);
  }

  template <class fltType>
  static bool floatLessThanEps(fltType a, fltType b) {
    static_assert(std::is_floating_point<T>::value, "not floating point");
    return (b - a) > (std::max(std::abs(a), std::abs(b)) * epsilon_);
  }

  union Float32ULP {
    Float32ULP(float num = 0.0f) : f(num) {}
    // Portable extraction of components.
    bool Negative() const { return i < 0; }
    int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
    int32_t RawExponent() const { return (i >> 23) & 0xFF; }

    int32_t i;
    float f;
  };

  union Float64ULP {
    Float64ULP(double num = 0.0) : f(num) {}
    // Portable extraction of components.
    bool Negative() const { return i < 0; }
    int64_t RawMantissa() const {
      int64_t mask { (1ll << 52) - 1 };
      return i & mask;
    }
    int64_t RawExponent() const { return (i >> 52) & 0x00000000000000FF; }

    int64_t i;
    double f;
  };


  template <class fltType>
  static bool floatCompareULP(fltType f1, fltType f2, int maxUlpsDiff);

  template <class fltType>
  static bool floatGreaterThanULP(fltType a, fltType b, int maxUlpsDiff);

  template <>
  bool floatCompareULP(float A, float B) {
    Float32ULP uA(A);
    Float32ULP uB(B);

    if (uA.Negative() != uB.Negative()) {
      if (A == B) return true;
      return false;
    }

    int ulpsDiff = std::abs(uA.i - uB.i);

    return (ulpsDiff <= maxUlpsDiff_) ? true : false;

  }

  template <>
  bool floatCompareULP(double A, double B) {
    Float64ULP uA(A);
    Float64ULP uB(B);

    if (uA.Negative() != uB.Negative()) {
      if (A == B) return true;
      return false;
    }

    int ulpsDiff = std::abs(uA.i - uB.i);

    return (ulpsDiff <= maxUlpsDiff_) ? true : false;

  }

  template <>
  bool floatGreaterThanULP(float A, float B) {
    Float32ULP uA(A);
    Float32ULP uB(B);

    if (uA.Negative() && !uB.Negative()) return false;
    if (!uA.Negative() && uB.Negative()) return true;

    int ulpsDiff = std::abs(uA.i - uB.i);

    return (ulpsDiff <= maxUlpsDiff_) ? false : true;

  }

  template <>
  bool floatGreaterThanULP(double A, double B) {
    Float64ULP uA(A);
    Float64ULP uB(B);

    if (uA.Negative() && !uB.Negative()) return false;
    if (!uA.Negative() && uB.Negative()) return true;

    int ulpsDiff = std::abs(uA.i - uB.i);

    return (ulpsDiff <= maxUlpsDiff_) ? false : true;
  }

  template <class fltType>
  static bool floatLessThanULP(fltType A, fltType B, int maxUlpsDiff) {
    return !floatGreaterThanULP(A, B, maxUlpsDiff);
  }

  template <class fltType>
  static bool floatCompare(fltType a, fltType b) {
    return useULP ? floatCompareULP(a, b) : floatGreaterThanEps(a, b);
  }

  template <class fltType>
  static bool floatGreaterThan(fltType a, fltType b) {
    return useULP ? floatGreaterThanULP(a, b) : floatGreaterThanEps(a, b);
  }

  template <class fltType>
  static bool floatLessThan(fltType a, fltType b) {
    return useULP ? floatLessThanULP(a, b) : floatLessThanEps(a, b);
  }


public:
  static T epsilon_ = std::numeric_limits<T>::epsilon();
  static bool useULP;
  static int maxUlpsDiff_;

  constexpr Float(T && value_) : value(value_) {};

  constexpr Float(const Float & rhs) : Float(rhs.value) {};

  constexpr Float(const Float && rhs) : value(std::move(rhs.value)) {}

  Float & operator=(const Float & rhs) {
    value = rhs.value;
    return *this;
  }

  Float & operator=(const Float && rhs) {
    value = rhs.value;
    return *this;
  }

  bool operator==(const Float && rhs) const {
    return (&rhs == this) ? true : floatCompare(value, rhs.value);
  }

  bool operator==(const T && val) const {
    return floatCompare(value, val, epsilon_);
  }

  bool operator<(const Float && rhs) const {
    return (&rhs == this) ? false : floatLessThan(value, rhs.value);
  }

  bool operator<(const T && val) const {
    return floatLessThan(value, val);
  }

  bool operator>=(const Float && rhs) const {
    return !operator<(rhs);
  }

  bool operator>=(const T && val) const {
    return !operator<(val);
  }

  bool operator>(const Float & rhs) const {
    return (&rhs == this) ? false : floatGreaterThan(value, rhs.value);
  }

  bool operator>(const T val) const {
    return floatGreaterThan(value, val);
  }

  bool operator<=(const Float & rhs) const {
    return !operator>(rhs);
  }

  bool operator<=(const T val) const {
    return !operator>(val);
  }

  bool operator!=(const Float & rhs) const {
    return (&rhs == this) ? false : !floatCompare(value, rhs.value);
  }

  bool operator!=(const T val) const {
    return !floatCompareEps(value, val);
  }

  Float operator+(const Float & rhs) const {
    Float tmp { *this };
    tmp.value += rhs.value;
    return tmp;
  }

  Float operator+(const T val) const {
    Float tmp { value };
    tmp.value += val;
    return tmp;
  }

  Float operator-(const Float & rhs) const {
    Float tmp { *this };
    tmp.value -= rhs.value;
    return tmp;
  }

  Float operator-(const T val) const {
    Float tmp { *this };
    tmp.value -= val;
    return tmp;
  }

  Float operator*(const Float & rhs) const {
    Float tmp { *this };
    tmp.value *= rhs.value;
    return tmp;
  }

  Float operator*(const T val) const {
    Float tmp { *this };
    tmp.value *= val;
    return tmp;
  }

  Float operator/(const Float & rhs) const {
    Float tmp { value };
    tmp.value /= rhs.value;
    return tmp;
  }

  Float operator/(const T val) const {
    Float tmp { *this };
    tmp.value /= val;
    return tmp;
  }

  Float operator%(const Float & rhs) const {
    Float tmp { *this };
    tmp.value = std::fmod(value, rhs.value);
    return tmp;
  }

  Float operator%(const T val) const {
    Float tmp { *this };
    tmp.value = std::fmod(value, val);
    return tmp;
  }

  Float operator^(const Float & rhs) {
    Float tmp { *this };
    tmp.value = std::pow(value, rhs.value);
    return tmp;
  }

  Float operator^(const T val) {
    Float tmp { *this };
    tmp.value = std::pow(value, val);
    return tmp;
  }

  Float & operator++() {
    value += T(1.0f);
    return *this;
  }

  Float operator++(int) {
    Float tmp { *this };
    value += T(1.0f);
    return tmp;
  }

  Float & operator--() {
    value -= T(1.0f);
    return *this;
  }

  Float operator--(int) {
    Float tmp { *this };
    value -= T(1.0f);
    return tmp;
  }

  Float & operator+=(const Float & rhs) {
    value += rhs.value;
    return *this;
  }

  Float & operator+=(const T val) {
    value += val;
    return *this;
  }

  Float & operator-=(const Float & rhs) {
    value -= rhs.value;
    return *this;
  }

  Float & operator-=(const T val) {
    value -= val;
    return *this;
  }

  Float & operator*=(const Float & rhs) {
    value *= rhs.value;
    return *this;
  }

  Float & operator*=(const T val) {
    value *= val;
    return *this;
  }

  Float & operator/=(const Float & rhs) {
    value /= rhs.value;
    return *this;
  }

  Float & operator/=(const T val) {
    value /= val;
    return *this;
  }

  Float & operator%=(const Float & rhs) {
    value = std::fmod(value, rhs.value);
    return *this;
  }

  Float & operator%=(const T val) {
    value = std::fmod(value, val);
    return *this;
  }

  Float & operator^=(const Float & rhs) {
    Float tmp { value };
    tmp.value = std::pow(value, rhs.value);
    return *this;
  }

  Float & operator^=(const T val) {
    value = std::pow(value, val);
    return *this;
  }

  T & operator()()  {
     return value;
  }

private:
  T value;
};

template <class T>
bool operator==(const T f, const Float<T> &s) {
  return s == f;
}

template <class T>
bool operator==(const uint32_t f, const Float<T> &s) {
  return s == static_cast<T>(f);
}

template <class T>
bool Float<T>::useULP = false;

template <class T>
static int maxUlpsDiff = 1;

using float32_t = Float<float>;
using float64_t = Float<double>;

#include <vector>

template <class floatType>
floatType fast_two_sum (floatType &&t, floatType &&a, double b) {
  floatType s = a+b;
  t = b-(s-a);
  return s;
}

template <class floatType>
floatType Kahan (const std::vector<floatType> &&X) {
  double s=0.0, c=0.0;
  for (auto x: X) {
    double y = x + c;
    s = fast_two_sum (c, s, y);
  }
  return s;
}

template <class floatType>
floatType RumpOgitaOishi (const std::vector<floatType> &&X) {
  floatType s=0.0, c=0.0;
  for (auto x: X) {
    floatType y = x + c;
    s = fast_two_sum (c, s, y);
  }
  return s;
}

#endif
