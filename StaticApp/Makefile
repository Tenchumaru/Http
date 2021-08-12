CXX  = clang++
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
LDFLAGS = -L../Http -L../Json
LIBS = -lhttp -ljson
CXXFLAGS := -I /local/include -g -fdeclspec -fexceptions -pipe -std=c++17 -Wall -Werror $(CFLAGS)

ifeq "$(Configuration)" "Release"
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
endif

static-app: $(OBJS)
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
	$(RM) $(OBJS) $(DEPS) static-app

-include $(DEPS)
