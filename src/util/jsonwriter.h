#pragma once
#include "util/stringref.h"
#include <memory>
#include <stdexcept>
#include <ostream>

namespace util {
class JsonWriter {
public:
  JsonWriter(std::ostream &out);
  ~JsonWriter();

  void beginObject();
  void endObject();
  void beginArray();
  void endArray();

  void name(util::StringRef name);
  void value(int64_t val);
  void value(double val);
  void value(util::StringRef str);

private:
  struct JsonWriterPrivate;
  std::unique_ptr<JsonWriterPrivate> d;
};
} // namespace util