bool CollectName(char const*& p, xstring*& q) {
	while(*p != ':' && *p != '\r' && *p != '\n') {
		++p;
	}
	if(*p == ':') {
		++p;
		q = nullptr;
		return true;
	}
	return false;
}

void CollectValue(char const*& p, xstring& q) {
	while(*p == ' ' || *p == '\t') {
		++p;
	}
	q.first = p;
	while(*p != '\r' && *p != '\n') {
		++p;
	}
	q.second = p;
	while(q.second[-1] == ' ' || q.second[-1] == '\t' && q.first < q.second) {
		--q.second;
	}
	if(*p == '\r') {
		++p;
	}
	if(*p == '\n') {
		++p;
	}
}

bool CollectHeaders(char const* p) {
	Initialize();

	// Check for early termination cases.
	if(*p == '\r' || *p == '\n') {
		return true;
	}

	// Loop, expecting name-value pairs.
	xstring* q;
	while(CollectName(p, q)) {
		if(q) {
			CollectValue(p, *q);
		} else {
			while(*p != '\r' && *p != '\n') {
				++p;
			}
			if(*p == '\r') {
				++p;
			}
			if(*p == '\n') {
				++p;
			}
			if(*p == '\r' || *p == '\n') {
				break;
			}
		}
	}

	if(*p == '\r') {
		++p;
	}
	if(*p == '\n') {
		++p;
	}
	return true;
}
