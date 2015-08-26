#ifndef HALIDE_FLOAT16_CAST_H
#define HALIDE_FLOAT16_CAST_H

#include "Expr.h"
#include "RoundingMode.h"

namespace Halide {
namespace Internal {

/** Generate an expression representing the downcast of an expression
 *  of type float32 to a float16 type. This is intended for use in backends
 *  that do not have native support for performing this cast.
 *
 *  \param input. The expression of type float32 to cast.
 *  \param mode. The rounding mode to use.
 */
Expr EXPORT soft_cast_float32_to_float16(Expr input, RoundingMode mode);

/** Generate an expression representing the downcast of an expression
 *  of type float64 to a float16 type. This is intended for use in backends
 *  that do not have native support for performing this cast.
 *
 *  \param input. The expression of type float64 to cast.
 *  \param mode. The rounding mode to use.
 */
Expr EXPORT soft_cast_float64_to_float16(Expr input, RoundingMode mode);

/** Generate an expression representing the upcast of an expression
 *  of type float16 to a float32 type. This is intended for use in backends
 *  that do not have native support for performing this cast.
 *
 *  \param input. The expression of type float16 to cast.
 */
Expr EXPORT soft_cast_float16_to_float32(Expr input);

/** Generate an expression representing the upcast of an expression
 *  of type float16 to a float64 type. This is intended for use in backends
 *  that do not have native support for performing this cast.
 *
 *  \param input. The expression of type float16 to cast.
 */
Expr EXPORT soft_cast_float16_to_float64(Expr input);

}
}
#endif
