#include "Halide.h"
#include "HalfFloatLiteral.h"
#include <stdio.h>
#include <cmath>

using namespace Halide;

// EURGH: Why aren't we using a unit test framework for this?
void h_assert(bool condition, const char* msg) {
  if (!condition) {
    printf("FAIL: %s\n", msg);
    abort();
  }
}

int main() {
  // Number is subnormal as a float but when converting to
  // half it will not be representable forcing rounding to occur
  float verySmallNum = 0x0.0020p-14;
  h_assert(verySmallNum != 0.0, "smallNum should not be 0");

  // This should trigger an underflow (IEEE754 7.5 Underflow) warning
  //
  // Basically if the result computed as if we had an unbounded exponent but
  // did the rounding would be
  // - subnormal
  // AND
  // - non zero
  // and the actual result returned (distinct from the result described above)
  // is not exact (i.e. rounding occurred when using the finite exponenet)
  // then underflow should be flagged.
  fp16_t warning(verySmallNum, fp16_t::RoundingMode::TONEAREST_TIESTOAWAY);
  h_assert(warning.toBits() == 0x0001, "bits incorrect");

  // Supress GCC's -Wunused-but-set-variable
  warning.isInfinity();
  return 0;
}
