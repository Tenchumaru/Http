ifeq "$(Configuration)" ""
	Configuration=Debug
endif

all:
	cd Http && $(MAKE) "Configuration=$(Configuration)"
	cd Json && $(MAKE) "Configuration=$(Configuration)"
	cd App && $(MAKE) "Configuration=$(Configuration)"

clean:
	cd Http && $(MAKE) clean "Configuration=$(Configuration)"
	cd Json && $(MAKE) clean "Configuration=$(Configuration)"
	cd App && $(MAKE) clean "Configuration=$(Configuration)"

.PHONY: all clean
