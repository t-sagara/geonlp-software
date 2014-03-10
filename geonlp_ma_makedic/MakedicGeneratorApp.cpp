#include "MakedicGeneratorApp.h"
#include "MakedicGeneratorAppArgument.h"
#include "MakedicGenerator.h"
#include "MakedicGeneratorConfiguration.h"
#include <map>
#include <iostream>

void MakedicGeneratorApp::usage(const char* progname) {
  std::cout << "Usage: " << progname << " [options] [profile_filepath]\n"
	    << "\nOptions:\n"
	    << "-u\t\tupdate geonlp database index\n"
	    << "-f filepath\tspecify configuration file (geonlp_ma_makedic.rc)\n"
	    << "-v\t\tverbose mode\n"
	    << "-i\t\tIPADIC mode (generate userdic for IPADIC)\n"
	    << "-n\t\tnaist-jdic mode (generate userdic for naist-jdic)\n"
	    << "-d id\t\tgeoword class id (for mecab, default=1293)\n"
	    << "-o filename\toutput MeCab compatible CSV with calculated costs\n"
	    << "-h\t\tshow this message\n"
	    << std::endl;
}

int MakedicGeneratorApp::exec(int argc, const char **argv) {

  try {

    // 起動引数の解析
    MakedicGeneratorAppArgument arg(argc, argv);

    if (arg.hasOption("h")) {
      usage(argv[0]);
      return 0;
    }

    // 構成定義ファイルの読込み
    std::string file;
    if (arg.hasOption("f")) {
      file = arg.getOptIniFilePath();
    } else {
      file = DEFAULT_INI_FILE_PATH;
    }
    MakedicGeneratorConfiguration conf(file);

    // ビジネスロジックオブジェクトの生成＆設定
    MakedicGenerator generator(conf, arg);
		
    // 辞書の作成
    if (arg.hasOption("u")) {
      generator.generate(true);  // Wordlist の更新も行う
    } else {
      generator.generate(false); // Wordlist の更新は行わない
    }

  } catch (const InvalidArgumentException &e) {
    std::cerr << "MakedicGenerator: " << e.what() << std::endl;
    usage(argv[0]);
    return 1;
  } catch (const MakedicException &e) {
    std::cerr << "MakedicGenerator: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
