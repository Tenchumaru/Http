bool CollectName(char const*& p, xstring*& q) {
	while(*p != '=' && *p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
		++p;
	}
	if(*p == '=') {
		++p;
		q = nullptr;
		return true;
	}
	return false;
}

void CollectValue(char const*& p, xstring& q) {
	q.first = p;
	while(*p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
		++p;
	}
	q.second = p;
	if(*p == '&') {
		++p;
	}
}

bool CollectQuery(char const* p) {
	Initialize();

	// Check for early termination cases.
	if(*p == '#') {
		do {
			++p;
		} while(*p != ' ' && *p != '\r' && *p != '\n');
	}
	if(*p == ' ') {
		return true;
	}
	if(*p != '?') {
		return false;
	}
	++p;

	// Loop, expecting name-value pairs.
	xstring* q;
	while(CollectName(p, q)) {
		if(q) {
			CollectValue(p, *q);
		} else {
			while(*p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
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
		} while(*p != ' ' && *p != '\r' && *p != '\n');
	}
	return *p == ' ';
}
