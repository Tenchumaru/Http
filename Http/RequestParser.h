#pragma once

#include "HttpParser.h"
#include "Http.h"
#include "Request.h"
#include "Uri.h"

class RequestParser : public HttpParser {
public:
	using fn_t= std::function<void(Request const& request)>;

	explicit RequestParser(fn_t requestHandler);
	~RequestParser();

private:
	fn_t requestHandler;
	Uri uri;

	bool ValidateNext(std::string const& s) override;
	bool ValidateLast(std::string const& s) override;
	void HandleMessage() override;
};
