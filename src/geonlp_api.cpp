#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GeonlpService.h"

void usage(const char* cmd) {
  std::cerr << "Usage: " << cmd << " [--rc=<rc filename>] [<jsonfile>]" << std::endl;
  std::cerr << "or, " << cmd << " --version" << std::endl;
  return;
}

bool proc(geonlp::ServicePtr service, std::stringstream& ss_req) {
  picojson::ext req;
  picojson::value res;

  if (ss_req.str().length() == 0) return false;
  req.initByJson(ss_req.str());
  res = service->proc(picojson::value(req));
  std::cout << res.serialize() << std::endl;
  ss_req.str("");
  return true;
}

int main (int argc, char * const argv[]) {
  geonlp::ServicePtr service;
  std::string rcfilename = "";
  std::string infile = "";

  for (int i = 1; i < argc; i++) {
    if (!strncmp("--version", argv[i], 9)) {
      std::cout << PACKAGE_VERSION << std::endl;
      exit(0);
    } else if (!std::strncmp("--rc=", argv[i], 5)) {
      rcfilename = std::string(argv[i] + 5);
    } else if (infile.length() == 0 && argv[i][0] != '-') {
      infile = std::string(argv[i]);
    } else {
      usage(argv[0]);
      exit(1);
    }
  }

  try {
    if (rcfilename == "") {
      service = geonlp::createService();
    } else {
      service = geonlp::createService(rcfilename);
    }
  } catch (geonlp::ServiceCreateFailedException& e) {
    std::cerr << e.what();
    return 1;
  }

  std::stringstream ss_req;
  std::string line;
  std::istream* is = &std::cin;
  std::ifstream ifs;
  if (infile.length() > 0) {
    ifs.open(infile.c_str());
    if (!ifs.is_open()) {
      std::cerr << "File '" << infile << "' is not readable." << std::endl;
      return 1;
    }
    is = &ifs;
  }
  
  ss_req.str("");
  while(!is->eof()) {
    // parse
    std::getline(*is, line);
    ss_req << line;
    if (line.length() == 0) break;
  }
  proc(service, ss_req);

  return 0;
}
