#include "Float16Cast.h"

namespace Halide {
namespace Internal {

Expr soft_cast_float32_to_float16(Expr input, RoundingMode mode) {
    internal_assert(input.type() == Float(32)) << "input must have float32 type\n";
    // TODO
    return Expr();
}

Expr soft_cast_float64_to_float16(Expr input, RoundingMode mode) {
    internal_assert(input.type() == Float(64)) << "input must have float64 type\n";
    // TODO
    return Expr();
}

Expr soft_cast_float16_to_float32(Expr input) {
    internal_assert(input.type() == Float(16)) << "input must have float16 type\n";
    // TODO
    return Expr();
}

Expr soft_cast_float16_to_float64(Expr input) {
    internal_assert(input.type() == Float(16)) << "input must have float16 type\n";
    // TODO
    return Expr();
}

}
}
