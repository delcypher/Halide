#include "HalideRuntime.h"

#include <math.h>
#include <stdio.h>

#include "demo_x86_soft.h"
#include "demo_x86_vector.h"
#include "demo_x86_no_half.h"
#include "demo_x86_do_up_cast.h" // For preparing data
#include "demo_x86_gen_half.h" // For preparing data
#include "halide_image.h"
#include <chrono>
#include <iostream>

// GROSS
#include "emmintrin.h"

using namespace Halide::Tools;
using namespace std;

const char* indent1 = "  ";
const char* indent2 = "    ";
const char* indent3 = "      ";
const char* indent4 = "        ";

int main(int argc, char **argv) {

  //typedef std::chrono::duration<double> duration_t;
  typedef std::chrono::microseconds duration_t;
  std::chrono::time_point<std::chrono::high_resolution_clock> vStart, vEnd, sStart, sEnd,
    floatStart, floatEnd, initStart, initEnd;
  float param = 1.5;

  int repeats = 10;

  // YAML start
  cout << "# YAML\n";
  cout << "# times: <conv_process_conv_vector> <conv_process_conv_software> <process>\n";
  cout << "#\n";
  cout << "# high_resolution_clock" << endl;
  cout << "# period: " << chrono::high_resolution_clock::period::num;
  cout << " / " << chrono::high_resolution_clock::period::den << " seconds\n";
  cout << "# steady = " << boolalpha << chrono::high_resolution_clock::is_steady << endl << endl;

  for (int kSize=64; kSize < 50000; kSize += 64) {
    Image<uint16_t> input(kSize, kSize, 1); // float16 bits
    Image<uint16_t> output(kSize, kSize, 1);
    Image<float> inputAsFloat(kSize, kSize, 1);
    Image<float> outputAsFloat(kSize, kSize, 1);

    // FIXME: This is too slow!
    fprintf(stderr, "Initialise %ux%u img\n", kSize, kSize);
    initStart = std::chrono::high_resolution_clock::now();
    // Use Halide to initialise the halfs
    demo_x86_gen_half(kSize, input);
    /*
    for (int x=0; x < kSize; ++x) {
      for (int y=0; y < kSize; ++y) {
          // Only positive halfs, no NaN or inf
          uint16_t halfBits = (x + y*kSize) % 0x7bff;
          if (halfBits != input(x, y)) {
              std::cerr << "mismatch\n";
          }
      }
    } */
    // Use Halide to initialise the float data from the halfs
    demo_x86_do_up_cast(input, inputAsFloat);

    initEnd = std::chrono::high_resolution_clock::now();
    duration_t initTime = chrono::duration_cast<duration_t>(initEnd - initStart);
    fprintf(stderr, "Finished initialising\n");

    // Begin dict
    cout << "- init_time: " << initTime.count() << "\n";
    cout << indent1 << "dim_size: " << kSize << "\n";
    cout << indent1 << "results:\n";
    cout.flush();

    for (int repeat=0; repeat < repeats ; ++repeat) {

      // EVIL!
      #define FLUSH do { \
                         _mm_mfence(); \
                         _mm_clflush(input.data()); \
                         _mm_clflush(inputAsFloat.data()); \
                    } while(0);
      FLUSH

      fprintf(stderr, "Starting vector\n");
      vStart = std::chrono::high_resolution_clock::now();
      demo_x86_vector(input, param, output);
      vEnd = std::chrono::high_resolution_clock::now();
      fprintf(stderr, "Finished vector\n");

      FLUSH

      fprintf(stderr, "Starting pure float impl\n");
      floatStart = std::chrono::high_resolution_clock::now();
      demo_x86_no_half(inputAsFloat, param, outputAsFloat);
      floatEnd = std::chrono::high_resolution_clock::now();
      fprintf(stderr, "Finishing pure float impl\n");

      FLUSH

      fprintf(stderr, "Starting soft\n");
      sStart = std::chrono::high_resolution_clock::now();
      demo_x86_soft(input, param, output);
      sEnd = std::chrono::high_resolution_clock::now();
      fprintf(stderr, "Finished soft\n");

      duration_t vTime = chrono::duration_cast<duration_t>(vEnd - vStart);
      duration_t sTime = chrono::duration_cast<duration_t>(sEnd - sStart);
      duration_t floatTime = chrono::duration_cast<duration_t>(floatEnd - floatStart);

      cout << indent2 << "- [" << vTime.count() << ", "
                             << sTime.count() << ", "
                             << floatTime.count() <<
                         "]\n";
      cout.flush();
      FLUSH
    }
    cout << "\n";
  }

  fprintf(stderr, "Success!\n");
  return 0;
}
