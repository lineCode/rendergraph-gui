#pragma once
#include "util/arrayref.h"
#include "util/stringref.h"
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace util {

// Lightweight polymorphic value type that can contain:
// - Integers (int64_t)
// - Floating-point values (double)
// - Strings
// - Arrays of values
// - Objects (associative arrays of values)
// - Packed array of integers
// - Packed array of floating-point values
//
// The type of the value cannot be modified after creation. The value itself
// can, however. This type is meant to be a generic way to represent object
// trees to be serialized to a file or on the network.
class Value {
public:
  using Object = std::map<std::string, Value>;
  using Array = std::vector<Value>;
  using IntArray = std::vector<int64_t>;
  using RealArray = std::vector<double>;

  class OutOfRange : public std::exception {
  public:
    OutOfRange() = default;
    OutOfRange(const char *message) : std::exception{message} {}
  };

  class TypeError : public std::exception {
  public:
    TypeError() = default;
    TypeError(const char *message) : std::exception{message} {}
  };

  Value(Value &&v) { *this = std::move(v); }

  Value &operator=(Value &&v) noexcept {
    reset();
    ty_ = v.ty_;
    switch (v.ty_) {
    case Type::Empty:
      break;
    case Type::Int:
      v_.intVal = v.v_.intVal;
      break;
    case Type::Real:
      v_.doubleVal = v.v_.doubleVal;
      break;
    case Type::String:
      new (&v_.string) std::string{std::move(v.v_.string)};
      break;
    case Type::Object:
      new (&v_.object) Object{std::move(v.v_.object)};
      break;
    case Type::Array:
      new (&v_.array) Array{std::move(v.v_.array)};
      break;
    case Type::IntArray:
      new (&v_.intArray) IntArray{std::move(v.v_.intArray)};
      break;
    case Type::RealArray:
      new (&v_.realArray) RealArray{std::move(v.v_.realArray)};
      break;
    default:
      break;
    }
  }

  enum class Type {
    Empty,
    Int,
    Real,
    String,
    Object,
    Array,
    IntArray,
    RealArray,
  };

  Value() : ty_{Type::Empty} {}

  Value(util::StringRef str) : ty_{Type::String} {
    new (&v_.string) std::string{str.to_string()};
  }

  Value(double doubleVal) : ty_{Type::Real} { v_.doubleVal = doubleVal; }
  Value(int64_t intVal) : ty_{Type::Int} { v_.intVal = intVal; }

  ~Value() { reset(); }

  util::StringRef asString() const {
    checkType(Type::String);
    return util::StringRef{v_.string.c_str(), v_.string.size()};
  }

  double asReal() const {
    checkType(Type::Real);
    return v_.doubleVal;
  }

  int64_t asInt() const {
    checkType(Type::Int);
    return v_.intVal;
  }

  Object &asObject() {
    checkType(Type::Object);
    return v_.object;
  }

  const Object &asObject() const {
    checkType(Type::Object);
    return v_.object;
  }

  Array &asArray() {
    checkType(Type::Array);
    return v_.array;
  }

  const Array &asArray() const {
    checkType(Type::Array);
    return v_.array;
  }

  IntArray &asIntArray() {
    checkType(Type::IntArray);
    return v_.intArray;
  }

  const IntArray &asIntArray() const {
    checkType(Type::IntArray);
    return v_.intArray;
  }

  RealArray &asRealArray() {
    checkType(Type::RealArray);
    return v_.realArray;
  }

  const RealArray &asRealArray() const {
    checkType(Type::RealArray);
    return v_.realArray;
  }

  void reset() {
    switch (ty_) {
    case Type::Empty:
    case Type::Int:
    case Type::Real:
      break;
    case Type::String:
      v_.string.~basic_string();
      break;
    case Type::Object:
      v_.object.~map();
      break;
    case Type::Array:
      v_.array.~vector();
      break;
    case Type::IntArray:
      v_.intArray.~vector();
      break;
    case Type::RealArray:
      v_.realArray.~vector();
      break;
    default:
      break;
    }
    ty_ = Type::Empty;
  }

  static Value makeObject() {
    Value v;
    v.ty_ = Type::Object;
    new (&v.v_.object) Object;
    return v;
  }

  static Value makeArray() {
    Value v;
    v.ty_ = Type::Array;
    new (&v.v_.array) Array;
    return v;
  }

private:
  void checkType(Type ty) const {
    if (ty_ != ty) {
      throw TypeError{};
    }
  }

  union Inner {
    Inner() : intVal{0} {}

    ~Inner() {}

    double doubleVal;
    int64_t intVal;
    std::string string;
    Array array;
    Object object;
    RealArray realArray;
    IntArray intArray;
  };

  Type ty_;
  Inner v_;
};

} // namespace util