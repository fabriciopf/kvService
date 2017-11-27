#pragma once 

#include <basic_controller.hpp>
#include "../service/IKvService.hpp"
#include <memory>

using namespace cfx;

class RestController : public BasicController, Controller {
private:
	void processSetRequest(kv::proto::req_envelope req, http_request message);
	void processGetRequest(kv::proto::req_envelope req, http_request message);
	pplx::task<kv::proto::req_envelope> parseBody(utility::string_t body, std::string contentType);
	void replyOk(http_request request, google::protobuf::Message* message);
	void replyInternalError(http_request request, google::protobuf::Message* message);

	IKvService& kvService;

public:
    RestController(IKvService& service) : BasicController(), kvService(service) {}
    virtual ~RestController() {}
    void handleGet(http_request message) override;
    void handlePut(http_request message) override;
    void handlePost(http_request message) override;
    void handlePatch(http_request message) override;
    void handleDelete(http_request message) override;
    void handleHead(http_request message) override;
    void handleOptions(http_request message) override;
    void handleTrace(http_request message) override;
    void handleConnect(http_request message) override;
    void handleMerge(http_request message) override;
    void initRestOpHandlers() override;    

private:
    static json::value responseNotImpl(const http::method & method);
};
