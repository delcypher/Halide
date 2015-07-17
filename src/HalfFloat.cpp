#include "HalfFloat.h"
#include "Error.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/Support/ErrorHandling.h"

using namespace Halide;

// These helper functions are not members of fp16_t because
// it would expose the implementation
namespace {
llvm::APFloat::roundingMode
  getLLVMAPFRoundingMode(Halide::fp16_t::RoundingMode mode) {
    switch(mode) {
      case fp16_t::RoundingMode::TOWARDZERO:
        return llvm::APFloat::roundingMode::rmTowardZero;
      case fp16_t::RoundingMode::TONEAREST_TIESTOEVEN:
        return llvm::APFloat::roundingMode::rmNearestTiesToEven;
      case fp16_t::RoundingMode::UPWARD:
        return llvm::APFloat::roundingMode::rmTowardPositive;
      case fp16_t::RoundingMode::DOWNWARD:
        return llvm::APFloat::roundingMode::rmTowardNegative;
      case fp16_t::RoundingMode::TONEAREST_TIESTOAWAY:
        return llvm::APFloat::roundingMode::rmNearestTiesToAway;
      default:
        internal_error << "Invalid rounding mode :" << (int) mode << "\n";
    }
    llvm_unreachable("Could not get LLVM rounding mode");
}

fp16_t toFP16(llvm::APFloat v) {
  uint64_t bits = v.bitcastToAPInt().getZExtValue();
  internal_assert(bits <= 0xFFFF) << "Invalid bits for fp16_t\n";
  return fp16_t((uint16_t) bits);
}

llvm::APFloat toLLVMAPF(fp16_t v) {
  llvm::APInt bitRepr(16, (uint64_t) v.toBits(), /*isSigned=*/false);
  llvm::APFloat repr(llvm::APFloat::IEEEhalf, bitRepr);
  // use assert to avoid cost of conversion in release builds
  assert(v.toBits() == toFP16(repr).toBits() && "Converting to APFloat and back failed");
  return repr;
}

template<typename T>
void checkConversion(llvm::APFloat::opStatus status,
                            T value,
                            const char* typeName,
                            llvm::APFloat result) {
  // Check the exceptions
  internal_assert(!(status & llvm::APFloat::opStatus::opInvalidOp)) << "Unexpected invalid op\n";
  internal_assert(!(status & llvm::APFloat::opStatus::opDivByZero)) << "Unexpected div by zero\n";
  if (status & llvm::APFloat::opStatus::opOverflow) {
    user_error << "Converting " << value << " of type " << typeName <<
               " to fp16_t results in overflow (Result \"" <<
                 toFP16(result).toDecimalString() << "\")\n";
  }
  if (status & llvm::APFloat::opStatus::opUnderflow) {
    internal_assert(status & llvm::APFloat::opStatus::opInexact) <<
      "Underflow was flagged but inexact was not\n";
    // We don't emit a warning here because we will emit another warning
    // when handling ``opInexact``. APFloat mimics the default
    // exception handling behaviour for underflow in IEEE754 (7.5 Underflow)
    // where a flag is only raised if the result is inexact.
  }

  if (status & llvm::APFloat::opStatus::opInexact) {
    user_warning << "Converting " << value << " of type " << typeName <<
                 " to fp16_t cannot be done exactly (Result \"" <<
                 toFP16(result).toHexString() << "\" which is approximately \"" <<
                 toFP16(result).toDecimalString() << "\" in decimal)\n";
  }
}

template<typename T>
uint16_t getBitsFrom(T value, fp16_t::RoundingMode roundingMode, const char* typeName) {
  llvm::APFloat convertedValue(value);
  bool losesInfo = false;
  llvm::APFloat::opStatus status = convertedValue.convert(
    llvm::APFloat::IEEEhalf,
    getLLVMAPFRoundingMode(roundingMode),
    &losesInfo);
  checkConversion(status, value, typeName, convertedValue);
  return toFP16(convertedValue).toBits();
}

template<>
uint16_t getBitsFrom(const char* value, fp16_t::RoundingMode roundingMode, const char* typeName) {
  llvm::APFloat convertedValue(llvm::APFloat::IEEEhalf);
  // TODO: Sanitize value
  llvm::APFloat::opStatus status = convertedValue.convertFromString(value,
    getLLVMAPFRoundingMode(roundingMode));
  checkConversion(status, value, typeName, convertedValue);
  return toFP16(convertedValue).toBits();
}

} //end anonymous namespace
// End helper functions

namespace Halide {
  // The static_asserts checking the size is to make sure
  // fp16_t can be used as a 16-bits wide POD type.
fp16_t::fp16_t(float value, RoundingMode roundingMode) {
  static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
  this->data = getBitsFrom(value, roundingMode, "float");
}

fp16_t::fp16_t(double value, RoundingMode roundingMode) {
  static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
  this->data = getBitsFrom(value, roundingMode, "double");
}

fp16_t::fp16_t(const char* stringRepr, RoundingMode roundingMode) {
  static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
  this->data = getBitsFrom(stringRepr, roundingMode, "string");
}

fp16_t::fp16_t() {
  static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
  this->data = 0;
}

fp16_t::fp16_t(uint16_t rawBits) {
  static_assert(sizeof(fp16_t) == 2, "fp16_t is wrong size");
  this->data = rawBits;
}

fp16_t::operator float() {
  llvm::APFloat convertedValue = toLLVMAPF(*this);
  bool losesInfo = false;
  // Converting to a more precise type so the rounding mode does not matter, so
  // just pick any.
  convertedValue.convert(llvm::APFloat::IEEEsingle, llvm::APFloat::rmNearestTiesToEven, &losesInfo);
  internal_assert(!losesInfo) << "Unexpected information loss\n";
  return convertedValue.convertToFloat();
}
fp16_t::operator double() {
  llvm::APFloat convertedValue = toLLVMAPF(*this);
  bool losesInfo = false;
  // Converting to a more precise type so the rounding mode does not matter, so
  // just pick any.
  convertedValue.convert(llvm::APFloat::IEEEdouble, llvm::APFloat::rmNearestTiesToEven, &losesInfo);
  internal_assert(!losesInfo) << "Unexpected information loss\n";
  return convertedValue.convertToDouble();
}

fp16_t fp16_t::getZero(bool positive) {
  llvm::APFloat zero = llvm::APFloat::getZero(llvm::APFloat::IEEEhalf, !positive);
  return toFP16(zero);
}

fp16_t fp16_t::getInfinity(bool positive) {
  llvm::APFloat inf = llvm::APFloat::getInf(llvm::APFloat::IEEEhalf, !positive);
  return toFP16(inf);
}

fp16_t fp16_t::getNaN() {
  llvm::APFloat nan = llvm::APFloat::getNaN(llvm::APFloat::IEEEhalf);
  return toFP16(nan);
}

fp16_t fp16_t::add(fp16_t rhs, RoundingMode roundingMode) const {
  llvm::APFloat result = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(rhs);
  // FIXME: Ignoring possible exceptions
  result.add(rhsAPF, getLLVMAPFRoundingMode(roundingMode));
  return toFP16(result);
}

fp16_t fp16_t::subtract(fp16_t rhs, RoundingMode roundingMode) const {
  llvm::APFloat result = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(rhs);
  // FIXME: Ignoring possible exceptions
  result.subtract(rhsAPF, getLLVMAPFRoundingMode(roundingMode));
  return toFP16(result);
}

fp16_t fp16_t::multiply(fp16_t rhs, RoundingMode roundingMode) const {
  llvm::APFloat result = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(rhs);
  // FIXME: Ignoring possible exceptions
  result.multiply(rhsAPF, getLLVMAPFRoundingMode(roundingMode));
  return toFP16(result);
}

fp16_t fp16_t::divide(fp16_t denominator, RoundingMode roundingMode) const {
  llvm::APFloat result = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(denominator);
  // FIXME: Ignoring possible exceptions
  result.divide(rhsAPF, getLLVMAPFRoundingMode(roundingMode));
  return toFP16(result);
}

fp16_t fp16_t::remainder(fp16_t denominator) const {
  llvm::APFloat result = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(denominator);
  // FIXME: Ignoring possible exceptions
  result.remainder(rhsAPF);
  return toFP16(result);
}

fp16_t fp16_t::mod(fp16_t denominator, RoundingMode roundingMode) const {
  llvm::APFloat result = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(denominator);
  // FIXME: Ignoring possible exceptions
  result.mod(rhsAPF, getLLVMAPFRoundingMode(roundingMode));
  return toFP16(result);
}

fp16_t fp16_t::operator-() const {
  llvm::APFloat result = toLLVMAPF(*this);
  result.changeSign();
  return toFP16(result);
}

fp16_t fp16_t::operator+(fp16_t rhs) const {
  return this->add(rhs, RoundingMode::TONEAREST_TIESTOEVEN);
}

fp16_t fp16_t::operator-(fp16_t rhs) const {
  return this->subtract(rhs, RoundingMode::TONEAREST_TIESTOEVEN);
}

fp16_t fp16_t::operator*(fp16_t rhs) const {
  return this->multiply(rhs, RoundingMode::TONEAREST_TIESTOEVEN);
}

fp16_t fp16_t::operator/(fp16_t rhs) const {
  return this->divide(rhs, RoundingMode::TONEAREST_TIESTOEVEN);
}

bool fp16_t::operator==(fp16_t rhs) const {
  llvm::APFloat lhsAPF = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(rhs);
  return lhsAPF.compare(rhsAPF) == llvm::APFloat::cmpEqual;
}

std::string fp16_t::toHexString() const {
  // Expected format of result: [-]0xh.hhhhp[+-]d
  // 11 characters are needed for half precision
  // + 1 for null terminator
  char buffer[12];
  llvm::APFloat repr = toLLVMAPF(*this);
  // The rounding mode does not matter here when we set hexDigits to 0 which
  // will give the precise representation. So any rounding mode will do.
  unsigned count = repr.convertToHexString(buffer,
                                           /*hexDigits=*/ 0,
                                           /*upperCase=*/ false,
                                           llvm::APFloat::rmNearestTiesToEven);
  internal_assert(count < sizeof(buffer)/sizeof(char)) << "Incorrect buffer size\n";
  std::string result(buffer);
  return result;
}

bool fp16_t::operator>(fp16_t rhs) const {
  internal_assert(!this->areUnordered(rhs)) << "Cannot compare unorderable values\n";
  llvm::APFloat lhsAPF = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(rhs);
  return lhsAPF.compare(rhsAPF) == llvm::APFloat::cmpGreaterThan;

}

bool fp16_t::operator<(fp16_t rhs) const {
  internal_assert(!this->areUnordered(rhs)) << "Cannot compare unorderable values\n";
  llvm::APFloat lhsAPF = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(rhs);
  return lhsAPF.compare(rhsAPF) == llvm::APFloat::cmpLessThan;

}

bool fp16_t::areUnordered(fp16_t rhs) const {
  llvm::APFloat lhsAPF = toLLVMAPF(*this);
  llvm::APFloat rhsAPF = toLLVMAPF(rhs);
  return lhsAPF.compare(rhsAPF) == llvm::APFloat::cmpUnordered;

}

std::string fp16_t::toDecimalString(unsigned int significantDigits) const {
  llvm::APFloat repr = toLLVMAPF(*this);
  llvm::SmallVector<char,16> result;
  repr.toString(result, /*FormatPrecision=*/significantDigits, /*FormatMaxPadding=*/0);
  return std::string(result.begin(), result.end());
}

bool fp16_t::isNaN() const {
  llvm::APFloat repr = toLLVMAPF(*this);
  return repr.isNaN();
}

bool fp16_t::isInfinity() const {
  llvm::APFloat repr = toLLVMAPF(*this);
  return repr.isInfinity();
}

bool fp16_t::isNegative() const {
  llvm::APFloat repr = toLLVMAPF(*this);
  return repr.isNegative();
}

bool fp16_t::isZero() const {
  llvm::APFloat repr = toLLVMAPF(*this);
  return repr.isZero();
}

uint16_t fp16_t::toBits() const {
  return this->data;
}

} // namespace halide
