#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#include "MakedicGenerator.h"
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <dirent.h>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include "GeowordJudge.h"
#include "MakedicItem.h"
#include "MakedicCsvFileIO.h"
#include "DBAccessor.h"

#define MAX_RETRY_TIMES 20
#define SEP "{$GEOWORD}"

MakedicGenerator::MakedicGenerator(
				 const MakedicGeneratorConfiguration& conf, 
				 const MakedicGeneratorAppArgument& arg) {
  // MeCab 動作環境を取得
  std::cout << "MeCab の実行環境を調査しています。" << std::endl;
  MeCab::Tagger* tagger = MeCab::createTagger("");
  const MeCab::DictionaryInfo* d = tagger->dictionary_info();
  std::string charset(d->charset);
  std::cout << "  charset      = " << charset << std::endl;
  transform(charset.begin(), charset.end(), charset.begin(), tolower);
  if (charset != "utf-8" && charset != "utf8") {
    std::cerr << "  MeCab の辞書が UTF-8 ではありません。\n処理を中断します。" << std::endl;
    exit(1);
  }
  // "/usr/local/lib/mecab/dic/ipadic/" の部分を取得する
  std::string dic_path = std::string(d->filename);
  int last_slash_pos = dic_path.find_last_of('/');
  if (last_slash_pos != std::string::npos) {
    this->sysdic_directory_path = dic_path.substr(0, last_slash_pos);
  } else {
    this->sysdic_directory_path = dic_path;
  }
  std::cout << "  mecab_sysdic = " << this->sysdic_directory_path << std::endl;

  mecab_dict_index_path = conf.get_mecab_dict_index_path();
  example_sentences = conf.get_example_sentences();
  deleteTempFileFlag = conf.get_delete_temp_file_flag();
	
  verboseMode = arg.hasOption("v");
  //  geowordFeatureCheckMode = !arg.hasOption("c");
  profile = arg.getProfile();
  csv_output_filename = arg.getOptCsvOutputFilename();
  /*
  geodic_filepath = arg.getProfile().get_sqlite3_file();
  darts_filepath = arg.getProfile().get_darts_file();
  ipadic_filepath = arg.getProfile().get_mecab_userdic();
  */

  // 地名語の品詞ID
  if (arg.hasOption("d")) {
    // 直接品詞ID指定を最優先
    geoword_hinshi_id = arg.getOptGeowordHinshiId();
    std::cout << "  -d オプションで指定されたID（="
	      << geoword_hinshi_id
	      << "）を地名語IDとして利用します。" << std::endl;
  } else if (arg.hasOption("n")) {
    // naist-jdic の「名詞-固有名詞-地名-一般」品詞ID = "1365"
    geoword_hinshi_id = "1365";
    std::cout << "  naist-jdic を利用します（-n オプション）。" << std::endl;
  } else if (arg.hasOption("i")) {
    // IPADIC の「名詞-固有名詞-地名-一般」品詞ID = "1293"
    geoword_hinshi_id = "1293";
    std::cout << "  IPADIC を利用します（-i オプション）。" << std::endl;
  } else {
    if (sysdic_directory_path.find("ipadic") != std::string::npos) { // IPADIC
      geoword_hinshi_id = "1293";
      std::cout << "  IPADIC を利用します（dictionary path による自動判別）。" << std::endl;
    } else if (sysdic_directory_path.find("naist") != std::string::npos) { // NAIST
      geoword_hinshi_id = "1365";
      std::cout << "  naist-jdic を利用します（dictionary path による自動判別）。" << std::endl;
    } else { // MeCab を起動して調査
      MeCab::Tagger* tmp_tagger = MeCab::createTagger("-F %phl");
      std::string left_id(tmp_tagger->parse("東京"));
      delete tmp_tagger;
      int pos = left_id.find("EOS", 0);
      if (pos == std::string::npos) {
	std::cerr << "  MeCab が GeoNLP 互換辞書（IPADIC, naist-jdic）以外を利用していて、固有名詞-地名のIDが識別できません。\nMeCab が以下の値を返しました。\n" << left_id << "\n処理を中断します。" << std::endl;
	exit(1);
      }
      geoword_hinshi_id = left_id.substr(0, pos);
      if (geoword_hinshi_id == "1365") {
	std::cout << "  naist-jdic を利用します（MeCab 左文脈IDによる自動判別）。" << std::endl;
      } else if (geoword_hinshi_id == "1293") {
	std::cout << "  IPADIC を利用します（MeCab 左文脈IDによる自動判別）。" << std::endl;
      } else {	
	std::cout << "  MeCab が返したID (=" << geoword_hinshi_id << ") を地名語IDとして利用します。" << std::endl;
      }
    }
  }
	
  // 一時生成ファイルの prefix 作成
  char tmpfile_prefix_tmp[] = "/tmp/tmp_XXXXXX";
  close(mkstemp(tmpfile_prefix_tmp));
  remove(tmpfile_prefix_tmp);
  tmpfile_prefix = tmpfile_prefix_tmp;
}

void MakedicGenerator::generate(bool doUpdateWordlist) const
  throw (MakedicException) {

  std::vector<geonlp::Wordlist> wordlists;
	
  if (doUpdateWordlist) {
    // wordlist と darts ファイルを更新する
    printf("単語リストからインデックスを更新しています。\n");
    _createDartsIndex(wordlists);
  } else {
    // wordlist を DB から読みだす（更新はしない）
    printf("単語リストを読み込んでいます。\n");
    _read_db(wordlists);
  }
	
  //　終了時のCSV処理
  std::set<std::string> tmpfiles;
  std::string last_tmpfile = "";
  BOOST_SCOPE_EXIT ((&deleteTempFileFlag)(&tmpfiles)(&csv_output_filename)(&last_tmpfile)) {
    if (csv_output_filename != "" && last_tmpfile != "") {
      if (boost::filesystem::exists(csv_output_filename)) {
	boost::filesystem::remove(csv_output_filename);
      }
      // 最後の CSV ファイルをコピーする
      boost::filesystem::copy_file(last_tmpfile, csv_output_filename);
    }
    if (deleteTempFileFlag) {
      // 一時ファイルを削除する
      for (std::set<std::string>::iterator it = tmpfiles.begin(); it != tmpfiles.end(); it++) {
	remove(it->c_str());
      }
    }
  } BOOST_SCOPE_EXIT_END
	
      // ユーザ辞書に登録する地名語およびその生起コストを求める。
      std::map<std::string, long> wcosts;
  std::string user_dic_file_path = "";
  int retry_counter = 0;
  int prev_inconsistency = -1;
  while (retry_counter < MAX_RETRY_TIMES) {

    printf("コスト収束計算 %d 回目\n", retry_counter + 1);

    // MeCab の生成
    mecab_t *mecab;
    {
      std::string arg = "";
      if (retry_counter > 0) {
	arg += "--userdic=" + user_dic_file_path;
      }
      arg += " --unk-format=\"%m\t未知語,*,*,*,*,*,*,*,*\n\"";
#ifdef DEBUG
      std::cout << arg << std::endl;
#endif /* DEBUG */
      mecab = mecab_new2(arg.c_str());
      if (mecab == NULL) {
	throw MakedicException(mecab_strerror(NULL));
      }
    }

    // 地名語が 地名語として識別可能かどうか判定するための判定オブジェクトを設定する
    std::vector<GeowordJudge> judges;
    for (std::vector<std::string>::const_iterator it = example_sentences.begin(); it != example_sentences.end(); it++) {
      std::string::size_type index = (*it).find(SEP);
      if (index == std::string::npos) continue;
      std::string pre_sentence = (*it).substr(0, index);
      std::string pst_sentence = (*it).substr(index + strlen(SEP), (*it).length());
      GeowordJudge judge(mecab, pre_sentence, pst_sentence, profile);
      judges.push_back(judge);
    }

    // 見出し語毎に生起コストを求める。ただし、必要なものだけ。
    int inconsistency = 0;
    std::string surface;
    for (std::vector<geonlp::Wordlist>::iterator it_w = wordlists.begin(); it_w != wordlists.end(); it_w++) {
      geonlp::Wordlist wordlist = *it_w;
      surface = wordlist.get_surface();
			
      // 地名語を様々なパターンで形態素解析して、/^P?HB*$/ にマッチするかどうか判定する
      bool judge_result = true;
      for (std::vector<GeowordJudge>::iterator it = judges.begin(); it != judges.end(); it++) {
	try {
	  judge_result &= (*it).judge(surface);
	} catch (const GeowordJudgeFailedException &e) {
	  throw MakedicException("failed to judge as a geoword. (case 1)");
	}
	if (!judge_result) break;
      }

      // /^P?HB*$/ にマッチしない場合は、生起コストを求めてマップに登録
      if (!judge_result) {
	long wcost = -1;
	bool is_undefined = true;
	for (std::vector<GeowordJudge>::iterator it = judges.begin(); it != judges.end(); it++) {
	  try {
	    long wcost_new = (*it).cost(surface, is_undefined);
	    if (wcost_new < 0) continue; // 固有名詞として登録済み
	    if (wcost < 0) wcost = wcost_new;
	    else {
	      // 要検討
	      // if (is_undefined) wcost = std::min(wcost, wcost_new);
	      // else wcost = std::max(wcost, wcost_new);
	      wcost = std::min(wcost, wcost_new);
	    }
	  } catch (const GeowordJudgeFailedException &e) {
	    throw MakedicException("failed to judge as a geoword. (case 2)");
	  }
	}

	if (verboseMode) {
	  std::cout << surface << "\tcost= " << wcost << std::endl;
	}
	if (wcost < 0) { // 登録済み地名語
	  continue;
	}
				
	if (wcosts.find(surface) == wcosts.end()) {
	  inconsistency++;
	} else {
	  long wcost_before = wcosts[surface];
	  if (wcost_before != wcost) inconsistency++;
	}
	wcosts[surface] = wcost;
      } else {
	//      if (verboseMode) {
	//	  std::cout << surface << "\tcost= --" << std::endl;
	//	}
      }
    }

    // 各種クローズ処理
    mecab_destroy(mecab);
		
    // 辞書が収束していたらここでループを抜ける
    printf("  不整合数: % 6d 件\n", inconsistency);
    if (inconsistency == 0) break;
    if (prev_inconsistency > 0 && prev_inconsistency <= inconsistency) {
      std::cerr << "  これ以上不整合箇所を減らせないため、収束処理を終了します。" << std::endl;
      break;
    }
    prev_inconsistency = inconsistency;

    // 形態素辞書に登録するアイテムセットを作成する
    // IPADICファイルを作成する
    std::string csv_file_path = _create_tmpfile_name(".csv", retry_counter);
    {
      std::vector<MakedicItem> ipadic_items;
      ipadic_items.clear();
      for (std::vector<geonlp::Wordlist>::iterator it_w = wordlists.begin(); it_w != wordlists.end(); it_w++) {
	geonlp::Wordlist& wordlist = *it_w;
	long cost = wcosts[wordlist.get_surface()];
	if (cost == 0L) continue;
	MakedicItem item;
	_create_ipadic_item(wordlist, cost, item);
	ipadic_items.push_back(item);
      }

      MakedicCsvFileIO::write(csv_file_path, ipadic_items);
      tmpfiles.insert(csv_file_path);
      last_tmpfile = csv_file_path; // 最後に生成した CSV ファイル
    }
		
    std::string dic_file_path = _create_tmpfile_name(".dic", retry_counter);
    std::string compile_command;
    {
      std::ostringstream stream;
      stream << mecab_dict_index_path
	     << " -d "
	     << sysdic_directory_path
	     << " -u "
	     << dic_file_path
	     << " -f utf-8 -t utf-8 "
	     << csv_file_path;
      compile_command = stream.str();
      //#ifdef DEBUG
      std::cout << compile_command << std::endl;
      //#endif /* DEBUG */
    }
    std::cout << "  ユーザ辞書を作成中。\n";

    int rc = system(compile_command.c_str());
    if (rc == -1) {
      perror("Error:");
      std::cerr << compile_command << std::endl;
      throw std::runtime_error("mecab-dict-index コマンドが実行できません。");
    }
    tmpfiles.insert(dic_file_path);

    // 次ループ向け処理
    user_dic_file_path = dic_file_path;
    retry_counter++;
  }
	
  // 出力先にコピー
  if (user_dic_file_path.length() > 0) {
    std::ifstream ifs(user_dic_file_path.c_str());
    std::ofstream ofs(profile.get_mecab_userdic().c_str());
    if (!ifs) {
      throw MakedicException("生成したユーザ辞書データファイルが開けません");
    }
    if (!ofs) {
      throw MakedicException("プロファイルで指定されているユーザ辞書データファイルに書き込めません");
    }
    ofs << ifs.rdbuf() << std::flush;
    if (!ifs) {
      throw MakedicException("生成したユーザ辞書データファイルからの読み込みに失敗しました");
    }
    if (!ofs) {
      throw MakedicException("ユーザ辞書データファイルへの書き込みに失敗しました");
    }
  }
	
}

// 辞書に登録されている全見出し語（Wordlist）を取得する
void MakedicGenerator::_read_db(std::vector<geonlp::Wordlist>& wordlists) const 
  throw(MakedicException) {
  geonlp::DBAccessor dao = geonlp::DBAccessor(this->profile);
  try {
    dao.open();
  } catch (std::runtime_error& e) {
    throw MakedicException(e.what());
  }
  try {
    dao.findAllWordlist(wordlists);
		
    BOOST_SCOPE_EXIT ((&dao)) {
      dao.close();
    } BOOST_SCOPE_EXIT_END
	} catch (std::runtime_error& e) {
    throw MakedicException(e.what());
  }
}

std::string MakedicGenerator::_create_tmpfile_name(
						  const char* suffix, int count) const {
  std::ostringstream stream;
  stream << tmpfile_prefix << "__geoword";
  stream.width(2);
  stream.fill('0');
  stream << count << suffix;
  return stream.str();
}

void MakedicGenerator::_create_ipadic_item(const geonlp::Wordlist& wordlist, const long& cost, MakedicItem& item) const {
  item.set_surface(wordlist.get_surface());
  item.set_leftContextId(geoword_hinshi_id);
  item.set_rightContextId(geoword_hinshi_id);
  item.set_cost(cost);
  item.set_partOfSpeech("名詞");
  item.set_subclassification1("固有名詞");
  item.set_subclassification2("地域");
  item.set_subclassification3("一般");
  item.set_conjugatedForm("*");
  item.set_conjugationType("*");
  item.set_originalForm(wordlist.get_surface());
  item.set_yomi(wordlist.get_yomi());
  item.set_pronunciation(wordlist.get_yomi());
}

// Wordlist と Darts ファイルを更新、保存、結果を取得する
void MakedicGenerator::_createDartsIndex(std::vector<geonlp::Wordlist>& wordlists) const {
  geonlp::DBAccessor dao = geonlp::DBAccessor(this->profile);
  dao.open();
  try {
    dao.updateWordlists(wordlists);
		
    BOOST_SCOPE_EXIT ((&dao)) {
      dao.close();
    } BOOST_SCOPE_EXIT_END
	} catch (std::runtime_error& e) {
    throw MakedicException(e.what());
  }
  std::cout << wordlists.size() << "件の地名語表記を登録しました。" << std::endl;
}
