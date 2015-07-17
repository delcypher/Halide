#include "Halide.h"
#include <stdio.h>
#include <cmath>

using namespace Halide;

// EURGH: Why aren't we using a unit test framework for this?
void h_assert(bool condition, const char* msg) {
  if (!condition) {
    printf("FAIL: %s\n", msg);
    abort();
  }
}

int main() {
  // Special constants

  // +ve Zero
  {
    // Try constructing +ve zero in different ways and check they all represent
    // the same fp16_t
    fp16_t zeroDefaultConstructor;
    fp16_t zeroP = fp16_t::getZero(/*positive=*/true);
    fp16_t zeroPStringConstructorDecimal("0.0", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t zeroPStringConstructorHex("0x0p0", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t zeroPFromFloat(0.0f, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t zeroPFromDouble(0.0,fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(zeroDefaultConstructor.toBits() == zeroP.toBits(), "Mismatch between constructors");
    h_assert(zeroPStringConstructorDecimal.toBits() == zeroP.toBits(), "Mismatch between constructors");
    h_assert(zeroPStringConstructorHex.toBits() == zeroP.toBits(), "Mismatch between constructors");
    h_assert(zeroPFromFloat.toBits() == zeroP.toBits(), "Mistmatch between constructors");
    h_assert(zeroPFromDouble.toBits() == zeroP.toBits(), "Mistmatch between constructors");

    // Check the representation
    h_assert(zeroP.isZero() && !zeroP.isNegative(), "+ve zero invalid");
    h_assert(zeroP.toBits() == 0x0000, "+ve zero invalid bits");
    h_assert(zeroP.toHexString() == "0x0p0", "+ve zero hex string invalid");
    h_assert(zeroP.toDecimalString(0) == "0.0E+0", "+ve zero decimal string invalid");

    // Try converting to native float types
    h_assert( ((float) zeroP) == 0.0f, "+ve zero conversion to float invalid");
    h_assert( ((double) zeroP) == 0.0, "+ve zero conversion to double invalid");
  }

  // -ve Zero
  {
    // Try constructing -ve zero in different ways and check they all represent
    // the same fp16_t
    fp16_t zeroN = fp16_t::getZero(/*positive=*/false);
    fp16_t zeroNStringConstructorDecimal("-0.0", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t zeroNStringConstructorHex("-0x0p0", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t zeroNFromFloat(-0.0f, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t zeroNFromDouble(-0.0, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(zeroNStringConstructorDecimal.toBits() == zeroN.toBits(), "Mismatch between constructors");
    h_assert(zeroNStringConstructorHex.toBits() == zeroN.toBits(), "Mismatch between constructors");
    h_assert(zeroNFromFloat.toBits() == zeroN.toBits(), "Mismatch between constructors");
    h_assert(zeroNFromDouble.toBits() == zeroN.toBits(), "Mismatch between constructors");

    // Check the representation
    h_assert(zeroN.isZero() && zeroN.isNegative(), "-ve zero invalid");
    h_assert(zeroN.toBits() == 0x8000, "-ve zero invalid bits");
    h_assert(zeroN.toHexString() == "-0x0p0", "-ve zero hex string invalid");
    h_assert(zeroN.toDecimalString(0) == "-0.0E+0", "-ve zero decimal string invalid");

    // Try converting to native float types
    h_assert( ((float) zeroN) == -0.0f, "-ve zero conversion to float invalid");
    h_assert( ((double) zeroN) == -0.0, "-ve zero conversion to double invalid");
  }

  // +ve infinity
  {
    // Try constructing +ve infinity in different ways and check they all
    // represent the same fp16_t
    fp16_t infinityP = fp16_t::getInfinity(/*positive=*/true);
    fp16_t infinityPFromFloat( (float) INFINITY, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t infinityPFromDouble( (double) INFINITY, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(infinityPFromFloat.toBits() == infinityP.toBits(), "Mismatch between constructors");
    h_assert(infinityPFromDouble.toBits() == infinityP.toBits(), "Mismatch between constructors");

    // Check the representation
    h_assert(infinityP.isInfinity() && !infinityP.isNegative(), "+ve infinity invalid");
    h_assert(infinityP.toBits() == 0x7c00, "+ve infinity invalid bits");
    h_assert(infinityP.toHexString() == "infinity", "+ve infinity hex string invalid");
    h_assert(infinityP.toDecimalString() == "+Inf", "+ve infinity decimal string invalid");

    // Try converting to native float types
    float infinityPf = (float) infinityP;
    double infinityPd = (double) infinityP;
    h_assert(std::isinf(infinityPf) & !std::signbit(infinityPf),
             "+ve infinity conversion to float invalid");
    h_assert(std::isinf(infinityPd) & !std::signbit(infinityPd),
             "+ve infinity conversion to double invalid");
  }

  // -ve infinity
  {
    // Try constructing -ve infinity in different ways and check they all
    // represent the same fp16_t
    fp16_t infinityN = fp16_t::getInfinity(/*positive=*/false);
    fp16_t infinityNFromFloat( (float) -INFINITY, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t infinityNFromDouble( (double) -INFINITY, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(infinityNFromFloat.toBits() == infinityN.toBits(), "Mismatch between constructors");
    h_assert(infinityNFromDouble.toBits() == infinityN.toBits(), "Mismatch between constructors");

    // Check the representation
    h_assert(infinityN.isInfinity() && infinityN.isNegative(), "-ve infinity invalid");
    h_assert(infinityN.toBits() == 0xfc00, "-ve infinity invalid bits");
    h_assert(infinityN.toHexString() == "-infinity", "-ve infinity hex string invalid");
    h_assert(infinityN.toDecimalString() == "-Inf", "-ve infinity decimal string invalid");

    // Try converting to native float types
    float infinityNf = (float) infinityN;
    double infinityNd = (double) infinityN;
    h_assert(std::isinf(infinityNf) & std::signbit(infinityNf),
             "-ve infinity conversion to float invalid");
    h_assert(std::isinf(infinityNd) & std::signbit(infinityNd),
             "-ve infinity conversion to double invalid");
  }

  // NaN
  {
    // Try constructing NaN in different ways and check they all
    // represent the same fp16_t
    fp16_t nanValue = fp16_t::getNaN();
    fp16_t nanValueFromFloat( (float) NAN, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t nanValueFromDouble( (double) NAN, fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(nanValueFromFloat.toBits() == nanValue.toBits(), "Mismatch between constructors");
    h_assert(nanValueFromDouble.toBits() == nanValue.toBits(), "Mismatch between constructors");

    // Check the representation
    h_assert(nanValue.isNaN(), "NaN invalid");
    // Check exponent is all ones
    h_assert((nanValue.toBits() & 0x7c00) == 0x7c00, "NaN exponent invalid");
    // Check significand is non zero
    h_assert((nanValue.toBits() & 0x03ff) > 0, "NaN significant invalid");
    h_assert(nanValue.toHexString() == "nan", "NaN hex string invalid");
    h_assert(nanValue.toDecimalString() == "NaN", "NaN decimal string invalid");

    // Try converting to native float types
    float nanValuef = (float) nanValue;
    double nanValued = (double) nanValue;
    h_assert(std::isnan(nanValuef), "NaN conversion to float invalid");
    h_assert(std::isnan(nanValued), "NaN conversion to float invalid");
  }

  // Test the rounding of a few constants

  // 0.1 Cannot be represented exactly in binary
  // Try rounding the decimal representation in different ways
  {
    fp16_t noughtPointOneRZ("0.1", fp16_t::RoundingMode::TOWARDZERO);
    h_assert(noughtPointOneRZ.toBits() == 0x2e66, "0.1 incorrectly rounded to zero");
    h_assert(noughtPointOneRZ.toHexString() == "0x1.998p-4", "0.1 incorrectly rounded to zero");
    h_assert(noughtPointOneRZ.toDecimalString(0) == "9.9976E-2", "0.1 converted to half then decimal failed");

    // Check can roundtrip convert fp16_t to a string and back again
    fp16_t reconstruct(noughtPointOneRZ.toDecimalString(0).c_str(), fp16_t::RoundingMode::TOWARDZERO);
    h_assert(reconstruct.toBits() == noughtPointOneRZ.toBits(), "roundtrip conversion failed");

    // Try round to nearest and round down. For 0.1 this turns out to be the same as rounding
    // to zero
    fp16_t noughtPointOneRNE("0.1", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    fp16_t noughtPointOneRNA("0.1", fp16_t::RoundingMode::TONEAREST_TIESTOAWAY);
    fp16_t noughtPointOneRD("0.1", fp16_t::RoundingMode::DOWNWARD);
    h_assert(noughtPointOneRNE.toBits() == noughtPointOneRZ.toBits(), "incorrect rounding");
    h_assert(noughtPointOneRNA.toBits() == noughtPointOneRZ.toBits(), "incorrect rounding");
    h_assert(noughtPointOneRD.toBits() == noughtPointOneRZ.toBits(), "incorrect rounding");

    // Try rounding up
    fp16_t noughtPointOneRU("0.1", fp16_t::RoundingMode::UPWARD);
    h_assert(noughtPointOneRU.toBits() == 0x2e67, "0.1 incorrectly rounded up");
    h_assert(noughtPointOneRU.toHexString() == "0x1.99cp-4", "0.1 incorrectly rounded to zero");
  }

  // 4091 is an integer that can't be exactly represented in half
  {
    fp16_t fourZeroNineOneRD("4091", fp16_t::RoundingMode::DOWNWARD);
    h_assert(fourZeroNineOneRD.toBits() == 0x6bfd, "4091 incorreclty rounded down");
    h_assert(fourZeroNineOneRD.toHexString() == "0x1.ff4p11", "4091 incorreclty rounded down");
    h_assert(fourZeroNineOneRD.toDecimalString(0) == "4.09E+3", "4091 converted to half then decimal failed");

    // Check can roundtrip convert fp16_t to a string and back again
    fp16_t reconstruct(fourZeroNineOneRD.toDecimalString(0).c_str(), fp16_t::RoundingMode::DOWNWARD);
    h_assert(reconstruct.toBits() == fourZeroNineOneRD.toBits(), "roundtrip conversion failed");

    fp16_t fourZeroNineOneRU("4091", fp16_t::RoundingMode::UPWARD);
    h_assert(fourZeroNineOneRU.toBits() == 0x6bfe, "4091 incorreclty rounded up");
    h_assert(fourZeroNineOneRU.toHexString() == "0x1.ff8p11", "4091 incorreclty rounded up");
    h_assert(fourZeroNineOneRU.toDecimalString(0) == "4.092E+3", "4091 converted to half then decimal failed");

    fp16_t fourZeroNineOneRZ("4091", fp16_t::RoundingMode::TOWARDZERO);
    h_assert(fourZeroNineOneRZ.toBits() == fourZeroNineOneRD.toBits(), "4091 incorrectly rounded toward zero");

    fp16_t fourZeroNineOneRNE("4091", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(fourZeroNineOneRNE.toBits() == fourZeroNineOneRU.toBits(), "4091 incorrectly rounded towards nearest even");

    fp16_t fourZeroNineOneRNA("4091", fp16_t::RoundingMode::TONEAREST_TIESTOAWAY);
    h_assert(fourZeroNineOneRNA.toBits() == fourZeroNineOneRU.toBits(), "4091 incorrectly rounded towards nearest even");
  }

  // -4091 is an integer that can't be exactly represented in half
  {
    fp16_t fourZeroNineOneRD("-4091", fp16_t::RoundingMode::DOWNWARD);
    h_assert(fourZeroNineOneRD.toBits() == 0xebfe, "-4091 incorreclty rounded down");
    h_assert(fourZeroNineOneRD.toHexString() == "-0x1.ff8p11", "-4091 incorreclty rounded down");
    h_assert(fourZeroNineOneRD.toDecimalString(0) == "-4.092E+3", "-4091 converted to half then decimal failed");

    // Check can roundtrip convert fp16_t to a string and back again
    fp16_t reconstruct(fourZeroNineOneRD.toDecimalString(0).c_str(), fp16_t::RoundingMode::DOWNWARD);
    h_assert(reconstruct.toBits() == fourZeroNineOneRD.toBits(), "roundtrip conversion failed");

    fp16_t fourZeroNineOneRU("-4091", fp16_t::RoundingMode::UPWARD);
    h_assert(fourZeroNineOneRU.toBits() == 0xebfd, "4091 incorreclty rounded up");
    h_assert(fourZeroNineOneRU.toHexString() == "-0x1.ff4p11", "-4091 incorreclty rounded up");
    h_assert(fourZeroNineOneRU.toDecimalString(0) == "-4.09E+3", "-4091 converted to half then decimal failed");

    fp16_t fourZeroNineOneRZ("-4091", fp16_t::RoundingMode::TOWARDZERO);
    h_assert(fourZeroNineOneRZ.toBits() == fourZeroNineOneRU.toBits(), "-4091 incorrectly rounded toward zero");

    fp16_t fourZeroNineOneRNE("-4091", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(fourZeroNineOneRNE.toBits() == fourZeroNineOneRD.toBits(), "-4091 incorrectly rounded towards nearest even");

    fp16_t fourZeroNineOneRNA("-4091", fp16_t::RoundingMode::TONEAREST_TIESTOAWAY);
    h_assert(fourZeroNineOneRNA.toBits() == fourZeroNineOneRD.toBits(), "-4091 incorrectly rounded towards nearest even");
  }

  // 0.3 can't be exactly represented in half
  // This case is slightly different from the 0.1 case because both the "round
  // bit" and "sticky bit" are both 1 (see the Handbook of floating point
  // arithmetic 2.2.1 Rounding modes)
  {
    fp16_t noughtPointThreeRD("0.3", fp16_t::RoundingMode::DOWNWARD);
    h_assert(noughtPointThreeRD.toBits() == 0x34cc, "0.3 incorrectly rounded downward");
    h_assert(noughtPointThreeRD.toHexString() == "0x1.33p-2", "0.3 incorrectly rounded downward");
    h_assert(noughtPointThreeRD.toDecimalString(0) == "2.998E-1", "0.3 incorrectly rounded downward");

    // Possible BUG:?? This doesn't work if we use RoundingMode::DOWNWARD when
    // creating "reconstruct".
    // Check can roundtrip convert fp16_t to a string and back again
    fp16_t reconstruct(noughtPointThreeRD.toDecimalString(0).c_str(), fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(reconstruct.toBits() == noughtPointThreeRD.toBits(), "roundtrip conversion failed");

    fp16_t noughtPointThreeRU("0.3", fp16_t::RoundingMode::UPWARD);
    h_assert(noughtPointThreeRU.toBits() == 0x34cd, "0.3 incorrectly rounded upward");
    h_assert(noughtPointThreeRU.toHexString() == "0x1.334p-2", "0.3 incorrectly rounded upward");
    h_assert(noughtPointThreeRU.toDecimalString(0) == "3.0005E-1", "0.3 incorrectly rounded upward");

    fp16_t noughtPointThreeRZ("0.3", fp16_t::RoundingMode::TOWARDZERO);
    h_assert(noughtPointThreeRZ.toBits() == noughtPointThreeRD.toBits(), "0.3 incorrectly rounded toward nearest even");

    fp16_t noughtPointThreeRNE("0.3", fp16_t::RoundingMode::TONEAREST_TIESTOEVEN);
    h_assert(noughtPointThreeRNE.toBits() == noughtPointThreeRU.toBits(), "0.3 incorrectly rounded toward nearest even");

    fp16_t noughtPointThreeRNA("0.3", fp16_t::RoundingMode::TONEAREST_TIESTOAWAY);
    h_assert(noughtPointThreeRNA.toBits() == noughtPointThreeRU.toBits(), "0.3 incorrectly rounded toward nearest even");
  }

  printf("Success!\n");
  return 0;
}
