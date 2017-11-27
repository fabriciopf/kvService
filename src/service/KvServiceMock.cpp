#include "KvServiceMock.hpp"

#include <thread>
#include <chrono>

pplx::task<bool> KvServiceMock::putItem(const kv::proto::set_request& request) {
	return pplx::create_task([request, this] {
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return true;
	});
}

pplx::task<bool> KvServiceMock::getItem(const kv::proto::get_request& request, std::shared_ptr<kv::proto::data> data) {
	return pplx::create_task([request, this] {
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return true;
	});
}
