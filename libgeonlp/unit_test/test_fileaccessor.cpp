/*
 * FileAccessor.cpp のユニットテスト
 */

#include <iostream>
#include <sstream>
#include "FileAccessor.h"

int main(int argc, char** argv) {
  geonlp::Geoword geoword;
  geonlp::Dictionary dictionary;
  geonlp::Wordlist wordlist;
  geonlp::DBAccessor dba("dba.rc");
  dba.open();

  // std::stringstream sstr_json;

  // データベース初期化
  dba.clearDictionaries();
  dba.clearGeowords();

  // ファイルアクセス
  geonlp::FileAccessor fa(dba);
  try {
    fa.importDictionaryCSV("test/pref.csv", "test/pref.json");
  } catch (picojson::PicojsonException e) {
    std::cerr << "Exception occurred while reading 'test/pref.json', " << e.what() << std::endl;
  }
  try {
    fa.importDictionaryCSV("test/city.csv", "test/city.json");
  } catch (picojson::PicojsonException e) {
    std::cerr << "Exception occurred while reading 'test/city.json', " << e.what() << std::endl;
  }
  try {
    fa.importDictionaryCSV("test/oaza.csv", "test/oaza.json");
  } catch (picojson::PicojsonException e) {
    std::cerr << "Exception occurred while reading 'test/oaza.json', " << e.what() << std::endl;
  }
  try {
    fa.importDictionaryCSV("test/station.csv", "test/station.json");
  } catch (picojson::PicojsonException e) {
    std::cerr << "Exception occurred while reading 'test/station.json', " << e.what() << std::endl;
  }

  // 単語リスト更新
  dba.updateWordlists();

  // 「東京都」を持つエントリを検索
  dba.findGeowordById("ivHFzZ", geoword);
  std::cout << "findGeowordById\t" << geoword.toJson() << std::endl;
  dba.findGeowordByDictionaryIdAndEntryId(1, "1977", geoword);
  std::cout << "findGeowordByDictionaryIdAndEntryId\t" << geoword.toJson() << std::endl;
  
  dba.findDictionaryById(1, dictionary);
  std::cout << "findDictionaryById\t" << dictionary.toJson() << std::endl;
  
  dba.findWordlistBySurface("青森", wordlist);
  std::cout << "findWordlist\t" << wordlist.toString() << std::endl;

  dba.findWordlistBySurface("塩竃", wordlist);
  std::cout << "findWordlist\t" << wordlist.toString() << std::endl;

  dba.findWordlistBySurface("一ツ橋", wordlist);
  std::cout << "findWordlist\t" << wordlist.toString() << std::endl;

  std::cout << "---- findGeowordListBySurface ----" << std::endl;
  std::vector<geonlp::Geoword> geowords;
  dba.findGeowordListBySurface("一ツ橋", geowords);
  for(std::vector<geonlp::Geoword>::iterator it = geowords.begin(); it != geowords.end(); it++) {
    std::cout << (*it).get_typical_name() << "\t" << (*it).toJson() << std::endl;
  }
  
  dba.close();
}
