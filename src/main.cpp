#include "ui/mainwindow.hpp"

#include <iostream>
#include <QApplication>
#include <cstdint>

const char* ENDPOINT = "tcp://127.0.0.1:5555";

int main(int argc, char **argv) {
	/*// Create ZMQ context and socket
	zmq::context_t zmqctx;
	zmq::socket_t socket{ zmqctx, zmq::socket_type::req };
	socket.connect(ENDPOINT);
	
	// send a request
	std::stringstream buffer;
	msgpack::pack(buffer, (uint32_t)Method::GetVersion);
	auto str = buffer.str();
	socket.send(str.data(), str.size());

	// receive reply
	zmq::message_t msg;
	socket.recv(&msg);

	// read version
	msgpack::object_handle obj = msgpack::unpack((const char*)msg.data(), msg.size());
	auto deserialized = obj.get();
	unsigned int version = 0;
	deserialized.convert(version);
	std::cerr << "Version: " << version << "\n";*/
	

	// Create window
	auto app = new QApplication(argc, argv);
	auto mainWindow = new ui::MainWindow();
	mainWindow->show();
	app->exec();
    return 0;
}