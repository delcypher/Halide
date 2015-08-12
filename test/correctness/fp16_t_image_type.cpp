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
  Halide::Image<fp16_t> simple(/*x=*/10, /*y=*/3);

  h_assert(sizeof(fp16_t) == 2, "fp16_t has invalid size");

  // Assert some basic properties of the image
  h_assert(simple.extent(0) == 10, "invalid width");
  h_assert(simple.extent(1) == 3, "invalid height");
  h_assert(simple.min(0) == 0, "unexpected non zero min in x");
  h_assert(simple.min(1) == 0, "unexpected non zero min in y");
  h_assert(simple.channels() == 1, "invalid channels");
  h_assert(simple.dimensions() == 2, "invalid number of dimensions");

  // Write a constant value and check we can read it back
  for (int x=simple.min(0); x < simple.extent(0); ++x) {
    for (int y = simple.min(1); y < simple.extent(1); ++y) {
      simple(x, y) = 0.25_fp16;
    }
  }

  for (int x=simple.min(0); x < simple.extent(0); ++x) {
    for (int y = simple.min(1); y < simple.extent(1); ++y) {
      h_assert(simple(x, y) == 0.25_fp16, "Invalid value read back");
      h_assert(simple(x, y).toBits() == 0x3400, "Bit pattern incorrect");
    }
  }

  // Check we can also access via the raw buffer
  buffer_t* rawImage = simple.raw_buffer();
  for (int x=simple.min(0); x < simple.extent(0); ++x) {
    for (int y = simple.min(1); y < simple.extent(1); ++y) {
      uint8_t* loc = rawImage->host +
                     sizeof(fp16_t)*((x - rawImage->min[0])*rawImage->stride[0] +
                                     (y - rawImage->min[1])*rawImage->stride[1]);
      fp16_t* pixel = (fp16_t*) loc;
      h_assert(*pixel == 0.25_fp16, "Failed to read value back via buffer_t");
      h_assert(pixel->toBits() == 0x3400, "Bit pattern incorrect via buffer_t");
    }
  }

  printf("Success!\n");
  return 0;
}
