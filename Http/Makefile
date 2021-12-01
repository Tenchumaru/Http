CXX := clang++
SRCS := $(wildcard *.cc)
OBJS := $(SRCS:.cc=.o)
DEPS := $(SRCS:.cc=.d)
ifeq "$(Configuration)" ""
	Configuration := Debug
endif
OUTDIR := /local/lib/$(Configuration)
OUTPUT := $(OUTDIR)/libhttp.a
CXXFLAGS := -g -fdeclspec -fexceptions -pipe -std=c++17 -Wall -Werror -I /local/include $(CFLAGS)
ifeq "$(Configuration)" "Debug"
	CXXFLAGS := $(CXXFLAGS) -O0 -D_DEBUG -DDEBUG
else
	CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
endif

$(OUTPUT): $(OBJS)
	mkdir -p $(OUTDIR)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -MMD -MP -o $@ $<

%.o: %.s
	as -g -o $@ $<

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(OBJS) $(DEPS) $(OUTPUT)

-include $(DEPS)
