#include "pch.h"
#include "Utilities.h"
namespace {
#include "myDispatch.inl"
	bool callsCollectParameter = true;
	bool callsCollectQueries = false;
	std::string name = "my";
}
#define TestClassName MyDispatchTest
#include "DispatchTest.inl"
