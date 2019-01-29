#include "pch.h"
#include "Dispatch.h"

int main() {
#ifdef _DEBUG
	int const count = 999999;
#else
	int const count = 9999999;
#endif
	TestMyDispatch(count);
	TestSegDispatch(count);
	TestSmDispatch(count);
}
