#include "util/jsonreader.hpp"
#include "util/jsonwriter.hpp"
#include "util/stringref.hpp"
#include <cstdint>
#include <memory>
#include <sstream>
#include <vector>

namespace client {

enum class Status {
  Success = 0,
  UnknownMethod = 1,
  InvalidParameter = 2,
};

namespace method {

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

  void write(util::JsonWriter &o);
};

template <MethodCode M> struct MethodBase {
  static constexpr MethodCode CODE = M;
};

struct GetVersion : MethodBase<MethodCode::GetVersion> {
  struct Reply : ReplyBase {
    int version;
    void read(util::JsonReader &i) { version = i.nextInt(); }
  };

  void write(util::JsonWriter& o) { 
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
    o.value(static_cast<int64_t>(M::CODE));
    o.name("data");
    m.write(o);
    o.endObject();
    auto str = ss.str();
    sendRaw(util::StringRef{str.c_str(), str.size()});
  }

  std::string sendRaw(util::StringRef str);

private:
	struct RendergraphClientPrivate;
  std::unique_ptr<RendergraphClientPrivate> d_;
};

} // namespace client