/*
 * GeonlpResolver::parse のユニットテスト
 */

#include <iostream>
#include <sstream>
#include "GeonlpService.h"

int main(int argc, char** argv) {
  geonlp::ServicePtr service = geonlp::createService();
  picojson::array params;
  picojson::value v;
  picojson::ext p;

  for (;;) {
    std::string line;
    std::getline(std::cin, line);
    if (line.length() == 0) {
      //line = "目黒区の施設：\n住所\n駒場４－６－１\n";
      //line = "住所\n\n日本橋１－２－３ 日本橋プラザビル　2Ｆ\n\n南海難波駅より徒歩5分。\n";
      //line = "さいたま市の常盤小に行く。";
      //line = "長野新潟など中日本でも強振が\n東海等に飛び火しなければ良いが";
      line = "日本橋１－２－３（大阪上本町駅より徒歩5分）";
      //line = "さいたま市の常盤小に行く。";
    }

    // parse
    params.clear();
    params.push_back(picojson::value(line));
    params.push_back(picojson::value(picojson::ext("{\"threshold\":0,\"geocoding\":true,\"show-candidate\":false}")));
    v = service->parse(params);
    std::cout << v.serialize() << std::endl << std::endl;
  }

}
