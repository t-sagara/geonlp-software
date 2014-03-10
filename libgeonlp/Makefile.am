include $(top_srcdir)/am.conf
AM_CPPFLAGS = -DPROFILE_DEFAULT_DIR_PATH=\"$(sysconfdir)\" $(DAMS_INC_FLAGS)
lib_LTLIBRARIES = libgeonlp.la
libgeonlp_la_SOURCES = DBAccessor.cpp FileAccessor.cpp GeowordFormatter.cpp NodeExt.cpp \
                      Util.cpp GeonlpMA.cpp PHBSDefs.cpp CSVReader.cpp Dictionary.cpp \
                      GeonlpMAImplSq3.cpp MeCabAdapter.cpp Profile.cpp Address.cpp \
                      Geoword.cpp Node.cpp picojsonExt.cpp GeonlpService.cpp \
                      Context.cpp Classifier.cpp \
                      ../include/DBAccessor.h ../include/FileAccessor.h \
                      ../include/MeCabAdapter.h ../include/Suffix.h \
                      ../include/Exception.h ../include/Node.h ../include/Dictionary.h \
                      ../include/GeonlpMA.h ../include/NodeExt.h ../include/Util.h \
                      ../include/GeonlpMAImplSq3.h ../include/PHBSDefs.h ../include/Wordlist.h \
                      ../include/Geoword.h ../include/Profile.h ../include/darts.h \
                      ../include/GeowordFormatter.h ../include/SqliteErrException.h \
                      ../include/SqliteNotInitializedException.h \
                      ../include/DartsException.h ../include/FormatException.h \
                      ../include/picojson.h ../include/picojsonExt.h ../include/CSVReader.h \
                      ../include/Address.h ../include/GeonlpService.h \
                      ../include/Context.h ../include/Classifier.h
libgeonlp_la_LIBADD = $(LIBBOOST_SYSTEM_LIB) $(LIBBOOST_FILESYSTEM_LIB) $(LIBBOOST_REGEX_LIB) $(LIBMECAB_LIB) $(LIBDAMS_LIB)
libgeonlp_la_LDFLAGS = -release $(LIB_VERSION_INFO)
