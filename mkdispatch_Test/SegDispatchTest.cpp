#include "pch.h"
#include "Utilities.h"
namespace {
#include "segDispatch.inl"
	bool callsCollectParameter = true;
	bool callsCollectQueries = true;
	std::string name = "seg";
}
#define TestClassName SegDispatchTest
#include "DispatchTest.inl"
