///
/// @file
/// @brief プロファイルクラスの実装。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#include "Profile.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include "config.h"
#include "stdio.h"

static int is_file_exists(const std::string& filename) {
  FILE* fp = fopen((char*)filename.c_str(), "r");
  if (!fp) return 0;
  fclose(fp);
  return 1;
}

namespace geonlp {

  // プロファイルのパスを探す
  std::string Profile::searchProfile(const std::string& basename) {
    std::string filename;
    std::string rcfilepath = "";
    std::string ext(PROFILE_FILE_EXT);
    if (basename.substr(basename.length() - ext.length()) != ext)
      filename = basename + ext;
    else 
      filename = basename;

    std::string fname = filename;
    if (is_file_exists(fname)) {
      rcfilepath = fname;
    } else {
      fname = std::string(PROFILE_DEFAULT_DIR_PATH) + "/" +  filename;
      if (is_file_exists(fname)) {
	rcfilepath = fname;
      } else {
	fname = std::string("/etc/") + filename;
	if (is_file_exists(fname)) {
	  rcfilepath = fname;
	} else {
	  fname = std::string("/usr/etc/") + filename;
	  if (is_file_exists(fname)) {
	    rcfilepath = fname;
	  } else {
	    fname = std::string("/usr/local/etc/") + filename;
	    if (is_file_exists(fname)) {
	      rcfilepath = fname;
	    }
	  }
	}
      }
    }
    return rcfilepath;
  }

  // プロファイルを読み込む
  void Profile::load(const std::string& f) throw(std::runtime_error) {
    filepath = f;
    try {
      boost::property_tree::ptree prop;
      read_ini(filepath, prop);
			
      formatter = prop.get<std::string>("formatter", "");

      // suffix
      suffix.clear();
      suffix_str = prop.get<std::string>("suffix", "");
      std::vector<std::string> suffix_data_strs;
      boost::split(suffix_data_strs, suffix_str, boost::is_any_of("|"));

      for (std::vector<std::string>::iterator it = suffix_data_strs.begin(); 
	   it != suffix_data_strs.end(); it++) {
	std::vector<std::string> v;
	boost::split(v, (*it), boost::is_any_of(","));
	if (v.size() == 3) {
	  Suffix s(v[0], v[1], v[2]);
	  suffix.push_back(s);
	}
      }
			
      // spatial
      spatial.clear();
      spatial_str = prop.get<std::string>("spatial", "");
      boost::split(spatial, spatial_str, boost::is_any_of("|"));

      // dictionary
      dictionary.clear();
      dictionary_str = prop.get<std::string>("dictionary", "");
      if (dictionary_str.empty()) dictionary_str = ""; // 無指定の場合はすべての辞書を利用
      boost::split(dictionary, dictionary_str, boost::is_any_of("|"));

      // ne_class
      ne_class.clear();
      ne_class_str = prop.get<std::string>("ne_class", "");
      if (ne_class_str.empty()) ne_class_str = ".*"; // 無指定の場合はすべてのクラスを利用
      boost::split(ne_class, ne_class_str, boost::is_any_of("|"));

      // address_regex
      std::string address_regex_str = prop.get<std::string>("address_regex", "^$");
      address_regex = boost::regex(address_regex_str);

      // データファイルディレクトリを取得
      // 最後が '/' で終わっていない場合、追加する
      data_dir = prop.get<std::string>("data_dir", "");
      if (data_dir.empty()) throw std::runtime_error("data_dir property is not set in the profile.");
      if (data_dir.at(data_dir.length() - 1) != '/') data_dir += "/";

      // log_dir
      log_dir = prop.get<std::string>("log_dir", "");
      if (log_dir.empty()) log_dir = "";
      else if (log_dir.at(log_dir.length() - 1) != '/') log_dir += "/";

#ifdef HAVE_LIBDAMS
      // dams_path
      dams_path = prop.get<std::string>("dams_path", "");
      if (dams_path.empty()) {
	// GeoNLP のライブラリパスを探す
	dams_path = this->get_data_dir() + "dams";
	if (!is_file_exists(dams_path + ".dat")) dams_path = "";

	// DAMS のデフォルトパスを探す
	dams_path = std::string(damswrapper::default_dic_path()) + "dams";
	if (!is_file_exists(dams_path + ".dat")) dams_path = "";
      }
      if (dams_path.empty()) {
	std::string msg = std::string("'dams_path' property is not set in the profile, and DAMS dictionary files can't be found in default paths (") + this->get_data_dir() + "dams.dat, " + damswrapper::default_dic_path() + "dams.dat).";
	throw std::runtime_error(msg);
      }
#endif /* HAVE_LIBDAMS */

    } catch (std::runtime_error& e) {
      throw e;
    }
  }

}
