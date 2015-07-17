#include "HalfFloat.h"
#include "Error.h"

namespace Halide {

  // The static_assert checking the size is to make sure
  // fp16_t can be used as a POD type that is 16-bits wide

  fp16_t::fp16_t(float value, RoundingMode roundingMode) {
    static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
    internal_error << "Not implemented";
  }

  fp16_t::fp16_t(double value, RoundingMode roundingMode) {
    static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
    internal_error << "Not implemented";
  }

  fp16_t::fp16_t() {
    static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
    this->data = 0;
  }

  fp16_t::operator float() {
    internal_error << "Not implemented";
  }
  fp16_t::operator double() {
    internal_error << "Not implemented";
  }
} // namespace halide
