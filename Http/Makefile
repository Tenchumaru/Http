CXX  = clang++
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
CXXFLAGS := -g -fdeclspec -fexceptions -pipe -std=c++14 -Wall -Werror $(CFLAGS)

ifeq "$(Configuration)" "Release"
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
endif

libhttp.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(LDFLAGS) $^ $(LIBS)

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -MMD -MP -o $@ $<

%.o: %.s
	as -g -o $@ $<

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(OBJS) $(DEPS) libhttp.a

-include $(DEPS)
