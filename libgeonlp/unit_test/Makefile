CXX = g++
CXXFLAGS = -g -DPROFILE_DEFAULT_DIR_PATH=\"/usr/local/etc\" -I../../include
#CXXFLAGS = -g -DHAVE_LIBDAMS -DPROFILE_DEFAULT_DIR_PATH=\"/usr/local/etc\" -I../../include
#CXXFLAGS = -g -DDEBUG -DHAVE_LIBDAMS -DPROFILE_DEFAULT_DIR_PATH=\"/usr/local/etc\" -I../../include
#CXXFLAGS = -pg -DPROFILE_DEFAULT_DIR_PATH=\"/usr/local/etc\" -I../../include
LFLAGS   = -lsqlite3 -lboost_regex -lboost_filesystem -lmecab -L/data/geonlp/lib -ldams
#LFLAGS   = -lsqlite3 -lboost_regex -lboost_filesystem -lmecab

OBJS     = ../picojsonExt.o ../Geoword.o ../Address.o ../Dictionary.o \
        ../Profile.o ../DBAccessor.o ../CSVReader.o \
	../FileAccessor.o ../GeonlpMA.o ../GeonlpMAImplSq3.o ../Node.o ../NodeExt.o ../MeCabAdapter.o \
	../PHBSDefs.o ../GeowordFormatter.o ../GeonlpService.o ../Context.o ../Classifier.o ../Util.o \
	../JsonRpcClient.o

test_picojson:	test_picojson.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ test_picojson.cpp $(OBJS) $(LFLAGS)

test_geoword:	test_geoword.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ test_geoword.cpp $(OBJS) $(LFLAGS)

test_dictionary:	test_dictionary.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ test_dictionary.cpp $(OBJS) $(LFLAGS)

test_dbaccessor:	test_dbaccessor.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

test_fileaccessor:	test_fileaccessor.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

test_ma:	test_ma.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

test_service:	test_service.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

test_parse:	test_parse.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

test_util:	test_util.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

test_rpcclient:	test_rpcclient.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LFLAGS)

clean:
	-rm *~ *.o test_geoword test_dictionary test_dbaccessor test_fileaccessor test_ma test_service test_parse test_picojson test_util test_rpcclient
