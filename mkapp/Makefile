ifeq "$(Configuration)" ""
	Configuration=Debug
endif

CXX  = clang++
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
LDFLAGS = -L/local/lib/$(Configuration)
#LIBS = -lhttp -ladrezdi -lssl -lcrypto -lpthread
CXXFLAGS := -I /local/include -g -fdeclspec -fexceptions -pipe -std=c++17 -Wall -Werror $(CFLAGS)
OUTDIR = ../x64/$(Configuration)
OUTPUT = $(OUTDIR)/mkapp

ifeq "$(Configuration)" "Release"
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
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
