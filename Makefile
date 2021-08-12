ifeq "$(Configuration)" ""
	Configuration=Debug
endif

all:
	cd Http && $(MAKE) "Configuration=$(Configuration)"
	cd StaticApp && $(MAKE) "Configuration=$(Configuration)"
	cd DynamicApp && $(MAKE) "Configuration=$(Configuration)"

clean:
	cd Http && $(MAKE) clean "Configuration=$(Configuration)"
	cd StaticApp && $(MAKE) clean "Configuration=$(Configuration)"
	cd DynamicApp && $(MAKE) clean "Configuration=$(Configuration)"

.PHONY: all clean
