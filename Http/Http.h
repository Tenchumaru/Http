#pragma once

#include "HeaderBase.h"
#include "QueryBase.h"
#include "Request.h"
#include "Response.h"
#include "StatusLines.h"

bool AtEndOfPath(char ch);
bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn);
void FourHundred(Response& response, char const* message);
void FourZeroFour(Response& response);
