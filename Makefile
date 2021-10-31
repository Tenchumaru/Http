ifeq "$(Configuration)" ""
	Configuration=Debug
endif

all:
	mkdir -p "x64/$(Configuration)"
	cd Http && $(MAKE) "Configuration=$(Configuration)"
	cd mkapp && $(MAKE) "Configuration=$(Configuration)"
	cd StaticApp && $(MAKE) "Configuration=$(Configuration)"
	cd DynamicApp && $(MAKE) "Configuration=$(Configuration)"
	cd Http_Test && $(MAKE) "Configuration=$(Configuration)"

clean:
	cd Http && $(MAKE) clean "Configuration=$(Configuration)"
	cd mkapp && $(MAKE) clean "Configuration=$(Configuration)"
	cd StaticApp && $(MAKE) clean "Configuration=$(Configuration)"
	cd DynamicApp && $(MAKE) clean "Configuration=$(Configuration)"
	cd Http_Test && $(MAKE) clean "Configuration=$(Configuration)"
	$(RM) -r "x64/$(Configuration)"

.PHONY: all clean
