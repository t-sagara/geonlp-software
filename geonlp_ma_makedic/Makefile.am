include $(top_srcdir)/am.conf
bin_PROGRAMS = geonlp_ma_makedic
geonlp_ma_makedic_SOURCES = GeowordJudge.h MakedicGenerator.h \
  GeowordJudgeFailedException.h MakedicGeneratorApp.h \
  InvalidArgumentException.h MakedicGeneratorAppArgument.h \
  MakedicCsvFileIO.h MakedicGeneratorConfiguration.h \
  MakedicException.h MakedicItem.h \
  GeowordJudge.cpp MakedicGeneratorApp.cpp \
  MakedicCsvFileIO.cpp MakedicGeneratorAppArgument.cpp \
  MakedicGenerator.cpp MakedicGeneratorConfiguration.cpp \
  main.cpp
geonlp_ma_makedic_LDADD = $(LIBGEONLP) $(LIBBOOST_SYSTEM_LIB) $(LIBBOOST_FILESYSTEM_LIB) $(LIBMECAB_LIB) $(LIBSQLITE3_LIB)
