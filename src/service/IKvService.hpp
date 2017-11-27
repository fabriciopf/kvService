#ifndef SERVICE_IKV_HPP_
#define SERVICE_IKV_HPP_

#include <memory>
#include "payload.pb.h"
#include "pplx/pplxtasks.h"

class IKvService {
public:
	virtual ~IKvService() {};
	virtual pplx::task<bool> putItem(const kv::proto::set_request& request) = 0;
	virtual pplx::task<bool> getItem(const kv::proto::get_request& request, std::shared_ptr<kv::proto::data> data) = 0;
};

#endif
