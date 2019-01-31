#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utils.inl"
namespace {
#include "segDispatch.inl"
	bool callsCollectParameter = true;
	std::string name = "seg";
}
#define TestClassName SegDispatchTest
#include "DispatchTest.inl"
