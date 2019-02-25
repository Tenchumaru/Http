#include "pch.h"
#include "Dispatch.h"

int main() {
#ifdef _DEBUG
	constexpr int count = 999999;
#else
	constexpr int count = 9999999;
#endif
	TestMyDispatch(count);
	TestSegDispatch(count);
	TestSmDispatch(count);
}
