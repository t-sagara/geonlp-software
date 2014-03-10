///
/// @file
/// @brief 解析辞書クラスDictionaryの実装
/// @author 国立情報学研究所
///
/// Copyright (c)2010-2013, NII
///
#include <sstream>
#include <boost/regex.hpp>
#include "Dictionary.h"

namespace geonlp
{

  /// JSON からオブジェクトを復元する
  Dictionary Dictionary::fromJson(const std::string& json_str) throw (picojson::PicojsonException) {
    Dictionary dic;
    dic.initByJson(json_str);
    return dic;
  }

  // 必須項目が揃っていることを確認する
  bool Dictionary::isValid(std::string& err) const {
    if (this->get_identifier().length() == 0) return false;
    if (this->get_internal_id() == 0) return false;
    if (this->get_title().length() == 0) return false;
    return true;
  }

  bool Dictionary::isValid(void) const {
    std::string err("");
    return this->isValid(err);
  }

  // 空間範囲へのアクセス
  void Dictionary::set_spatial(const double lng0, const double lat0, const double lng1, const double lat1) {
    picojson::array spatial_array, latlon;
    
    latlon.clear();
    latlon.push_back((picojson::value)lng0); latlon.push_back((picojson::value)lat0);
    spatial_array.push_back((picojson::value(latlon)));

    latlon.clear();
    latlon.push_back((picojson::value)lng1); latlon.push_back((picojson::value)lat1);
    spatial_array.push_back((picojson::value(latlon)));

    this->set_value("spatial", (picojson::value)spatial_array);
  }

  void Dictionary::set_spatial(const std::pair<std::pair<double, double>, std::pair<double, double> >& v) {
    picojson::array spatial_array, latlon;
    
    latlon.clear();
    latlon.push_back((picojson::value)((v.first).first));
    latlon.push_back((picojson::value)((v.first).second));
    spatial_array.push_back((picojson::value(latlon)));

    latlon.clear();
    latlon.push_back((picojson::value)((v.second).first));
    latlon.push_back((picojson::value)((v.second).second));
    spatial_array.push_back((picojson::value(latlon)));

    this->set_value("spatial", (picojson::value)spatial_array);
  }

  void Dictionary::get_spatial(double& lng0, double& lat0, double& lng1, double& lat1) const throw (picojson::PicojsonException) {
    const picojson::value& v = this->get_value("spatial");
    std::stringstream sstr;
    sstr << "'spatial' must be a 2x2 vector of double value.";
    
    for (;;) {
      if (!v.is<picojson::array>()) break;
      const picojson::array& spatial_array = v.get<picojson::array>();
      if (spatial_array.size() != 2) break;

      const picojson::value& latlon0 = spatial_array[0];
      if (!latlon0.is<picojson::array>()) break;
      const picojson::array& latlon0_array = latlon0.get<picojson::array>();
      if (!latlon0_array.size() != 2) break;
      if (!latlon0_array[0].is<double>() || !latlon0_array[1].is<double>()) break;
      lng0 = latlon0_array[0].get<double>();
      lat0 = latlon0_array[1].get<double>();
      
      const picojson::value& latlon1 = spatial_array[1];
      if (!latlon1.is<picojson::array>()) break;
      const picojson::array& latlon1_array = latlon1.get<picojson::array>();
      if (!latlon1_array.size() != 2) break;
      if (!latlon1_array[0].is<double>() || !latlon1_array[1].is<double>()) break;
      lng1 = latlon1_array[0].get<double>();
      lat1 = latlon1_array[1].get<double>();
      return; // 正常終了
    }
    throw picojson::PicojsonException(sstr.str()); // フォーマットエラー
  }

  std::pair<std::pair<double, double>, std::pair<double, double> > Dictionary::get_spatial() const throw (picojson::PicojsonException) {
    double lng0, lat0, lng1, lat1;
    this->get_spatial(lng0, lat0, lng1, lat1);
    std::pair<std::pair<double, double>, std::pair<double, double> > r;
    (r.first).first = lng0;
    (r.first).second = lat0;
    (r.second).first = lng1;
    (r.second).second = lat1;
    return r;
  }

  // 辞書作成者コード（識別子から取得）
  std::string Dictionary::get_user_code() const throw (picojson::PicojsonException) {
    std::string identifier = this->get_identifier();
    std::string user_code("");
    boost::regex pattern_url("https?://.+/([^/]+)/([^/]+)/?$", boost::regex_constants::egrep);
    boost::regex pattern_file("file:/.+/([^/]+)/([^/]+)/([^/]+)\\.json$", boost::regex_constants::egrep);
    boost::smatch what;
    if (regex_match(identifier, what, pattern_url)) {
      user_code = std::string(what[1].first, what[1].second);
    } else if (regex_match(identifier, what, pattern_file)) {
      user_code = std::string(what[1].first, what[1].second);
    }
    return user_code;
  }

  // 辞書コード（識別子から取得）
  std::string Dictionary::get_code() const throw (picojson::PicojsonException) {
    std::string identifier = this->get_identifier();
    std::string dict_code("");
    boost::regex pattern_url("https?://.+/([^/]+)/([^/]+)/?$", boost::regex_constants::egrep);
    boost::regex pattern_file("file:/.+/([^/]+)/([^/]+)/([^/]+)\\.json$", boost::regex_constants::egrep);
    boost::smatch what;
    if (regex_match(identifier, what, pattern_url)) {
      dict_code = std::string(what[2].first, what[2].second);
    } else if (regex_match(identifier, what, pattern_file)) {
      dict_code = std::string(what[2].first, what[2].second);
    }
    return dict_code;
  }

  // ファイルパス（識別子から取得）
  std::string Dictionary::get_path() const throw (picojson::PicojsonException) {
    std::string identifier = this->get_identifier();
    std::string path("");
    boost::regex pattern_file("file:/(.+\\.json)$", boost::regex_constants::egrep);
    boost::smatch what;
    if (regex_match(identifier, what, pattern_file)) {
      path = std::string(what[1].first, what[1].second);
    }
    return path;
  }

}
