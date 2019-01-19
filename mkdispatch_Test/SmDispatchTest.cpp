#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utils.inl"
namespace {
#include "smDispatch.inl"
	bool callsCollectParameter = false;
}
#define TestClassName SmDispatchTest
#include "DispatchTest.inl"
