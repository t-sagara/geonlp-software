include $(top_srcdir)/am.conf
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = libgeonlp src etc geonlp_ma_makedic dict
DIST_SUBDIRS = $(SUBDIRS) include php-extension
EXTRA_DIST  = m4 autotools.sh configure.ac test/geonlp_api_test.json test/test_api.sh test/geonlp_api_server_client.php
#EXTRA_DIST = ruby-extension/README.euc_jp ruby-extension/extconf.rb \
             ruby-extension/rbgeonlp_ma.cpp ruby-extension/tests/*.rb

test-preinstall:
	cd dict; make test-api-local

test-postinstall:
	cat ./test/geonlp_api_test.json | $(bindir)/geonlp_api
