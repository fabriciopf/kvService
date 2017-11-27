#include "rest_controller.hpp"
#include "payload.pb.h"
#include "../util/pbjson.hpp"
#include <rapidjson/document.h>

using namespace web;
using namespace http;

void RestController::initRestOpHandlers() {
	const std::function<void(http_request)>& getFn = std::bind(&RestController::handleGet, this, std::placeholders::_1);
	const std::function<void(http_request)>& postFn = std::bind(&RestController::handlePost, this, std::placeholders::_1);
	const std::function<void(http_request)>& delFn = std::bind(&RestController::handleDelete, this, std::placeholders::_1);
    _listener.support(methods::GET, getFn);
    _listener.support(methods::POST, postFn);
    _listener.support(methods::DEL, delFn);
}

void RestController::handleGet(http_request request) {
	kv::proto::req_envelope env;
	env.set_type(kv::proto::req_envelope_msg_type_get_request_t);
	kv::proto::get_request* get_req = new kv::proto::get_request;
	get_req->set_key("089");
	env.set_allocated_get_req(get_req);
	processGetRequest(env, request);
}

void RestController::replyInternalError(http_request request, google::protobuf::Message* message) {
	if (!message) {
		request.reply(status_codes::InternalError, utf8string("Invalid request"));
	} else if (request.headers().content_type() == "application/json") {
		std::string json;
		pbjson::pb2json(message, json);
		request.reply(status_codes::InternalError, json, "application/json");
	} else {
		request.reply(status_codes::InternalError, message->SerializeAsString(), "application/octet-stream");
	}
}

void RestController::replyOk(http_request request, google::protobuf::Message* message) {
	if (!message) {
		request.reply(status_codes::InternalError, utf8string("Invalid request"));
	} else if (request.headers().content_type() == "application/json") {
		std::string json;
		pbjson::pb2json(message, json);
		request.reply(status_codes::OK, json, "application/json");
	} else {
		request.reply(status_codes::OK, message->SerializeAsString(), "application/octet-stream");
	}
}

void RestController::processGetRequest(kv::proto::req_envelope req, http_request request) {
	if (!req.has_get_req()) {
		kv::proto::get_response message;
		message.set_error(kv::proto::get_response_error_t_internal);
		replyInternalError(request, &message);
		return;
	}

	std::shared_ptr<kv::proto::data> data(std::make_shared<kv::proto::data>());
	kvService.getItem(req.get_req(), data).then([=] (bool ret) {
		kv::proto::get_response message;
		if (!ret) {
			message.set_error(kv::proto::get_response_error_t_internal);
			replyInternalError(request, &message);
		} else {
			message.set_allocated_req(new kv::proto::data(*data));
			message.set_error(kv::proto::get_response_error_t_ok);
			replyOk(request, &message);
		}
	});

}

void RestController::processSetRequest(kv::proto::req_envelope req, http_request request) {
	if (!req.has_set_req()) {
		kv::proto::set_response message;
		message.set_error(kv::proto::set_response_error_t_internal);
		replyInternalError(request, &message);
		return;
	}

	kvService.putItem(req.set_req()).then([=] (bool result) {
		kv::proto::set_response message;
		if (!result) {
			message.set_error(kv::proto::set_response_error_t_internal);
			replyInternalError(request, &message);
		} else {
			message.set_error(kv::proto::set_response_error_t_ok);
			replyOk(request, &message);
		}
	});

}

pplx::task<kv::proto::req_envelope> RestController::parseBody(utility::string_t body, std::string contentType) {
	return pplx::create_task([body, contentType]{
		kv::proto::req_envelope req{};
    	if (contentType == "application/json") {
    		std::string err;
    		pbjson::json2pb(body, &req, err);
    	} else if (contentType == "application/octect-binary") {
    		req.ParseFromString(body);
		}
    	//std::cout << "payload: " << req.DebugString() << std::endl;
		return req;
	});
}

void RestController::handlePost(http_request request) {
    request.extract_utf8string().then([=](utility::string_t body) {
    	parseBody(body, request.headers().content_type()).then([=](kv::proto::req_envelope req) {
    		switch (req.type()) {
    		case kv::proto::req_envelope_msg_type_set_request_t:
    			processSetRequest(req, request);
    			break;
    		case kv::proto::req_envelope_msg_type_get_request_t:
    			processGetRequest(req, request);
    			break;
    		default:
    			replyInternalError(request, nullptr);
    			break;
    		}
    	});
	});
}

void RestController::handlePut(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PUT));
}

void RestController::handlePatch(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PATCH));
}

void RestController::handleDelete(http_request message) {    
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::DEL));
}

void RestController::handleHead(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::HEAD));
}

void RestController::handleOptions(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::OPTIONS));
}

void RestController::handleTrace(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::TRCE));
}

void RestController::handleConnect(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::CONNECT));
}

void RestController::handleMerge(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::MERGE));
}

json::value RestController::responseNotImpl(const http::method & method) {
    auto response = json::value::object();
    response["message"] = json::value::string("HTTP method not implemented");
    return response ;
}
