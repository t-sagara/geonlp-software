/*
 * geoword.cpp のユニットテスト
 */

#include <iostream>
#include <sstream>
#include "Geoword.h"

int main(int argc, char** argv) {
  geonlp::Geoword geo;
  
  std::vector<std::string> suffix;
  suffix.push_back("飛行場");
  suffix.push_back("空港");
  
  std::vector<std::string> suffix_kana;
  suffix_kana.push_back("ひこうじょう");
  suffix_kana.push_back("くうこう");

  std::vector<std::string> hypernym;
  hypernym.push_back("日本");
  hypernym.push_back("北海道");
  
  std::map<std::string, std::string> code;
  code.insert(std::map<std::string, std::string>::value_type("IATA", "OKD"));
  code.insert(std::map<std::string, std::string>::value_type("ICAO", "RJCO"));

  std::map<std::string, std::string> options;
  options.insert(std::map<std::string, std::string>::value_type("runway", "1500x45"));
  options.insert(std::map<std::string, std::string>::value_type("type", "military-civilian"));
  options.insert(std::map<std::string, std::string>::value_type("country", "JP"));
  options.insert(std::map<std::string, std::string>::value_type("prefcode", "01"));
  options.insert(std::map<std::string, std::string>::value_type("citycode", "01103"));

  geo.set_geonlp_id("c4ca4238");
  geo.set_entry_id("1");
  geo.set_body("札幌");
  // geo.set_suffix(suffix);
  geo.set_suffix("飛行場/空港");
  geo.set_body_kana("さっぽろ");
  // geo.set_suffix_kana(suffix_kana);
  geo.set_suffix_kana("ひこうじょう/くうこう");
  // geo.set_hypernym(hypernym);
  geo.set_hypernym("日本/北海道");
  geo.set_ne_class("Airport");
  geo.set_priority_score(0);
  // geo.set_code(code);
  geo.set_code("IATA:OKJ/ICAO:RJCO");
  geo.set_latitude("43.1175");
  geo.set_longitude("141.381389");
  geo.set_address("札幌市東区丘珠町");
  for (std::map<std::string, std::string>::iterator it = options.begin(); it != options.end(); it++) {
    geo.set_value((*it).first, (*it).second);
  }

  std::cout << geo.toJson() << std::endl;

  // fromJson
  std::stringstream sstr_json;
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
  std::cout << "Input JSON string ---\n" << sstr_json.str() << std::endl;
  geonlp::Geoword geo_in;
  geo_in = geonlp::Geoword::fromJson(sstr_json.str());
  std::cout << "Imported GEO object ---\n" <<  geo_in.toJson() << std::endl;

  // get_values by key
  std::string geonlp_id = geo_in.get_geonlp_id();
  suffix = geo_in.get_suffix();
  int priority_score = geo_in.get_priority_score();
  std::string runway = geo_in.get_value("runway").to_str();
  code = geo_in.get_code();
  for (std::map<std::string, std::string>::iterator it = code.begin(); it != code.end(); it++) {
    std::cout << (*it).first << "\t" << (*it).second << std::endl;
  }

  // getGeoJson
  std::string geojson_str = geo_in.getGeoJson();
  std::cout << "GEOJSON:\n" << geojson_str << std::endl;
  
}
