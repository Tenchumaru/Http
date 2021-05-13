#include "pch.h"
#include "Utilities.h"
namespace {
#include "smDispatch.inl"
	bool callsCollectParameter = false;
	bool callsCollectQueries = false;
	std::string name = "sm";
}
#define TestClassName SmDispatchTest
#include "DispatchTest.inl"
