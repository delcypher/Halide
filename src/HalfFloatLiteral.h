#ifndef HALIDE_LITERAL_H
#define HALIDE_LITERAL_H
// FIXME: Halide's header file hack that creates a difference between internal
// and external (public) headers prevents us from doing the include below
// #include "HalfFloat.h"
//
// This means clients of this header file will need to have included the right
// header file before including this one. Internally HalFloat.h or a header that
// includes it (Expr.h) can be used and externally Halide.h must be used

// User defined literal for fp16_t This is not in HalfFloat.h because the
// operator is at the global scope so we don't want to pollute the user's
// namespace unless they really want this feature.
Halide::fp16_t operator "" _fp16(const char*);

#endif
