/*
 * GeonlpResolver のユニットテスト
 */

#include <iostream>
#include <sstream>
#include "GeonlpService.h"

int main(int argc, char** argv) {
  geonlp::ServicePtr service = geonlp::createService("dba.rc");
  picojson::array params;
  picojson::value v;
  picojson::ext p;

#if 0
  // debug
  std::cout << "\ndebug ------------------- " << std::endl;
  p.initByJson("{\"method\":\"geonlp.parse\", \"params\":[\"この地震による津波の心配はありません。発生時刻 10月5日 14時17分震源地 岩手県沖位置 緯度 北緯40.2度経度 東経142度震源 マグニチュード M4.8深さ 約20km震度 都道府県 地域震度市町村震度震度4 岩手 岩手県沿岸北部野田村震度3 岩手  久慈市 田野畑村 普代村震度2 青森 青森県津軽北部青森県三八上北外ヶ浜町 八戸市 七戸町 三戸町 五戸町 青森南部町 階上町 おいらせ町岩手 岩手県沿岸南部岩手県内陸北部岩手県内陸南部宮古市 岩泉町 岩手洋野町 陸前高田市 釜石市 盛岡市 滝沢村 八幡平市 軽米町 矢巾町 花巻市 遠野市 一関市宮城 宮城県北部宮城県中部気仙沼市 涌谷町 栗原市 登米市 石巻市震度1 青森 青森県下北蓬田村 十和田市 三沢市 野辺地町 六戸町 東北町 東通村岩手  山田町 大船渡市 住田町 大槌町 二戸市 雫石町 葛巻町 岩手町 一戸町 九戸村 紫波町 北上市 金ケ崎町 平泉町 藤沢町 西和賀町 奥州市宮城 宮城県南部宮城加美町 南三陸町 宮城美里町 大崎市 名取市 岩沼市 大河原町 丸森町 仙台青葉区 東松島市 松島町 利府町秋田 秋田県沿岸南部秋田県内陸北部秋田県内陸南部由利本荘市 にかほ市 北秋田市 横手市 湯沢市 大仙市山形 山形県庄内酒田市 庄内町\"], \"id\":\"debug\"}");
  v = service->proc(picojson::value(p));
  std::cout << v.serialize() << std::endl;

  // era
  std::cout << "\nera --------------------- " << std::endl;
  p.initByJson("{\"method\":\"geonlp.parse\", \"params\":[\"今日は平成25年7月31日です。\",{\"show-candidate\":true,\"threshold\":0}], \"id\":\"era\"}");
  v = service->proc(picojson::value(p));
  std::cout << v.serialize() << std::endl;

  // person name
  std::cout << "\nperson --------------------- " << std::endl;
  p.initByJson("{\"method\":\"geonlp.parse\", \"params\":[\"開発者は相良毅です。\",{\"show-candidate\":true,\"threshold\":0}], \"id\":\"person\"}");
  v = service->proc(picojson::value(p));
  std::cout << v.serialize() << std::endl;

  // adjunct
  std::cout << "\nadjunct --------------------- " << std::endl;
  p.initByJson("{\"method\":\"geonlp.parse\", \"params\":[\"札幌ラーメンを食べに行く。\",{\"show-candidate\":true,\"threshold\":0}], \"id\":\"adjunct\"}");
  v = service->proc(picojson::value(p));
  std::cout << v.serialize() << std::endl;
#endif

  // proc
  std::cout << "\nproc --------------------- " << std::endl;
  p.initByJson("{\"method\":\"geonlp.parse\", \"params\":[\"上溝公民館は相模原市中央区上溝7-7-17にあります。\",{\"geocoding\":\"simple\"}], \"id\":\"proc\"}");
  v = service->proc(picojson::value(p));
  std::cout << v.serialize() << std::endl;

  // resolve
  std::cout << "\nresolve --------------------- " << std::endl;
  p.initByJson("{\"method\":\"geonlp.parse\", \"params\":[\"京王多摩センターから新宿駅までは特急で27分です。港区にいます。\",{\"show-candidate\":true,\"threshold\":0}], \"id\":\"resolve\"}");
  v = service->proc(picojson::value(p));
  std::cout << v.serialize() << std::endl;

  // getDictionaryInfo
  std::cout << "\nproc (getDictionaryInfo) --- " << std::endl;
  p.initByJson("{\"method\":\"getDictionaryInfo\",\"params\":[[1,2]],\"id\":7}");
  v = service->proc(picojson::value(p));
  std::cout << v.serialize() << std::endl;

  // parse
  std::cout << "\nparse (single) ----------- " << std::endl;
  //  p.initByJson("\"情報試作室は多摩市にあります。\"");
  p.initByJson("\"今日は塩竃市に行きます\"");
  params.clear();
  params.push_back(picojson::value(p));
  p.initByJson("{\"show-candidate\":true}");
  params.push_back(picojson::value(p));
  v = service->parse(params);
  std::cout << v.serialize() << std::endl;

  // parse (multiple sentence)
  std::cout << "\nparse (multiple) ----------- " << std::endl;
  p.initByJson("[\"国立情報学研究所は千代田区にあります。\",\"情報試作室は多摩市にあります。\"]");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->parse(params);
  std::cout << v.serialize() << std::endl;

  // parseStructured
  std::cout << "\nparseStructured ----------- " << std::endl;
  p.initByJson("[{\"organization\":{\"surface\":\"NII\",\"name\":\"国立情報学研究所\",\"tel\":\"03-4212-2000（代表）\"}},\"は千代田区にあります。\",\"情報試作室は多摩市にあります。\"]");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->parseStructured(params);
  std::cout << v.serialize() << std::endl;

  // search
  std::cout << "\nsearch ----------- " << std::endl;
  p.initByJson("\"たまし\"");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->search(params);
  std::cout << v.serialize() << std::endl;

  // getGeoInfo
  std::cout << "\ngetGeoInfo ----------- " << std::endl;
  p.initByJson("\"y5rfZ0\"");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->getGeoInfo(params);
  std::cout << v.serialize() << std::endl;

  // getGeoInfo (multiple)
  std::cout << "\ngetGeoInfo (multiple) ----------- " << std::endl;
  p.initByJson("[\"y5rfZ0\", \"invalid\", \"X4gFiM\"]");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->getGeoInfo(params);
  std::cout << v.serialize() << std::endl;

  // getDictionaries
  std::cout << "\ngetDictionaries ----------- " << std::endl;
  params.clear();
  v = service->getDictionaries(params);
  std::cout << v.serialize() << std::endl;

  // getDictionaryInfo
  std::cout << "\ngetDictionaryInfo ----------- " << std::endl;
  p.initByJson("1");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->getDictionaryInfo(params);
  std::cout << v.serialize() << std::endl;

  // getDictionaryInfo (multiple)
  std::cout << "\ngetDictionaryInfo (multiple) ----------- " << std::endl;
  p.initByJson("[1,\"2\"]");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->getDictionaryInfo(params);
  std::cout << v.serialize() << std::endl;
  
  // addressGeocoding
  std::cout << "\naddressGeocoding (\"geocoding\":true)----------- " << std::endl;

  //p.initByJson("\"榛原郡相良町相良２７５番地\"");
  p.initByJson("\"千代田区一ツ橋二丁目１番２号\"");
  params.clear();
  params.push_back(picojson::value(p));
  p.initByJson("{\"geocoding\":true}");
  params.push_back(picojson::value(p));
  v = service->addressGeocoding(params);
  std::cout << v.serialize() << std::endl;

  std::cout << "\naddressGeocoding (\"geocoding\":\"simple\")----------- " << std::endl;
  p.initByJson("\"千代田区一ツ橋二丁目１番２号\"");
  params.clear();
  params.push_back(picojson::value(p));
  p.initByJson("{\"geocoding\":\"simple\"}");
  params.push_back(picojson::value(p));
  v = service->addressGeocoding(params);
  std::cout << v.serialize() << std::endl;

  std::cout << "\naddressGeocoding (\"geocoding\":\"full\")----------- " << std::endl;
  p.initByJson("\"千代田区一ツ橋二丁目１番２号\"");
  params.clear();
  params.push_back(picojson::value(p));
  p.initByJson("{\"geocoding\":\"full\"}");
  params.push_back(picojson::value(p));
  v = service->addressGeocoding(params);
  std::cout << v.serialize() << std::endl;

  std::cout << "\naddressGeocoding (\"geocoding\":\"normal\")----------- " << std::endl;
  p.initByJson("\"千代田区一ツ橋二丁目１番２号\"");
  params.clear();
  params.push_back(picojson::value(p));
  p.initByJson("{\"geocoding\":\"normal\"}");
  params.push_back(picojson::value(p));
  v = service->addressGeocoding(params);
  std::cout << v.serialize() << std::endl;

  // addressGeocoding (multiple)
  std::cout << "\naddressGeocoding (multiple) ----------- " << std::endl;
  p.initByJson("[\"千代田区一ツ橋2-1-2\", \"多摩市永山一の五\"]");
  params.clear();
  params.push_back(picojson::value(p));
  v = service->addressGeocoding(params);
  std::cout << v.serialize() << std::endl;

}
