/*
 * test_util のユニットテスト
 */

#include <iostream>
#include <boost/regex.hpp>
#include "Util.h"

int main(int argc, char** argv) {
  // 東京
  double lat0 = 35.65500;
  double lon0 = 139.74472;
  // 筑波
  double lat1 = 36.10056;
  double lon1 = 140.09111;
  // 距離
  double dist = geonlp::Util::latlonDist(lat0, lon0, lat1, lon1);
  // 国土地理院サイトによる正解との誤差
  double delta_gsi = dist - 58.501873;

  // 結果表示
  std::cout << "距離：" << dist << ", 国土地理院サイトとの誤差：" << delta_gsi << std::endl;


  // URL 分解のテスト
  std::string url = "http://www.info-proto.com/foo.cgi?key=20141009&t=abc";
  std::vector<std::string> result;
  if (geonlp::Util::split_url(url, result)) {
    std::cerr << "url:    " << result[0] << std::endl;
    std::cerr << "schema: " << result[1] << std::endl;
    std::cerr << "server: " << result[2] << std::endl;
    std::cerr << "port:   " << result[3] << std::endl;
    std::cerr << "fullpath: " << result[4] << std::endl;
    std::cerr << "path:   " << result[5] << std::endl;
    std::cerr << "param:  " << result[6] << std::endl;
  } else {
    std::cout << "URL が解析できませんでした。" << std::endl;
  }
}
