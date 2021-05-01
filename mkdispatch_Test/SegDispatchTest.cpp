#include "pch.h"
#include "../Http/AsyncSocket.h"
#include "../Http/AsyncResponse.h"
#include "Utils.inl"
namespace {
#include "segDispatch.inl"
	bool callsCollectParameter = true;
	bool callsCollectQueries = true;
	std::string name = "seg";
}
#define TestClassName SegDispatchTest
#include "DispatchTest.inl"
