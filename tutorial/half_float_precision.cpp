#include "Halide.h"
using Halide::Image;
#include "image_io.h"
#include "math.h"


int main(int argc, char **argv) {
    // Next we define our Func object that represents our one pipeline
    // stage.
    Halide::Func stage0, stage1, stage2;

    // Our Func will have three arguments, representing the position
    // in the image and the color channel. Halide treats color
    // channels as an extra dimension of the image.
    Halide::Var x, y, c;

    // Normally we'd probably write the whole function definition on
    // one line. Here we'll break it apart so we can explain what
    // we're doing at every step.
    
    float scale = 2*M_PI/100;

    // For each pixel of the input image.
    Halide::Expr radius = Halide::sqrt(Halide::cast<float>(x) * Halide::cast<float>(x) +
                          Halide::cast<float>(y) * Halide::cast<float>(y));
                        
    // Cast it back to an 8-bit unsigned integer.
    //value = Halide::cast<uint8_t>(value);

    // Define the function.
    //stage0(x, y, c) = Halide::cast<uint8_t>(Halide::max(
    //    255.0f * Halide::sin(scale * radius), 0.0f));
    stage0(x, y, c) = 255.0f * Halide::abs(Halide::sin(scale * radius));

    Halide::Type fphalf_t = Halide::Float(16);

    // No warning about
    stage1(x, y, c) = Halide::cast(fphalf_t, stage0(x, y, c)) * 0.5f;
    stage2(x, y, c) = Halide::cast<uint8_t>(Halide::cast<float>(stage1(x, y, c)) + 0.5f);

    Halide::Image<uint8_t> output = stage2.realize(800, 600, 3);

    save(output, "circles1.png");

    printf("Success!\n");
    return 0;
}
