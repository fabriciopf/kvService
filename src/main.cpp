#include <iostream>

#include <usr_interrupt_handler.hpp>
#include <runtime_utils.hpp>

#include "controller/rest_controller.hpp"
#include "service/KvService.hpp"
#include "service/KvServiceMock.hpp"

using namespace web;
using namespace cfx;

int main(int argc, const char * argv[]) {
	InterruptHandler::hookSIGINT();

	IKvService& service = KvServiceMock::getInstance();
	RestController server(service);
	server.setEndpoint("http://host_auto_ip4:8000/kv");

	try {
		server.accept().wait();
		std::cout << "Miniclip API now listening for requests at: " << server.endpoint() << '\n';
		InterruptHandler::waitForUserInterrupt();
		server.shutdown().wait();
	} catch (std::exception & e) {
		std::cerr << "Oh no! [" << e.what() << "]" << std::endl;
	} catch (...) {
		RuntimeUtils::printStackTrace();
	}

	return 0;
}
