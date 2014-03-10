/**
 * @file  geonlp_rebuild.cpp
 * @brief コマンドラインからインデックスを再構築する
 * @author 株式会社情報試作室
 * @copyright 2013, NII
 **/

#include <iostream>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#include "Profile.h"
#include "DBAccessor.h"
#include "FileAccessor.h"

void usage(const char* cmd) {
  std::cerr << "Usage: " << cmd << " [--rc=<rc filename>]" << std::endl;
  std::cerr << "or, " << cmd << " --version" << std::endl;
  return;
}

int main (int argc, char * const argv[]) {
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
  if (rcfilename == "") rcfilename = geonlp::Profile::searchProfile();
  else rcfilename = geonlp::Profile::searchProfile(rcfilename);
  std::cerr << "use " << rcfilename << " as the resource file." << std::endl;

  geonlp::DBAccessor dba(rcfilename);
  dba.open();
  dba.updateWordlists();
  dba.close();

}
