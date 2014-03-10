/*
 * Dictionary.cpp のユニットテスト
 */

#include <iostream>
#include <sstream>
#include "Dictionary.h"

int main(int argc, char** argv) {
  geonlp::Dictionary dictionary;
  
  dictionary.set_identifier("https://geonlp.ex.nii.ac.jp/dictionary/admin/airport");
  dictionary.set_internal_id(1);
  dictionary.set_creator("管理者");
  dictionary.set_title("国土数値情報 空港データ");
  dictionary.set_description("全国の公共用空港・飛行場について整備された国土数値情報空港データより、標点位置を抽出した地名辞書データ。\n種別（空港整備法に基づく第一種～第三種空港、共用飛行場等）、名称、設置者・管理者、特定飛行場指定状況の情報を含む。");
  dictionary.set_source("http://nlftp.mlit.go.jp/ksj/gml/datalist/KsjTmplt-C28-v2_2.html");
  dictionary.set_spatial(127.679630, 26.213300, 141.347020, 43.064110);
  dictionary.set_report_count(24);
  dictionary.set_issued("2013-06-11");
  dictionary.set_modified("2013-06-14");
  dictionary.set_icon("https://geonlp.ex.nii.ac.jp/icon/user/admin/13Yq6j.png");
  dictionary.set_url("https://geonlp.ex.nii.ac.jp/dictionary/admin/japan-airport/%E6%97%A5%E6%9C%AC%E3%81%AE%E7%A9%BA%E6%B8%AF_20130613_u.zip");
  dictionary.set_record_count(99);
  dictionary.set_value("downloaded_count", 1);
  std::cout << dictionary.toJson() << std::endl;

  // fromJson
  std::stringstream sstr_json;
  sstr_json << "{"							\
	    << "\"identifier\":\"https://geonlp.ex.nii.ac.jp/dictionary/admin/pref2013\","  \
	    << "\"internal_id\":1," \
	    << "\"creator\":\"管理者\"," \
	    << "\"title\":\"都道府県\"," \
	    << "\"description\":\"1都1道2府43県の情報（1972年沖縄復帰後）\"," \
	    << "\"spatial\":[[122.93361, 20.42528], [153.98639, 45.55778]]," \
	    << "\"subject\":[ \"State/Prefecture\" ]," \
	    << "\"issued\":\"2001-01-01 00:00:00\"," \
	    << "\"modified\":\"2001-01-01 00:00:00\"," \
	    << "\"report_count\":214," \
	    << "\"record_count\":47," \
	    << "\"icon\":\"https://geonlp.ex.nii.ac.jp/icon/user/admin/rNSSrn.png\"," \
	    << "\"url\":\"https://geonlp.ex.nii.ac.jp/dictionary/admin/pref2013/%E9%83%BD%E9%81%93%E5%BA%9C%E7%9C%8C_20130611_u.zip\" "\
	    << "}";

  std::cout << "Input JSON string ---\n" << sstr_json.str() << std::endl;
  geonlp::Dictionary dic_in;
  dic_in.initByJson(sstr_json.str());
  std::cout << "Imported Dictionary object ---\n" <<  dic_in.toJson() << std::endl;

  // get_user_code
  std::cout << "get_user_code() ---\n" << dic_in.get_user_code() << std::endl;

  // get_code
  std::cout << "get_code() ---\n" << dic_in.get_code() << std::endl;

  // get_path
  std::cout << "get_path() ---\n" << dic_in.get_path() << std::endl;
}
