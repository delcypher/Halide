#ifndef HALIDE_HALF_FLOAT_H
#define HALIDE_HALF_FLOAT_H
#include <stdint.h>

namespace Halide {

struct fp16_t {
  // Rounding modes
  enum class RoundingMode {
    TOWARDZERO,
    TONEAREST, // Uses "to nearest even" as tie-break
    UPWARD, // toward +infinity
    DOWNWARD // toward -infinity
  };
  // The raw bits.
  // This must be the **ONLY** data member so that
  // this data type is 16-bits wide.
  uint16_t data;

  // NOTE: Do not use virtual methods here
  // it will change the size of this data type.
  fp16_t(float value, RoundingMode roundingMode);
  fp16_t(double value, RoundingMode roundingMode);
  fp16_t();
  operator float();
  operator double();
};
} // namespace Halide
#endif
