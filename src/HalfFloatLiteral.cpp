#include "HalfFloat.h"
#include "HalfFloatLiteral.h"

Halide::fp16_t operator "" _fp16(const char* stringRepr) {
  // Note we will never get a string starting with "-".
  return Halide::fp16_t(stringRepr,
    Halide::fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
}
