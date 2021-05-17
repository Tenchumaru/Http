#include "Functions.h"

class TestResponse : public Response {
public:
	TestResponse(TcpSocket& socket);
	~TestResponse() {}

private:
	Date date;
};

char const* CollectParameter(char const*& p, size_t index);
bool CollectQueries(char const* p);
