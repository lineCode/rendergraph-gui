#include "util/jsonwriter.h"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>

namespace util {

struct JsonWriter::JsonWriterPrivate {
  JsonWriterPrivate(std::ostream &out) : wrapper{out}, writer{wrapper} {}

  void beginObject() { writer.StartObject(); }
  void endObject() { writer.EndObject(); }
  void beginArray() { writer.StartArray(); }
  void endArray() { writer.EndArray(); }
  void name(StringRef name) { writer.Key(name.ptr, name.len, true); }
  void value(int64_t val) { writer.Int64(val); }
  void value(double val) { writer.Double(val); }
  void value(util::StringRef str) { writer.String(str.ptr, str.len, true); }

  rapidjson::OStreamWrapper wrapper;
  rapidjson::Writer<rapidjson::OStreamWrapper> writer;
};

//==============================================================================================
JsonWriter::JsonWriter(std::ostream &out)
    : d{std::make_unique<JsonWriterPrivate>(out)} {}

JsonWriter::~JsonWriter() {}

void JsonWriter::beginObject() { d->beginObject(); }
void JsonWriter::endObject() { d->endObject(); }
void JsonWriter::beginArray() { d->beginArray(); }
void JsonWriter::endArray() { d->beginArray(); }
void JsonWriter::name(util::StringRef name) { d->name(name); }
void JsonWriter::value(int64_t val) { d->value(val); }
void JsonWriter::value(double val) { d->value(val); }
void JsonWriter::value(util::StringRef val) { d->value(val); }

} // namespace util