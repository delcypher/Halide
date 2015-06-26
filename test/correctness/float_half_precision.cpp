#include "Halide.h"
using Halide::Image;
//#include "image_io.h"
#include "math.h"


int main(int argc, char **argv) {
    Halide::Func stage0, stage1, stage2;

    Halide::Var x, y, c;

    float scale = 2*M_PI/100;

    // For each pixel of the input image.
    Halide::Expr radius = Halide::sqrt(Halide::cast<float>(x) * Halide::cast<float>(x) +
                          Halide::cast<float>(y) * Halide::cast<float>(y));
    stage0(x, y, c) = 255.0f * Halide::abs(Halide::sin(scale * radius));

    Halide::Type fphalf_t = Halide::Float(16);

    // No warning about
    stage1(x, y, c) = Halide::cast(fphalf_t, stage0(x, y, c)) * 0.5f;
    stage2(x, y, c) = Halide::cast<uint8_t>(Halide::cast<float>(stage1(x, y, c)) + 0.5f);

    Halide::Image<uint8_t> output = stage2.realize(800, 600, 3);

    //save(output, "circles1.png");

    printf("Success!\n");
    return 0;
}
