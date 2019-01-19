#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utils.inl"
namespace {
#include "segDispatch.inl"
	bool callsCollectParameter = true;
}
#define TestClassName SegDispatchTest
#include "DispatchTest.inl"
