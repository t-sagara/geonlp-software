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
}
