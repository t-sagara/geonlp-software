EXTRA_DIST = README phpgeonlp.ini phpgeonlp.xml tests/*.php

PROJROOT	= $(shell pwd)
PROJNAME	= php@PACKAGE_NAME@
MAKEDIR		= $(PROJROOT)/$(PROJNAME)
SPEC		= $(PROJNAME).xml
TARGET		= $(PROJNAME).so

all:	$(TARGET)

install:
	(cd $(MAKEDIR); make install;)
	cp $(PROJROOT)/$(PROJNAME).ini /etc/php.d

$(TARGET):	$(SPEC)
	pecl-gen $(SPEC);
	(cd $(MAKEDIR); \
	  phpize; \
          ./configure; \
          make;)

clean:
	rm -rf $(MAKEDIR)
