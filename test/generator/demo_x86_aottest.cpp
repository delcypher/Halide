#include "HalideRuntime.h"

#include <math.h>
#include <stdio.h>

#include "demo_x86_soft.h"
#include "demo_x86_vector.h"
#include "halide_image.h"

using namespace Halide::Tools;

const int kSize = 4096 *10;

int main(int argc, char **argv) {

  // FIXME: Initialise data!
  Image<uint16_t> input(kSize, kSize, 1); // float16 bits
  Image<uint16_t> output(kSize, kSize, 1);

  printf("Starting soft\n");
  demo_x86_soft(input, 1.5, output);
  printf("Finished soft\n");
  printf("Starting vector\n");
  demo_x86_vector(input, 1.5, output);
  printf("Finished vector\n");

  printf("Success!\n");
  return 0;
}
