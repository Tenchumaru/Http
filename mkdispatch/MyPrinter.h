#pragma once

#include "Printer.h"

class MyPrinter : public Printer {
public:
	MyPrinter();
	~MyPrinter();

private:
	void InternalPrint(vector const& requests, Options const& options) override;
};
