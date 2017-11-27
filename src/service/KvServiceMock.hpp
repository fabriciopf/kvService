#ifndef SERVICE_KVSERVICEMOCK_HPP_
#define SERVICE_KVSERVICEMOCK_HPP_

#include "IKvService.hpp"

class KvServiceMock : public IKvService {
public:
	virtual ~KvServiceMock() {};
	static KvServiceMock& getInstance() {static KvServiceMock instance; return instance;};
	pplx::task<bool> putItem(const kv::proto::set_request& request) override;
	pplx::task<bool> getItem(const kv::proto::get_request& request, std::shared_ptr<kv::proto::data> data) override;

};

#endif /* SERVICE_KVSERVICEMOCK_HPP_ */
