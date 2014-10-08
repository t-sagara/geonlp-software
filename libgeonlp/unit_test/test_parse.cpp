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
      //line = "日本橋１－２－３（大阪上本町駅より徒歩5分）";
      //line = "さいたま市の常盤小に行く。";
      //line = "永山駅で降りる。";
      line = "2014年8月20日午前3時20分から40分にかけて、局地的な短時間大雨によって安佐北区可部、安佐南区八木・山本・緑井などの住宅地後背の山が崩れ、同時多発的に大規模な土石流が発生した。";
    }

    // parse
    params.clear();
    params.push_back(picojson::value(line));
    // params.push_back(picojson::value(picojson::ext("{\"threshold\":0,\"geocoding\":true,\"show-candidate\":false}")));
    // params.push_back(picojson::value(picojson::ext("{\"threshold\":0,\"geocoding\":true,\"show-candidate\":false,\"topic-radius\":20.0,\"topic-point\":[35.681382,139.766084]}")));
    // params.push_back(picojson::value(picojson::ext("{\"threshold\":0,\"geocoding\":true,\"show-candidate\":true,\"dist-server\":{\"host\":\"localhost\",\"port\":80,\"path\":\"/test.php\",\"method\":\"getHiroshimaWeight\",\"option\":null}}")));
    params.push_back(picojson::value(picojson::ext("{\"threshold\":0,\"geocoding\":true,\"show-candidate\":false,\"dist-server\":{\"url\":\"http://localhost:80/test.php\",\"method\":\"getHiroshimaWeight\",\"option\":null}}")));
    v = service->parse(params);
    std::cout << v.serialize() << std::endl << std::endl;
  }

}
