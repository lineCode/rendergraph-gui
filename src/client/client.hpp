#include "util/jsonreader.hpp"
#include "util/jsonwriter.hpp"
#include "util/stringref.hpp"
#include <cstdint>
#include <memory>
#include <sstream>
#include <vector>

namespace client {

enum class Status: int {
  Success = 0,
  UnknownMethod = 1,
  InvalidParameter = 2,
};

namespace method {

template <typename Reply>
void parseStdReply(util::JsonReader &reader, Reply &reply) {
  reader.beginObject();
  while (reader.hasNext()) {
    auto k = reader.nextName();
    if (k == "status") {
      reply.status = static_cast<Status>(reader.nextInt());
    } else if (k == "errorMessage") {
      reply.errorMessage = reader.nextString();
    } else if (k == "data") {
      reply.read(reader);
	}
	else {
		reader.skipValue();
	}
  }
  reader.endObject();
}

enum class MethodCode {
  GetVersion = 1,
};

struct ReplyBase {
  Status status;
  std::string errorMessage;
};

struct CreateNode {
  struct Reply : ReplyBase {
    std::string name;
    void read(util::JsonReader &i);
  };

  void write(util::JsonWriter &o) const;
};

struct MethodBase {
};

struct GetVersion : MethodBase {
	static constexpr const char* NAME = "GetVersion";

  struct Reply : ReplyBase {
    int version;
    void read(util::JsonReader &i) { version = i.nextInt(); }
  };

  void write(util::JsonWriter &o) const {
    o.beginObject();
    o.endObject();
  }
};

} // namespace method

class RendergraphClient {
public:
  RendergraphClient();
  RendergraphClient(util::StringRef address);
  ~RendergraphClient();

  bool connect(util::StringRef address);
  bool isConnected() const;

  // Sends a message consisting of the given message body.
  //
  // Throws an exception if the message could not be sent, or a reply could
  // not be received.
  template <typename M> auto send(const M &m) -> typename M::Reply {
    std::ostringstream ss;
    util::JsonWriter o{ss};
    o.beginObject();
    o.name("method");
    o.value(M::NAME);
    o.name("data");
    m.write(o);
    o.endObject();
    auto str = ss.str();
    auto reply_str = sendRaw(util::StringRef{str.c_str(), str.size()});
    util::JsonReader i{util::StringRef{reply_str.c_str(), reply_str.size()}};
    M::Reply reply;
    method::parseStdReply(i, reply);
    return reply;
  }

  std::string sendRaw(util::StringRef str);

private:
  struct RendergraphClientPrivate;
  std::unique_ptr<RendergraphClientPrivate> d_;
};

} // namespace client