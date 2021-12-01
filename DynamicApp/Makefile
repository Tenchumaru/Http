CXX := clang++
SRCS := $(wildcard *.cc)
OBJS := $(SRCS:.cc=.o)
DEPS := $(SRCS:.cc=.d)
ifeq "$(Configuration)" ""
	Configuration := Debug
endif
LDFLAGS := -L/local/lib/$(Configuration)
LIBS := -lhttp -ladrezdi -lssl -lcrypto -lpthread
OUTDIR := ../x64/$(Configuration)
OUTPUT := $(OUTDIR)/dynamic-app
CXXFLAGS := -I /local/include -g -fdeclspec -fexceptions -pipe -std=c++17 -Wall -Werror $(CFLAGS)
ifeq "$(Configuration)" "Debug"
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
endif

$(OUTPUT): $(OBJS)
	mkdir -p $(OUTDIR)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS)

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -MMD -MP -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -MMD -MP -o $@ $<

%.o: %.s
	as -g -o $@ $<

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(OBJS) $(DEPS) $(OUTPUT)

-include $(DEPS)
