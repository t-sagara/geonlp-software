include $(top_srcdir)/am.conf
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = libgeonlp src etc geonlp_ma_makedic
DIST_SUBDIRS = $(SUBDIRS) include php-extension
EXTRA_DIST  = m4 autotools.sh configure.ac geonlp-dic-util test/geonlp_api_test.json test/test_api.sh test/geonlp_api_server_client.php 

test_api:
	cat ./test/geonlp_api_test.json | $(bindir)/geonlp_api
