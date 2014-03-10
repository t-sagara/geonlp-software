/*
 * DBAccessor.cpp のユニットテスト
 */

#include <iostream>
#include <sstream>
#include "DBAccessor.h"

int main(int argc, char** argv) {
  geonlp::DBAccessor dba("dba.rc");
  dba.open();
  std::stringstream sstr_json;

  // 辞書の登録
  // fromJson 
  sstr_json.str("");
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
  geonlp::Dictionary dic_in;
  dic_in.initByJson(sstr_json.str());

  std::vector<geonlp::Dictionary> dictionaries;
  dictionaries.push_back(dic_in);
  dba.clearDictionaries();
  dba.setDictionaries(dictionaries);

  // 地名語の登録
  // fromJson
  sstr_json.str("");
  sstr_json << "{"				 \
	    << "\"geonlp_id\":\"c4ca4238\", "	 \
            << "\"dictionary_id\":1, "           \
	    << "\"entry_id\":\"1\", "		 \
	    << "\"body\":\"札幌\", "			  \
    	    << "\"suffix\":[\"飛行場\", \"空港\"], "			\
	    << "\"kana\":\"さっぽろ\", "				\
	    << "\"suffix_kana\":[\"ひこうじょう\", \"くうこう\"], "	\
	    << "\"hypernym\":[\"日本\", \"北海道\"], "			\
	    << "\"ne_class\":\"Airport\", "				\
	    << "\"priority_score\":0, "					\
	    << "\"code\":{\"IATA\":\"OKD\",\"ICAO\":\"RJCO\"}, "	\
	    << "\"latitude\":\"43.1175\", "				\
	    << "\"longitude\":\"141.3814\", "				\
	    << "\"address\":\"札幌市東区丘珠町\", "			\
	    << "\"citycode\":\"01103\", "                               \
            << "\"runway\":\"1500x45\", "                               \
            << "\"type\":\"military-civilian\"}"                        \
	    << "}";
  geonlp::Geoword geo_in;
  geo_in = geonlp::Geoword::fromJson(sstr_json.str());
  std::cout << geo_in.toJson() << std::endl;
  
  std::vector<geonlp::Geoword> geowords;
  geowords.push_back(geo_in);
  dba.clearGeowords();
  dba.setGeowords(geowords);

  // getDictionaries();
  std::map<int, geonlp::Dictionary> dics = dba.getDictionaryList();
  for (std::map<int, geonlp::Dictionary>::iterator it = dics.begin(); it != dics.end(); it++) {
    std::cout << (*it).first << "\t" << (*it).second.toJson() << std::endl;
  }

  // update geoword
  geo_in.set_value("type", "civilian");
  std::cout << geo_in.toJson() << std::endl;
  geowords.clear();
  geowords.push_back(geo_in);
  dba.setGeowords(geowords);
  
}
