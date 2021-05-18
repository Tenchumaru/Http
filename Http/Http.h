#pragma once

// These constants specify the limits on the number of request headers, the
// size of request header names and values, and the size of the payload.
constexpr size_t maxHeaders = 99;
constexpr size_t maxNameSize = 333;
constexpr size_t maxValueSize = 999;
constexpr size_t maxContentLength = 999'999;

#include "HeaderBase.h"
#include "QueryBase.h"
#include "Body.h"
#include "Request.h"
#include "Response.h"
#include "StatusLines.h"

bool AtEndOfPath(char ch);
bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn);
void FourExEx(Response& response, std::string const& statusLine, char const* message = nullptr);
