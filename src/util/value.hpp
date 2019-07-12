#pragma once
#include "util/arrayref.hpp"
#include "util/stringref.hpp"

// Lightweight polymorphic value type that can contain:
// - Integers (int64_t)
// - Floating-point values (double)
// - Strings 
// - Arrays of values
// - Objects (associative arrays of values)
// - Packed array of integers
// - Packed array of floating-point values
//
// The type of the value cannot be modified after creation. The value itself can, however.
// This type is meant to represent object trees before serialization (either to a file, or to a network socket).
class Value {
public:
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

	Value() : ty{ Type::Object }
	{}

	Value(util::StringRef str) : ty{ Type::String } {
		v.stringVal.len = str.len;
		v.stringVal.data = new char[str.len];
		std::memcpy(v.stringVal.data, str.ptr, str.len);
	}

	Value(double doubleVal) : ty{ Type::Real } {
		v.doubleVal = doubleVal;
	}

	Value(int64_t intVal) : ty{ Type::Int } {
		v.intVal = intVal;
	}

	~Value() {
		switch (ty)
		{
		case Type::Empty:
		case Type::Int:
		case Type::Real:
			break;
		case Type::String:
			delete[] v.stringVal.data;
			break;
		case Type::Object:
			delete v.objVal;
			break;
		case Type::Array:
			delete[] v.arrayVal.data;
			break;
		case Type::IntArray:
			delete[] v.intArrayVal.data;
			break;
		case Type::RealArray:
			delete[] v.realArrayVal.data;
			break;
		default:
			break;
		}
	}

private:
	struct StringData {
		size_t len;
		char* data;
	};

	struct RealArrayData {
		size_t len;
		double* data;
	};

	struct IntArrayData {
		size_t len;
		int64_t* data;
	};

	struct Object {
		std::map<std::string, Value> values;
	};

	struct ArrayData {
		size_t len;
		Value* data;
	};

	union Inner {
		double doubleVal;
		int64_t intVal;
		StringData stringVal;
		ArrayData arrayVal;
		Object* objVal;
		RealArrayData realArrayVal;
		IntArrayData intArrayVal;
	};

	Type ty;
	Inner v;
};