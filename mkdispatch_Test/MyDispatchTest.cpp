#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utils.inl"
namespace {
#include "myDispatch.inl"
	bool callsCollectParameter = true;
	std::string name = "my";
}
#define TestClassName MyDispatchTest
#include "DispatchTest.inl"
