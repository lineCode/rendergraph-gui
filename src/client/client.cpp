// include zmq first because WinSock2.h must be included before Windows.h (other headers may include Windows.h)
#include <zmq.hpp>		

#include "client/client.h"
#include "util/log.h"

namespace client {

struct RendergraphClient::RendergraphClientPrivate {

  RendergraphClientPrivate()
      : zmqctx{}, socket{zmqctx, zmq::socket_type::req} {}

  zmq::context_t zmqctx;
  zmq::socket_t socket;
};

RendergraphClient::RendergraphClient()
    : d_{std::make_unique<RendergraphClientPrivate>()} {}

RendergraphClient::RendergraphClient(util::StringRef address)
    : RendergraphClient{} {
  connect(address);
}

RendergraphClient::~RendergraphClient() {}

bool RendergraphClient::connect(util::StringRef address) {
  std::string addr = address.to_string();
  util::log("[RendergraphClient::connect] endpoint={}", addr.c_str());
  d_->socket.connect(addr.c_str());
  return true;
}

bool RendergraphClient::isConnected() const {
	return d_->socket.connected();
}
 
void RendergraphClient::setTimeoutMs(int timeoutMs) {
	d_->socket.setsockopt(ZMQ_SNDTIMEO, timeoutMs);
	d_->socket.setsockopt(ZMQ_RCVTIMEO, timeoutMs);
}

std::string RendergraphClient::sendRaw(util::StringRef str) {
  if (!d_->socket.connected())
    throw std::runtime_error{"socket disconnected"};

  d_->socket.send(str.data(), str.size());

  zmq::message_t msg;
  if (!d_->socket.recv(&msg)) {
    // timeout
    throw TimeoutError{"timeout waiting for reply from server"};
  }
  return std::string{(const char *)msg.data(), msg.size()};
}

} // namespace client