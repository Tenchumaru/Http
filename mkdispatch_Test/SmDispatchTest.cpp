#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utils.inl"
namespace {
#include "smDispatch.inl"
	bool callsCollectParameter = false;
	std::string name = "sm";
}
#define TestClassName SmDispatchTest
#include "DispatchTest.inl"
