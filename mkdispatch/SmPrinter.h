#pragma once

#include "../mkapp/Printer.h"

class SmPrinter : public Printer {
public:
	SmPrinter();
	~SmPrinter();

private:
	void InternalPrint(vector const& requests, Options const& options, std::ostream& out) override;
};
