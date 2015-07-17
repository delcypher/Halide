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
  fp16_t one = 1.0_fp16;
  fp16_t onePointTwoFive = 1.25_fp16;

  // Check the bits are how we expect before using
  // comparision operators
  h_assert(one.toBits() != onePointTwoFive.toBits(), "bits should be different");
  h_assert(one.toBits() == 0x3c00, "bit pattern for 1.0 is wrong");
  h_assert(onePointTwoFive.toBits() == 0x3d00, "bit pattern for 1.25 is wrong");

  // Check comparision operators
  h_assert(!(one == onePointTwoFive), "comparision failed");
  h_assert(one != onePointTwoFive, "comparision failed");
  h_assert(one < onePointTwoFive, "comparision failed");
  h_assert(one <= onePointTwoFive, "comparision failed");
  h_assert(onePointTwoFive > one, "comparision failed");
  h_assert(onePointTwoFive >= one, "comparision failed");
  h_assert(one >= one, "comparision failed");
  h_assert(one == one, "comparision failed");

  // Try with a negative number
  fp16_t minusOne = -one;
  h_assert(minusOne < one, "-1.0 should be < 1.0");
  h_assert(one > minusOne, "1.0 should be > -1.0");

  // NaN never compares equal to itself
  fp16_t nanValue = fp16_t::getNaN();
  h_assert(nanValue.areUnordered(nanValue), "NaN must be unordered");
  h_assert(nanValue != nanValue, "NaN must not compare equal to itself");
  h_assert(!(nanValue == nanValue), "NaN must not compare equal to itself");

  // NaN should be incomparable with normal numbers, zero and inf
  h_assert(nanValue.areUnordered(one), "1.0 and NaN should be unordered");
  h_assert(nanValue.areUnordered(fp16_t::getZero(/*positive=*/true)),
    "+0 and NaN should be unordered");
  h_assert(nanValue.areUnordered(fp16_t::getZero(/*positive=*/false)),
    "-0 and NaN should be unordered");
  h_assert(nanValue.areUnordered(fp16_t::getInfinity(/*positive=*/true)),
    "+inf and NaN should be unordered");
  h_assert(nanValue.areUnordered(fp16_t::getInfinity(/*positive=*/false)),
    "-inf and NaN should be unordered");

  // +ve zero and -ve zero are comparable
  fp16_t zeroP = fp16_t::getZero(/*positive=*/true);
  fp16_t zeroN = fp16_t::getZero(/*positive=*/false);
  h_assert(!zeroP.areUnordered(zeroN), "+0 and -0 should be ordered");
  h_assert(zeroP == zeroN, "+0 and -0 should be treated as equal");

  // Infinities are comparable
  fp16_t infinityP = fp16_t::getInfinity(/*positive=*/true);
  fp16_t infinityN = fp16_t::getInfinity(/*positive=*/false);
  h_assert(!infinityP.areUnordered(infinityN),"Infinities are ordered");
  h_assert(infinityP > infinityN, "inf+ should be > inf-");
  h_assert(infinityN < infinityP, "inf- should be < inf+");
  h_assert(one < infinityP, "1.0 should be < inf+");
  h_assert(minusOne < infinityP, "1.0 should be < inf+");
  h_assert(one > infinityN, "1.0 should be > inf-");
  h_assert(minusOne > infinityN, "-1.0 should be > inf-");

  printf("Success!\n");
  return 0;
}
