#include "client/client.hpp"
#include <zmq.hpp>

namespace client {
namespace {
template <typename Reply, typename F>
void parseStdReply(util::JsonReader &reader, Reply &reply, F callback) {
  reader.beginObject();
  while (!reader.hasNext()) {
    auto k = reader.nextName();
    if (k == "status") {
      reply.status = static_cast<Status>(reader.nextInt());
    } else if (k == "errorMessage") {
      reply.errorMessage = reader.nextString();
    } else if (k == "data") {
      callback(reader, reply);
    }
  }
  reader.endObject();
}
} // namespace

namespace method {

//------------ CREATE NODE
/*CreateNodeReply::CreateNodeReply(util::JsonReader &r) {
  parseStdReply(r, *this, [](util::JsonReader &r, CreateNodeReply &this_) {
    r.beginObject();
    while (r.hasNext()) {
      auto k = r.nextName();
      if (k == "name") {
        this_.name = r.nextString();
      }
      if (k == "inputConnectors") {
        r.beginArray();
        while (r.hasNext()) {
          r.beginObject();
          while (r.hasNext()) {
            if (k == "id") {
              // TODO
            } else if (k == "name") {
              // TODO
            }
          }
          r.endObject();
        }
        r.endArray();
      } else {
        r.skipValue();
      }
    }
    r.endObject();
  });
}*/

} // namespace method

struct RendergraphClient::RendergraphClientPrivate {

	RendergraphClientPrivate() : zmqctx{}, socket{ zmqctx,zmq::socket_type::req }
	{}

	zmq::context_t zmqctx;
	zmq::socket_t socket;
};

RendergraphClient::RendergraphClient() : d_{std::make_unique<RendergraphClientPrivate>()}
{
}

RendergraphClient::RendergraphClient(util::StringRef address) : RendergraphClient{}
{
	connect(address);
}

RendergraphClient::~RendergraphClient()
{
}

bool RendergraphClient::connect(util::StringRef address)
{
	std::string addr{ address.ptr, address.len };
	d_->socket.connect(addr.c_str());
	return true;
}

std::string RendergraphClient::sendRaw(util::StringRef str)
{
	if (!d_->socket.connected())
		throw std::runtime_error{"socket disconnected"};

	d_->socket.send(str.ptr, str.len);
	zmq::message_t msg;
	d_->socket.recv(&msg);
	return std::string{ (const char*)msg.data(), msg.size() };
}

} // namespace client