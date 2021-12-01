CXX := clang++
HEADERS := $(wildcard *.h)
INLS := $(wildcard *.cpp)
ifeq "$(Configuration)" ""
	Configuration := Debug
endif
OUTDIR = x64/$(Configuration)
CXXFLAGS := -g -fdeclspec -fexceptions -pipe -std=c++17 -Wall -Werror -D_AMD64_ -I. -I/local/include $(CFLAGS)
ifeq "$(Configuration)" "Debug"
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
endif

.PHONY: all

all: $(OUTDIR) $(OUTDIR)/results

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/results: $(OUTDIR)/run-tests
	$< > $@
	cat $@

$(OUTDIR)/run-tests: $(OUTDIR)/run-tests.cc $(OUTDIR)/run-tests.inl /local/lib/$(Configuration)/libhttp.a $(HEADERS)
	$(CXX) $(CXXFLAGS) -L /local/lib/$(Configuration) -MMD -MP -o $@ $(OUTDIR)/run-tests.cc -lhttp -ladrezdi -lcrypto -lssl -lpthread

$(OUTDIR)/run-tests.cc: $(OUTDIR)/run-tests.inl $(OUTDIR)/mkrun-tests
	grep -h TEST_ $(INLS) | sed -e 's/.*TEST_\(.\)[^_(]*.\([^)]*\).*/\1\2/' -e 's/^MINITIALIZE(/I/' | $(OUTDIR)/mkrun-tests > $@

$(OUTDIR)/run-tests.inl: $(INLS)
	[ ! -s $@ ] || rm $@
	for f in $(INLS);do echo '#include "'$$f'"' >> $@;done

$(OUTDIR)/mkrun-tests: mkrun-tests.cc
	$(CXX) $(CXXFLAGS) -MMD -MP -o $@ $<

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) -r $(OUTDIR)
