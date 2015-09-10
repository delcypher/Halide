#include "HalideRuntime.h"

#include <math.h>
#include <stdio.h>

#include "demo_x86_soft.h"
#include "demo_x86_vector.h"
#include "demo_x86_no_half.h"
#include "halide_image.h"

// GROSS
#include "emmintrin.h"

using namespace Halide::Tools;

const int kSize = 10000;

int main(int argc, char **argv) {

  float param = 1.5;

  Image<uint16_t> input(kSize, kSize, 1); // float16 bits
  Image<uint16_t> output(kSize, kSize, 1);
  Image<float> inputAsFloat(kSize, kSize, 1);
  Image<float> outputAsFloat(kSize, kSize, 1);

  // FIXME: This is too slow!
  printf("Initialise %ux%u img\n", kSize, kSize);
  // Initialise data
  for (int x=0; x < kSize; ++x) {
    for (int y=0; y < kSize; ++y) {
        // Only positive halfs, no NaN or inf
        uint16_t halfBits = (x + y*kSize) % 0x7bff;
        input(x, y) = halfBits;
        inputAsFloat(x, y) = halide_float16_bits_to_float(halfBits);
    }
  }
  printf("Finished initialising\n");


  // EVIL!
  #define FLUSH do { _mm_clflush(input.data()); _mm_clflush(inputAsFloat.data());} while(0);

  FLUSH

  printf("Starting vector\n");
  demo_x86_vector(input, param, output);
  printf("Finished vector\n");

  FLUSH

  printf("Starting pure float impl\n");
  demo_x86_no_half(inputAsFloat, param, outputAsFloat);
  printf("Finishing pure float impl\n");

  FLUSH

  printf("Starting soft\n");
  demo_x86_soft(input, param, output);
  printf("Finished soft\n");



  printf("Success!\n");
  return 0;
}
