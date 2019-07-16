#include "client/client.hpp"
#include <zmq.hpp>
#include <util/log.hpp>

namespace client {

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
	util::log("[RendergraphClient::connect] endpoint={}", addr.c_str());
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