#include "util/jsonreader.h"
#include "util/value.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>

namespace util {
namespace {

enum class JsonToken {
  Begin,
  Int,
  Real,
  String,
  BeginObject,
  EndObject,
  BeginArray,
  EndArray,
  Key,
  End,
};

struct Handler {

  JsonToken last = JsonToken::Begin;
  union {
    double doubleVal;
    int64_t intVal;
  } u;
  std::string lastKey;
  std::string stringVal;
  int depth = 0;

  bool Null() { return false; }
  bool Bool(bool b) { return false; }

  bool Int(int val) {
    last = JsonToken::Int;
    u.intVal = val;
    return true;
  }

  bool Uint(unsigned val) {
    last = JsonToken::Int;
    u.intVal = val;
    return true;
  }

  bool Int64(int64_t val) {
    last = JsonToken::Int;
    u.intVal = val;
    return true;
  }

  bool Uint64(uint64_t val) {
    last = JsonToken::Int;
    u.intVal = val;
    return true;
  }

  bool Double(double val) {
    last = JsonToken::Real;
    u.doubleVal = val;
    return true;
  }

  bool RawNumber(const char *str, rapidjson::SizeType length, bool copy) {
    return false;
  }

  bool String(const char *str, rapidjson::SizeType length, bool copy) {
    last = JsonToken::Real;
    stringVal = std::string{str, length};
    return true;
  }

  bool StartObject() {
    depth += 1;
    last = JsonToken::BeginObject;
    return true;
  }

  bool Key(const char *str, rapidjson::SizeType length, bool copy) {
    last = JsonToken::Key;
    lastKey = std::string{str, length};
    return true;
  }

  bool EndObject(rapidjson::SizeType memberCount) {
    depth -= 1;
    last = JsonToken::EndObject;
    return true;
  }

  bool StartArray() {
    depth += 1;
    last = JsonToken::BeginArray;
    return true;
  }

  bool EndArray(rapidjson::SizeType elementCount) {
    depth -= 1;
    last = JsonToken::EndArray;
    return true;
  }
};

} // namespace

struct JsonReader::JsonReaderPrivate {
  JsonReaderPrivate(const char *src) : handler{}, str{src}, reader{} {
    reader.IterativeParseInit();
    next();
  }

  void expectString(std::string &val) {
    if (handler.last != JsonToken::String)
      throw TypeError{};
    val = std::move(handler.stringVal);
    next();
  }

  void expectKey(std::string &val) {
    if (handler.last != JsonToken::Key)
      throw TypeError{};
    val = std::move(handler.lastKey);
    next();
  }

  void expectInt(int64_t &val) {
    if (handler.last != JsonToken::Int)
      throw TypeError{};
    val = handler.u.intVal;
    next();
  }

  void expectReal(double &val) {
    if (handler.last != JsonToken::Real)
      throw TypeError{};
    val = handler.u.doubleVal;
    next();
  }

  void beginObject() {
    if (handler.last != JsonToken::BeginObject)
      throw TypeError{};
    next();
  }

  void endObject() {
    if (handler.last != JsonToken::EndObject)
      throw TypeError{};
    next();
  }
  void beginArray() {
    if (handler.last != JsonToken::BeginArray)
      throw TypeError{};
    next();
  }

  void endArray() {
    if (handler.last != JsonToken::EndArray)
      throw TypeError{};
    next();
  }

  bool hasNext() {
    return handler.last != JsonToken::EndArray &&
           handler.last != JsonToken::EndObject;
  }

  void skipValue() {
    if (handler.last == JsonToken::BeginObject ||
        handler.last == JsonToken::BeginArray) {
      int depth = handler.depth - 1;
      do {
        next();
      } while (handler.depth != depth);
    } else {
      next();
    }
  }

  bool next() {
    if (reader.IterativeParseComplete()) {
      return false;
    }
    return reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(str,
                                                                    handler);
  }

  Handler handler;
  rapidjson::StringStream str;
  rapidjson::Reader reader;
};

//==============================================================================================
JsonReader::JsonReader() : JsonReader("") {}

JsonReader::JsonReader(const StringRef &src)
    : d{std::make_unique<JsonReaderPrivate>(src.data())} {}

JsonReader::~JsonReader() {}

bool JsonReader::hasNext() { return d->hasNext(); }

int64_t JsonReader::nextInt() {
  int64_t result;
  d->expectInt(result);
  return result;
}

double JsonReader::nextReal() {
  double result;
  d->expectReal(result);
  return result;
}

std::string JsonReader::nextString() {
  std::string result;
  d->expectString(result);
  return result;
}

void JsonReader::beginObject() { d->beginObject(); }
void JsonReader::endObject() { d->endObject(); }
void JsonReader::beginArray() { d->beginArray(); }
void JsonReader::endArray() { d->beginArray(); }
void JsonReader::skipValue() { d->skipValue(); }

std::string JsonReader::nextName() {
  std::string result;
  d->expectKey(result);
  return result;
}

} // namespace util