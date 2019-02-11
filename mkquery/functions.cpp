void CollectValue(char const*& p, xstring& q) {
	q.first = p;
	while(*p != '\r' && *p != '\n' && *p != '#' && *p != '&') {
		++p;
	}
	q.second = p;
	if(*p == '&') {
		++p;
	}
}

bool CollectQuery(char const* p) {
	Initialize();
	if(*p == '\r' || *p == '\n') {
		return true;
	}
	if(*p != '?') {
		return false;
	}
	++p;

	xstring* q;
	while(CollectName(p, q)) {
		if(q) {
			CollectValue(p, *q);
		} else {
			while(*p != '\r' && *p != '\n' && *p != '#' && *p != '&') {
				++p;
			}
			if(*p == '&') {
				++p;
			} else {
				break;
			}
		}
	}

	// Ignore any fragment.
	if(*p == '#') {
		do {
			++p;
		} while(*p != '\r' && *p != '\n');
	}
	return *p == '\r' || *p == '\n';
}
