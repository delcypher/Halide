#include "HalideRuntime.h"

#include <math.h>
#include <stdio.h>

#include "demo_x86_soft.h"
#include "demo_x86_vector.h"
#include "demo_x86_no_half.h"
#include "halide_image.h"
#include <chrono>
#include <iostream>

// GROSS
#include "emmintrin.h"

using namespace Halide::Tools;

const int kSize = 10000;

int main(int argc, char **argv) {

  typedef std::chrono::duration<double> duration_t;
  std::chrono::time_point<std::chrono::high_resolution_clock> vStart, vEnd, sStart, sEnd,
    floatStart, floatEnd, initStart, initEnd;
  float param = 1.5;

  Image<uint16_t> input(kSize, kSize, 1); // float16 bits
  Image<uint16_t> output(kSize, kSize, 1);
  Image<float> inputAsFloat(kSize, kSize, 1);
  Image<float> outputAsFloat(kSize, kSize, 1);

  // FIXME: This is too slow!
  printf("Initialise %ux%u img\n", kSize, kSize);
  initStart = std::chrono::high_resolution_clock::now();
  // Initialise data
  for (int x=0; x < kSize; ++x) {
    for (int y=0; y < kSize; ++y) {
        // Only positive halfs, no NaN or inf
        uint16_t halfBits = (x + y*kSize) % 0x7bff;
        input(x, y) = halfBits;
        inputAsFloat(x, y) = halide_float16_bits_to_float(halfBits);
    }
  }
  initEnd = std::chrono::high_resolution_clock::now();
  printf("Finished initialising\n");

  // EVIL!
  #define FLUSH do { _mm_clflush(input.data()); _mm_clflush(inputAsFloat.data());} while(0);

  FLUSH

  printf("Starting vector\n");
  vStart = std::chrono::high_resolution_clock::now();
  demo_x86_vector(input, param, output);
  vEnd = std::chrono::high_resolution_clock::now();
  printf("Finished vector\n");

  FLUSH

  printf("Starting pure float impl\n");
  sStart = std::chrono::high_resolution_clock::now();
  demo_x86_no_half(inputAsFloat, param, outputAsFloat);
  sEnd = std::chrono::high_resolution_clock::now();
  printf("Finishing pure float impl\n");

  FLUSH

  printf("Starting soft\n");
  floatStart = std::chrono::high_resolution_clock::now();
  demo_x86_soft(input, param, output);
  floatEnd = std::chrono::high_resolution_clock::now();
  printf("Finished soft\n");

  duration_t vTime = vEnd - vStart;
  duration_t sTime = sEnd - sStart;
  duration_t floatTime = floatEnd - floatStart;
  duration_t initTime = initEnd - initStart;

  std::cout << "init time:" << initTime.count() << " secs\n";
  std::cout << "vector time:" << vTime.count() << " secs\n";
  std::cout << "soft time:" << sTime.count() << " secs\n";
  std::cout << "float (no half) time:" << floatTime.count() << " secs\n";

  printf("Success!\n");
  return 0;
}
