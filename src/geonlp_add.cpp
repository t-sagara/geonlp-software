/**
 * @file  geonlp_add.cpp
 * @brief コマンドラインから辞書ファイルを読み込むプログラム
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
  std::cerr << "Usage: " << cmd << " [--rc=<rc filename>] <jsonfile.json> <jsonfile.csv>" << std::endl;
  std::cerr << "or, " << cmd << " --version" << std::endl;
  return;
}

int main (int argc, char * const argv[]) {
  std::string jsonfile = "";
  std::string csvfile = "";
  std::string rcfilename = "";

  for (int i = 1; i < argc; i++) {
    if (!strncmp("--version", argv[i], 9)) {
      std::cout << PACKAGE_VERSION << std::endl;
      exit(0);
    } else if (!strncmp("--rc=", argv[i], 5)) {
      rcfilename = std::string(argv[i] + 5);
    } else if (argv[i][0] == '-') {
      usage(argv[0]);
      exit(-1);
    } else {
      std::string args = std::string(argv[i]);
      if (args.substr(args.length() - 5, 5) == ".json") {
	jsonfile = args;
      } else if (args.substr(args.length() - 4, 4) == ".csv") {
	csvfile = args;
      } else {
	usage(argv[0]);
	exit(-1);
      }
    }
  }
  if (jsonfile == "" || csvfile == "") {
    usage(argv[0]);
    exit(-1);
  }
  if (rcfilename == "") rcfilename = geonlp::Profile::searchProfile();
  else rcfilename = geonlp::Profile::searchProfile(rcfilename);
  std::cerr << "use " << rcfilename << " as the resource file." << std::endl;

  geonlp::DBAccessor dba(rcfilename);
  geonlp::FileAccessor fa(dba);
  dba.open();

  // データベースにファイルから読み込み
  try {
    fa.importDictionaryCSV(csvfile, jsonfile);
  } catch (picojson::PicojsonException e) {
    std::cerr << "ファイル読み込み時にエラーが発生しました。" << e.what() << std::endl;
    exit(-1);
  }

  std::cerr << "ファイルの読み込みに成功しました。\ngeonlp_ma_makedic を実行してインデックスを再作成してください。" << std::endl;
}
