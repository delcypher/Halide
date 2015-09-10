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

class DemoX86 : public Halide::Generator<DemoX86> {
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

// If you're only using a Generator with the JIT, you don't need to register it;
// however, registering it is needed for working seamlessly with the ahead-of-time
// compilation tools, so it's generally recommended to always register it.
// (As with Params, the name is constrained to C-like patterns.)
Halide::RegisterGenerator<DemoX86> register_example{"demo_x86"};

}  // namespace
