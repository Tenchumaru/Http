#pragma once

#include "../mkapp/Printer.h"

class SegPrinter : public Printer {
public:
	SegPrinter();
	~SegPrinter();

private:
	void InternalPrint(vector const& requests, Options const& options, std::ostream& out) override;
};
