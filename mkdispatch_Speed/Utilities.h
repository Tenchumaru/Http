#include "Functions.h"

bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn);
char const* CollectParameter(char const*& p, size_t index);
bool CollectQueries(char const* p);
void FourHundred(Response& response, char const* message);
void FourZeroFour(Response& response);
