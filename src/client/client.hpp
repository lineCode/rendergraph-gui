#include <memory>
#include <cstdint>
#include <vector>
#include <map>
#include "util/stringref.hpp"

namespace client {

class RendergraphClientPrivate;

// Requests to the server:
// - Create a node, returns a node ID

using NodeId = uint64_t;

enum class Status {
	Success = 0,
	UnknownMethod = 1,
	InvalidParameter = 2,	
};


namespace methods {
	struct CreateNode {
		// No params
		
		struct Reply {
			std::vector<NodeId> createdNodeIds;
		};

		ErrorCode send(RendergraphClient& client, ) {
			Message msg;
			msg.setParam("param", ...);
			client.send(msg);

			auto reply = msg.send();
		}

	};

	// methods::CreateNode m;
	// m.param1 = ...;
	// m.send(client);	
	//

}

// Object => serialized object tree, actual format is abstract: can be toml or json or whatever is needed
// Client takes an object and builds a message out of it
// Can subclass object to create specialized object types, with custom get methods (instead of copying the data to a struct)


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

	Value(util::StringRef str) : ty{ Type::String }  {
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


class Message {
public:
	Message();
	~Message();

	// Sets the value of a parameter to the given integer.
	void setParam(const char* name, int64_t v);
	// Sets the value of a parameter to the given floating-point value.
	void setParam(const char* name, double v);
	// Sets the value of a parameter to a copy of the given string.
	void setParam(const char* name, const char* str);

	// Sets the value of a parameter to a copy of the given array of integer values.
	// This copies the data, so the array can be freed afterwards.
	void setArrayParam(const char* name, int count, const int64_t * v);
	// Sets the value of a parameter to a copy of the given array of floating-point values.
	// This copies the data, so the array can be freed afterwards.
	void setArrayParam(const char* name, int count, const double* v);
	// Sets the value of a parameter to a copy of the given array of strings.
	// This copies the data, so the array can be freed afterwards.
	void setArrayParam(const char* name, int count, const char* const * strs);

	// Gets the value of a parameter, or throws an exception if there is no
	// such parameter in the message or it is not of the expected type.
	Status paramDouble(const char* name, double& out);
	Status paramInt(const char* name, int64_t& out);
	// The returned string lives as long as the message is not modified by a set* method.
	Status paramString(const char* name, std::string& out);

	Status paramType(const char* name, ParamType )

	const double* paramDoubleArray(const char* name, size_t& arraysize);
	const int64_t* paramIntArray(const char* name, size_t& arraysize);
	const char* paramStringArray(const char* name, size_t& arraysize);

	// array of objects?

private:
	class Private;
	std::unique_ptr<Private> d;
};

class RendergraphClient {
public:
  RendergraphClient(const char *address);
  ~RendergraphClient();

  // Sends a message.
  Status send(const Message& msg, Message& reply);


private:
  std::unique_ptr<RendergraphClientPrivate> d_;
};

} // namespace client