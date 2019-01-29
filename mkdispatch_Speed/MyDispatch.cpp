#include "pch.h"
#include "Functions.h"
#include "Utilities.h"
namespace {
#include "myDispatch.inl"
}
#define TestDispatch TestMyDispatch
char const name[] = "MyDispatch";
#include "Dispatch.inl"
