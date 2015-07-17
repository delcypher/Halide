#ifndef HALIDE_HALF_FLOAT_H
#define HALIDE_HALF_FLOAT_H
#include <stdint.h>
#include <string>

namespace Halide {

struct fp16_t {
  // Rounding modes
  enum class RoundingMode {
    TOWARDZERO,
    TONEAREST_TIESTOEVEN,
    UPWARD, // toward +infinity
    DOWNWARD, // toward -infinity
    TONEAREST_TIESTOAWAY
  };

  // NOTE: Do not use virtual methods here
  // it will change the size of this data type.
  fp16_t(float value, RoundingMode roundingMode);
  fp16_t(double value, RoundingMode roundingMode);
  // StringRepr may be a decimal string or a C99 float hex literal
  fp16_t(const char* stringRepr, RoundingMode roundingMode);
  fp16_t(); // Zero initialised
  fp16_t(uint16_t rawBits);
  operator float();
  operator double();
  // Do not allow implicit or explicit casting from a float or double. It allows
  // to confusing behavior where fp16_t(uint16_t) gets called.  Clients should
  // use the constructor that specifies the rounding mode instead.
  fp16_t(float value) = delete;
  fp16_t(double value) = delete;

  // Static constructors
  static fp16_t getZero(bool positive);
  static fp16_t getInfinity(bool positive);
  static fp16_t getNaN();

  // Basic operators. Currently exceptions are ignored
  fp16_t add(fp16_t rhs, RoundingMode roundingMode) const;
  fp16_t subtract(fp16_t rhs, RoundingMode roundingMode) const;
  fp16_t multiply(fp16_t rhs, RoundingMode roundingMode) const;
  fp16_t divide(fp16_t denominator, RoundingMode roundingMode) const;
  fp16_t remainder(fp16_t denominator) const;
  fp16_t mod(fp16_t denominator, RoundingMode roudingMode) const;
  fp16_t operator-() const;
  // Overloaded operators. These assume TONEAREST_TIESTOEVEN rounding
  fp16_t operator+(fp16_t rhs) const;
  fp16_t operator-(fp16_t rhs) const;
  fp16_t operator*(fp16_t rhs) const;
  fp16_t operator/(fp16_t rhs) const;

  // Comparison
  bool operator==(fp16_t rhs) const;
  bool operator!=(fp16_t rhs) const {return !(*this == rhs);}
  bool operator>(fp16_t rhs) const;
  bool operator<(fp16_t rhs) const;
  bool operator>=(fp16_t rhs) const {return (*this > rhs) || (*this == rhs);}
  bool operator<=(fp16_t rhs) const {return (*this < rhs) || (*this == rhs);}
  bool areUnordered(fp16_t rhs) const;

  // String output
  std::string toHexString() const;
  // significantDigits of 0 uses enough precision such that
  // fp16_t x(...); // some arbitary fp16_t
  // fp16_t z(x.toDecimalString, <ANY ROUNDING MODE>);
  // x.data == z.data
  std::string toDecimalString(unsigned int significantDigits=0) const;

  // Properties
  bool isNaN() const;
  bool isInfinity() const;
  bool isNegative() const;
  bool isZero() const;

  // The raw bits.
  // This must be the **ONLY** data member so that
  // this data type is 16-bits wide.
  uint16_t toBits() const;
 private:
  uint16_t data;
};
} // namespace Halide
#endif
