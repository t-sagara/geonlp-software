///
/// @file
/// @brief Fileアクセスクラス FileAccessorの実装
/// @author 国立情報学研究所
///
/// Copyright (c)2010-2013, NII
///
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/regex.hpp>
#include "Geoword.h"
#include "Dictionary.h"
#include "FormatException.h"
#include "FileAccessor.h"
#include "CSVReader.h"

namespace geonlp
{
  /// @brief 辞書 CSV ファイルから地名語と辞書情報を読み込む
  ///
  /// @arg @c csvfilename   CSV ファイル名（地名語データ）
  /// @arg @c jsonfilename  JSON ファイル名（辞書データ）
  /// @return 読み込んだ地名語の件数
  /// @exception SqliteNotInitaizliedException Sqlite3 が初期化されていない
  /// @exception SqliteErrException Sqlite3 の処理中にエラーが発生
  int FileAccessor::importDictionaryCSV(const std::string& csvfilename, const std::string& jsonfilename) const {
    std::string err = "";
    
    // 辞書データ読み込み
    std::ifstream fs_json(jsonfilename.c_str());
    if (!fs_json.is_open()) {
      std::ostringstream sstr;
      sstr << "Dictionary Data (JSON) file is not readable : " << jsonfilename;
      throw std::runtime_error(sstr.str());
    }
    /// c++ の全部読み込みは少しトリッキー
    int filesize = 0;
    fs_json.seekg(0, std::ios::end);
    filesize = fs_json.tellg();
    fs_json.seekg(0, std::ios::beg);
    char* buf = new char[filesize + 1];
    fs_json.read(buf, filesize);
    buf[filesize] = '\0';
    std::string json_str(buf, filesize);
    delete[] buf;

    std::cerr << "-----\n" << json_str << "-----\n" << std::endl;
    geonlp::Dictionary dic_in;
    dic_in.initByJson(json_str);
    if (!dic_in.isValid(err)) {
      std::ostringstream sstr;
      sstr << "Dictionary Data (JSON) is not valid. " << err;
      throw geonlp::FormatException(sstr.str());
    }
    int dictionary_id = dic_in.get_internal_id();  // 辞書ID

    // 地名語データ読み込み
    std::fstream fs_csv(csvfilename.c_str(), std::ios::in);
    if (!fs_csv.is_open()) return 0;
    CSVReader csv(fs_csv);
    int lineno = 0;
    std::vector<std::string> fields;
    std::vector<std::string> tokens;
    Geoword geoword_in;
    std::vector<Geoword> geowords;
    while (!csv.Read(tokens)) {
      geoword_in.clear();
      if (lineno == 0) {
	// 見出し行
	for (unsigned int i = 0; i < tokens.size(); i++) {
	  fields.push_back(tokens[i]);
	}
      } else {
	// データ行
	for (unsigned int i = 0; i < tokens.size(); i++) {
	  // 複数可のフィールドはフィールド名を明示的に
	  // 指定することで、文字列が分割され、配列として登録される
	  if (fields[i] == "prefix") {
	    geoword_in.set_prefix(tokens[i]);
	  } else if (fields[i] == "suffix") {
	    geoword_in.set_suffix(tokens[i]);
	  } else if (fields[i] == "prefix_kana") {
	    geoword_in.set_prefix_kana(tokens[i]);
	  } else if (fields[i] == "suffix_kana") {
	    geoword_in.set_suffix_kana(tokens[i]);
	  } else if (fields[i] == "hypernym") {
	    geoword_in.set_hypernym(tokens[i]);
	  } else if (fields[i] == "code") {
	    geoword_in.set_code(tokens[i]);
	  } else {
	    geoword_in.set_value(fields[i], tokens[i]);
	  }
	}
	geoword_in.set_dictionary_id(dictionary_id);
	if (geoword_in.isValid(err)) {
	  geowords.push_back(geoword_in);
	}
      }
      lineno++;
    }
    std::vector<Dictionary> dictionaries;
    dictionaries.push_back(dic_in);
    this->_dbap->setDictionaries(dictionaries);
    this->_dbap->setGeowords(geowords);
    return geowords.size();
  }

}
