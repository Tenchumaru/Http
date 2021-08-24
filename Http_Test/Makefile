CXX  = clang++
HEADERS = $(wildcard *.h)
INLS = $(wildcard *.cpp)
CXXFLAGS := -g -fdeclspec -fexceptions -pipe -std=c++17 -Wall -Werror -D_AMD64_ -I. -I/local/include $(CFLAGS)

ifeq "$(Configuration)" ""
	Configuration=Debug
endif
ifeq "$(Configuration)" "Release"
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
endif

x64/results: x64/run-tests
	$< > $@
	cat $@

x64/run-tests: x64/run-tests.cc x64/run-tests.inl /local/lib/$(Configuration)/libhttp.a $(HEADERS)
	$(CXX) $(CXXFLAGS) -MMD -MP -o $@ x64/run-tests.cc /local/lib/$(Configuration)/libhttp.a /local/lib/$(Configuration)/libadrezdi.a -lcrypto -lssl -lpthread

x64/run-tests.cc: x64/run-tests.inl x64/mkrun-tests
	grep -h TEST_ $(INLS) | sed -e 's/.*TEST_\(.\)[^_(]*.\([^)]*\).*/\1\2/' -e 's/^MINITIALIZE(/I/' | x64/mkrun-tests > $@

x64/run-tests.inl: $(INLS)
	[ ! -s $@ ] || rm $@
	for f in $(INLS);do echo '#include "'$$f'"' >> $@;done

x64/mkrun-tests: mkrun-tests.cc
	$(CXX) $(CXXFLAGS) -MMD -MP -o $@ $<

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) x64/*run-tests*
