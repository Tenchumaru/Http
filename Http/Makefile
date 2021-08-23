CXX  = clang++
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
OUTPUT = /local/lib/$(Configuration)/libhttp.a
CXXFLAGS := -g -fdeclspec -fexceptions -pipe -std=c++17 -Wall -Werror -I /local/include $(CFLAGS)

ifeq "$(Configuration)" ""
	Configuration=Debug
endif
ifeq "$(Configuration)" "Release"
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
endif

$(OUTPUT): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(LDFLAGS) $^ $(LIBS)

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -MMD -MP -o $@ $<

%.o: %.s
	as -g -o $@ $<

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(OBJS) $(DEPS) $(OUTPUT)

-include $(DEPS)
