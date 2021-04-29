#include "pch.h"
#include "Utilities.h"
namespace {
#include "segDispatch.inl"
}
#define TestDispatch TestSegDispatch
char const name[] = "SegDispatch";
#include "Dispatch.inl"
