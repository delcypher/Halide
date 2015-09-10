#include "Halide.h"
#include <iostream>

// By convention, Generators always go in a .cpp file, usually with no
// corresponding .h file. They can be enclosed in any C++ namespaces
// you like, but the anonymous namespace is often the best choice.
//
// It's normally considered Best Practice to have exactly one Generator
// per .cpp file, and to have the .cpp file name match the generator name
// with a "_generator" suffix (e.g., Generator with name "foo" should
// live in "foo_generator.cpp"), as it tends to simplify build rules,
// but neither of these are required.

namespace {

class DemoX86UseHalf : public Halide::Generator<DemoX86UseHalf> {
public:
    ImageParam input_image{ Float(16), 2, "input_image"};
    Param<float> multiplication_factor{ "runtime_factor", 1.5 };

    Func build() {
        Var x, y, c;
        Func upCast("upcast"), process("process"), downCast("downcast");

        // Upcast
        upCast(x, y) = cast<float>( input_image(x, y) );

        // Do simple processing
        process(x, y) = upCast(x , y) * multiplication_factor;

        // Downcast, specifiying rounding
        downCast(x, y) = cast<Halide::float16_t>(process(x, y),
                                                 RoundingMode::ToNearestTiesToEven);

        Target t = get_target();

        if (t.arch == Halide::Target::X86 && t.has_feature(Halide::Target::F16C)) {
            // vcvtph2ps natural width is 8
            std::cout << "Vectorizing!\n";
            downCast.vectorize(x, 8);
        } else {
            std::cout << "Not trying to vectorize!\n";
        }

        return downCast;
    }
};

class DemoX86NoUseHalf : public Halide::Generator<DemoX86NoUseHalf> {
public:
    ImageParam input_image{ Float(32), 2, "input_image"};
    Param<float> multiplication_factor{ "runtime_factor", 1.5 };

    Func build() {
        Var x, y;
        Func process("process");

        // FIXME: This needs to be the same as what DemoX86 does!
        // Do simple processing
        process(x, y) = input_image(x , y) * multiplication_factor;

        return process;
    }
};

class DemoX86DoUpCast : public Halide::Generator<DemoX86DoUpCast> {
public:
    ImageParam input_image{ Float(16), 2, "input_image"};

    Func build() {
        Var x, y, c;
        Func upCast("upcast"), process("process"), downCast("downcast");

        // Upcast
        upCast(x, y) = cast<float>( input_image(x, y) );
        upCast.vectorize(x, 8);
        return upCast;
    }
};

class DemoX86GenerateHalf : public Halide::Generator<DemoX86GenerateHalf> {
public:
    // FIXME: How do I get the input image width
    Param<uint32_t> rowWidth{ "rowWidth", 0 };
    Func build() {
        Var x, y, c;
        Func output;
        output(x, y) = cast<uint16_t>( (x + y*rowWidth) % 0x7bff );

        // FIXME: Need to optimize schedule

        return output;
    }
};

Halide::RegisterGenerator<DemoX86UseHalf> register_example{"demo_x86"};
Halide::RegisterGenerator<DemoX86NoUseHalf> register_example2{"demo_x86_no_half"};
// Used for preparing data
Halide::RegisterGenerator<DemoX86DoUpCast> register_example3{"demo_x86_do_up_cast"};
Halide::RegisterGenerator<DemoX86GenerateHalf> register_example4{"demo_x86_gen_half"};

}  // namespace
