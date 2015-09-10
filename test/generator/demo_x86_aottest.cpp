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

  // The first result should always be thrown away.
  // This will give us 11 usable results which we can take the median from
  int repeats = 12;

  // YAML start
  cout << "# YAML\n";
  cout << "# times: <conv_process_conv_vector> <conv_process_conv_software> <process>\n";
  cout << "#\n";
  cout << "# high_resolution_clock" << endl;
  cout << "# period: " << chrono::high_resolution_clock::period::num;
  cout << " / " << chrono::high_resolution_clock::period::den << " seconds\n";
  cout << "# steady = " << boolalpha << chrono::high_resolution_clock::is_steady << endl << endl;

  for (int kSize=64; kSize < 50000; kSize += 64) {
    // Don't bother initialising these, we just write into them and never read.
    Image<uint16_t> output(kSize, kSize, 1);
    Image<float> outputAsFloat(kSize, kSize, 1);

    // Begin dict
    cout << "- dim_size: " << kSize << "\n";
    cout.flush();

    // Don't share input between stages to try to prevent caching effects


    // Vectorized: conv(float16, float) -> process -> conv(float, float16)
    {
      cout << indent1 << "vector_f16_f32_f16: [";
      Image<uint16_t> input(kSize, kSize, 1); // float16 bits
      Image<float> inputAsFloat(kSize, kSize, 1);
      demo_x86_gen_half(kSize, input); // Init
      for (int repeat=0; repeat < repeats ; ++repeat) {
        fprintf(stderr, "Starting vector\n");
        vStart = std::chrono::high_resolution_clock::now();
        demo_x86_vector(input, param, output);
        vEnd = std::chrono::high_resolution_clock::now();
        fprintf(stderr, "Finished vector\n");


        duration_t vTime = chrono::duration_cast<duration_t>(vEnd - vStart);
        cout << ((repeat!=0)?",":"") << vTime.count();
      }
      cout << "]\n";
      cout.flush();
    }


    // Vectorized: process
    {
      cout << indent1 << "vector_f32: [";
      Image<uint16_t> input(kSize, kSize, 1); // float16 bits
      Image<float> inputAsFloat(kSize, kSize, 1);
      demo_x86_gen_half(kSize, input); // Init
      demo_x86_do_up_cast(input, inputAsFloat); // Init
      for (int repeat=0; repeat < repeats ; ++repeat) {
        fprintf(stderr, "Starting pure float impl\n");
        floatStart = std::chrono::high_resolution_clock::now();
        demo_x86_no_half(inputAsFloat, param, outputAsFloat);
        floatEnd = std::chrono::high_resolution_clock::now();
        fprintf(stderr, "Finishing pure float impl\n");

        duration_t floatTime = chrono::duration_cast<duration_t>(floatEnd - floatStart);
        cout << ((repeat!=0)?",":"") << floatTime.count();
      }
      cout << "]\n";
      cout.flush();
    }

    // Scalar software: conv(float16, float) -> process -> conv(float16, float)
    {
      cout << indent1 << "soft_f16_f32_f16: [";
      Image<uint16_t> input(kSize, kSize, 1); // float16 bits
      demo_x86_gen_half(kSize, input); // Init
      for (int repeat=0; repeat < repeats ; ++repeat) {
        fprintf(stderr, "Starting soft\n");
        sStart = std::chrono::high_resolution_clock::now();
        demo_x86_soft(input, param, output);
        sEnd = std::chrono::high_resolution_clock::now();
        fprintf(stderr, "Finished soft\n");


        duration_t sTime = chrono::duration_cast<duration_t>(sEnd - sStart);
        cout << ((repeat!=0)?",":"") << sTime.count();
      }
      cout << "]\n";
      cout.flush();
    }
    cout << "\n";
  }

  fprintf(stderr, "Success!\n");
  return 0;
}
