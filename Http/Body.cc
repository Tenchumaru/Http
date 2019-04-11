#include "stdafx.h"
#include "Body.h"

Body::Body(char const* begin, char*& next, int size, TcpSocket& client) : begin(begin), next(next), size(size), client(client) {}
