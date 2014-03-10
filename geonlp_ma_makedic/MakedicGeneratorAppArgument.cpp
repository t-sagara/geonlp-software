#include <iostream>
#include <string>
#include "Profile.h"
#include "MakedicGeneratorAppArgument.h"

MakedicGeneratorAppArgument::MakedicGeneratorAppArgument(int argc, const char **argv)throw (InvalidArgumentException) {
  std::string profile_path;
  int opt;
  this->opt_csv_output_filename = "";

  while ((opt = getopt(argc, (char* const*)argv, "uhvnif:d:o:")) != -1) {
    std::string str(1, opt);
    switch (opt) {
    case 'u':
    case 'h':
    case 'v':
    case 'n':
    case 'i':
      this->options.insert(str);
      break;
    case 'f':
      this->options.insert("f");
      opt_ini_file_path = optarg;
      break;
    case 'd':
      this->options.insert("d");
      opt_geoword_hinshi_id = optarg;
      break;
    case 'o':
      this->options.insert("o");
      opt_csv_output_filename = optarg;
      break;
    default:
      throw InvalidArgumentException("invalid option.");
    }
  }

  argc -= optind;
  argv += optind;

  if (argc < 1) {
    // throw InvalidArgumentException("invalid number of arguments.");
    // profile path が省略されたので自動的に探す
    profile_path = geonlp::Profile::searchProfile();
  } else {
    profile_path = argv[0];
  }
		
  try {
    std::cerr << "プロファイルをロード中 : " << profile_path << std::endl;
    this->profile.load(profile_path);
  } catch (std::runtime_error& e) {
    throw InvalidArgumentException("プロファイルの読み込みに失敗しました。");
  }
	
  if (this->profile.get_data_dir().empty()) {
    throw InvalidArgumentException("プロファイルにデータディレクトリが定義されていません。");
  } else {
    std::cout << "  sqlite3_file = " << this->profile.get_sqlite3_file() << std::endl;
    std::cout << "  darts_file   = " << this->profile.get_darts_file() << std::endl;
    std::cout << "  wordlist_file = " << this->profile.get_wordlist_file() << std::endl;
    std::cout << "  mecab_userdic = " << this->profile.get_mecab_userdic() << std::endl;
  }
}

bool MakedicGeneratorAppArgument::hasOption(const std::string& key) const {
  return options.count(key);
}
