#pragma once

#include "HeaderBase.h"
#include "QueryBase.h"
#include "Body.h"
#include "Request.h"
#include "Response.h"
#include "StatusLines.h"

bool AtEndOfPath(char ch);
bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn);
void FourExEx(Response& response, std::string const& statusLine, char const* message = nullptr);
