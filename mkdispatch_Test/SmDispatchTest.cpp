#include "pch.h"
#include "../Http/AsyncSocket.h"
#include "../Http/AsyncResponse.h"
#include "Utils.inl"
namespace {
#include "smDispatch.inl"
	bool callsCollectParameter = false;
	bool callsCollectQueries = false;
	std::string name = "sm";
}
#define TestClassName SmDispatchTest
#include "DispatchTest.inl"
