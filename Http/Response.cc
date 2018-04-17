#include "stdafx.h"
#include "Response.h"

Response::Response() {}

Response::~Response() {}

void Response::Ok(std::string const& text) {
	text; // TODO
}

void Response::End(unsigned short responseCode) {
	responseCode; // TODO
}

void Response::Send(Socket& client) {
	client; // TODO
}
