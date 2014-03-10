include $(top_srcdir)/am.conf
bin_PROGRAMS       = geonlp_ma geonlp_add geonlp_rebuild geonlp_api geonlp_api_server
geonlp_ma_SOURCES  = geonlp_ma.cpp
geonlp_ma_LDADD    = $(LIBGEONLP) $(LIBSQLITE3_LIB)
geonlp_add_SOURCES = geonlp_add.cpp
geonlp_add_LDADD   = $(LIBGEONLP) $(LIBSQLITE3_LIB)
geonlp_rebuild_SOURCES = geonlp_rebuild.cpp
geonlp_rebuild_LDADD = $(LIBGEONLP) $(LIBSQLITE3_LIB)
geonlp_api_SOURCES = geonlp_api.cpp
geonlp_api_LDADD   = $(LIBGEONLP) $(LIBSQLITE3_LIB)
geonlp_api_server_SOURCES = geonlp_api_server.cpp
geonlp_api_server_LDADD   = $(LIBGEONLP) $(LIBBOOST_SYSTEM_LIB) $(LIBBOOST_THREAD_LIB) $(LIBSQLITE3_LIB)
