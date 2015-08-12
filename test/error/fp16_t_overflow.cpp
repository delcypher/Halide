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
  // Number is larger than can be represented in half
  // but should be representable in single precision
  float largeNum = 0x1.0p+16;
  h_assert(!std::isnan(largeNum), "largeNum should not be NaN");
  h_assert(!std::isinf(largeNum), "largeNum should not be inf");

  // This should fail as it triggers overflow
  fp16_t fail(largeNum, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);

  // Supress -Wunused-but-set-variable
  fail.isInfinity();

  printf("Should not be reached!\n");
  return 0;
}
