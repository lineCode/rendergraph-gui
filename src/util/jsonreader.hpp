#pragma once
#include "util/stringref.hpp"
#include <memory>
#include <stdexcept>

namespace util {
class JsonReader {
public:
  class TypeError : public std::exception {
  public:
    TypeError() = default;
    TypeError(const char *message) : std::exception{message} {}
  };

  JsonReader();
  JsonReader(const util::StringRef &src);
  ~JsonReader();

  int64_t nextInt();
  double nextReal();
  std::string nextString();

  bool hasNext();
  void beginObject();
  void endObject();
  void beginArray();
  void endArray();
  void skipValue();

  std::string nextName();

private:
  struct JsonReaderPrivate;
  std::unique_ptr<JsonReaderPrivate> d;
};
} // namespace util