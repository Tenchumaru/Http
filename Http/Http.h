#pragma once

#include "HeaderBase.h"
#include "QueryBase.h"
#include "Body.h"
#include "AsyncBody.h"
#include "Request.h"
#include "Response.h"
#include "AsyncResponse.h"
#include "StatusLines.h"

bool AtEndOfPath(char ch);
bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn);
void FourHundred(Response& response, char const* message);
void FourZeroFour(Response& response);
Task<void> FourHundredAsync(AsyncResponse& response, char const* message);
Task<void> FourZeroFourAsync(AsyncResponse& response);
