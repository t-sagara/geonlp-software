#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include "Profile.h"

int main(int argc, char** argv) {
  geoword::Profile profile;
  
  if (argc < 2) {
    throw std::runtime_error("Usage: main <profile-path>");
  }
  std::cerr << "Loading profile from : " << argv[1] << std::endl;

  profile.load(argv[1]);

  if (profile.get_data_dir().empty()) {
    throw std::runtime_error("no data_dir property in the profile.");
  } else {
    std::cout << "  sqlite3_file  = " << profile.get_sqlite3_file() << std::endl;
    std::cout << "  wordlist_file  = " << profile.get_wordlist_file() << std::endl;
    std::cout << "  darts_file    = " << profile.get_darts_file() << std::endl;
    std::cout << "  mecab_userdic = " << profile.get_mecab_userdic() << std::endl;
  }
  exit(0);
}
