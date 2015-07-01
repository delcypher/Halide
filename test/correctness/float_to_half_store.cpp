#include "Halide.h"
#include "math.h"

using Halide::Image;
int main(int argc, char **argv) {
    Halide::Func stage0("float_half_store");
    Halide::Var x, y, c;
    float scale = 2*M_PI/100;

    Halide::Expr radius = Halide::sqrt(Halide::cast<float>(x) * Halide::cast<float>(x) +
                          Halide::cast<float>(y) * Halide::cast<float>(y));

    // Define the function.
    stage0(x, y, c) = Halide::cast(Halide::Float(16), 255.0f * Halide::abs(Halide::sin(scale * radius)));
    /*Halide::Image<>*/ auto output = stage0.realize(800, 600, 3);
    stage0.compile_to_bitcode("float_to_half_store.bc", {});
    stage0.compile_to_assembly("float_to_half_store.s", {});

    // What should we test?
    printf("Success!\n");
    return 0;
}
