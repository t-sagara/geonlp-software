#include <iostream>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#include "GeonlpMA.h"

void usage(const char* cmd) {
  std::cerr << "Usage: " << cmd << " [--rc=<rc filename>]" << std::endl;
  std::cerr << "or, " << cmd << " --version" << std::endl;
  return;
}

int main (int argc, char * const argv[]) {
  geonlp::MAPtr service;
  std::string rcfilename = "";

  for (int i = 1; i < argc; i++) {
    if (!strncmp("--version", argv[i], 9)) {
      std::cout << PACKAGE_VERSION << std::endl;
      exit(0);
    } else if (!strncmp("--rc=", argv[i], 5)) {
      rcfilename = std::string(argv[i] + 5);
    } else {
      usage(argv[0]);
      exit(-1);
    }
  }
  /*
  if (rcfilename == "") rcfilename = geonlp::Profile::searchProfile();
  else rcfilename = geonlp::Profile::searchProfile(rcfilename);
  std::cerr << "use " << rcfilename << " as the resource file." << std::endl;
  */

  try {
    if (rcfilename == "") {
      service = geonlp::createMA();
    } else {
      service = geonlp::createMA(rcfilename);
    }
  }
  catch (geonlp::ServiceCreateFailedException& e) {
    std::cerr << e.what();
    return -1;
  }

  for(;;){
    // parse
    std::string line;
    std::getline( std::cin, line);
    if ( line.length()== 0) break;
    std::string str = service->parse( line);
    std::cout << str;
	  
    // parseNode
#ifdef USE_PARSENODE
    std::vector<geonlp::Node> nodes;
    nodes = service->parseNode(line);
    for (unsigned int i = 0; i < nodes.size(); i++) {
      geonlp::Node c_node = nodes[i];
      std::cout << i
		<< "\tsurface:" << c_node.get_surface()
		<< "\tpartOfSpeech:" << c_node.get_partOfSpeech()
		<< std::endl;
    }
#endif
  }
  return 0;
}
