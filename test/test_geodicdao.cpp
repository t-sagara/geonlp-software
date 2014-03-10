#include <iostream>
#include "Theme.h"
#include "DBAccessor.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: test <profile filename>" << std::endl;
    exit(1);
  }

  std::string profile_path = argv[1];
  geoword::DBAccessor dba(profile_path);
  dba.open();

  std::vector<geoword::Theme> all_themes = dba.findAllTheme();
  for (std::vector<geoword::Theme>::iterator it = all_themes.begin(); it != all_themes.end(); it++) {
    std::cout << (*it).toString() << std::endl;
  }

  std::string str;
  std::vector<std::string> regexps;
  std::vector<std::string> theme_ids;
  while (true) {
    std::cerr << "正規表現を入力(exit で終了): ";
    getline(std::cin, str);
    if (str == "exit") break;
    if (str == "clear") {
      regexps.clear();
      continue;
    }
    regexps.push_back(str);
    theme_ids = dba.regexThemeIds(regexps);
    for (std::vector<std::string>::iterator it = theme_ids.begin(); it != theme_ids.end(); it++) {
      std::cout << (*it) << ", ";
    }
    std::cout << std::endl;
  }

  /*
  std::vector<geoword::WordList> wordlists;
  geoword::WordList w0, w1;
  w0.set_id(1);
  w0.set_surface("東京");
  w0.set_idlist("13:都/1234:駅");
  w0.set_yomi("トウキョウ");
  w0.set_pron("トーキョー");

  w1.set_id(2);
  w1.set_surface("東京駅");
  w1.set_idlist("1234:駅");
  w1.set_yomi("トウキョウエキ");
  w1.set_pron("トーキョーエキ");

  wordlists.push_back(w0);
  wordlists.push_back(w1);
  dba.setWordLists(wordlists);
  */

  dba.close();
  exit(0);
}
