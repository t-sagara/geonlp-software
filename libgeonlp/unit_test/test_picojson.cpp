/*
 * picojsonExt.cpp のユニットテスト
 */

#include <iostream>
#include <boost/regex.hpp>
#include "picojsonExt.h"

int main(int argc, char** argv) {
  picojson::ext e;
  std::string key("キー");
  std::string str("テスト|");
  boost::regex ex("|");
  e.set_value("キー", "テスト/", boost::regex("/"));
  e.set_value("整数", 3);
  e.set_value("実数", 4.1);
  e.set_value("空", picojson::null());
  std::cout << e.toJson() << std::endl;
  
  e.initByJson("{\"空\":null}");
  std::cout << e.toJson() << std::endl;
}
