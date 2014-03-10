///
/// @file
/// @brief コンテキスト情報 Context の実装
/// @author 株式会社情報試作室
///
/// Copyright (c)2013, NII
///
#include <sstream>
#include <cmath>
#include "Context.h"

// #define CONTEXT_LOG 1 // デバッグ用、コメントアウトすると /tmp/geonlp.debug にスコア計算結果を出力する
#ifdef CONTEXT_LOG
#include <fstream>
#include <vector>
#include <string>
#endif /* CONTEXT_LOG */

// シグモイド関数
// -1.0 ≦ v ≦ 1.0
// v =  0 | x = 0
// v = -1 | x -> -inf
// v =  1 | x -> +inf
static double _sigmoid(double a, double x) {
  double v = -1.0 + 2.0 / (1.0 + exp(-a * x));
  return v;
}

namespace geonlp
{

  /// ContextRelation の実装

  // 地名語情報をコンテキスト関係に登録する
  void ContextRelation::add(const std::string& key, const std::string& geonlp_id, int n, int m) {
    if (this->_chain.count(key) > 0) {  // このキーは存在する
      std::map<std::string, std::vector<std::pair<int, int> > >& v = this->_chain[key];
      
      if (v.count(geonlp_id) > 0) { // このgeonlp_idは登録済み
	std::vector<std::pair<int, int> >& v2 = v[geonlp_id];
	v2.push_back(std::make_pair(n, m));
      } else { // この geonlp_id は未登録
	std::vector<std::pair<int, int> > v2;
	v2.push_back(std::make_pair(n, m));
	v.insert(make_pair(geonlp_id, v2));
      }
    } else { // キーも未登録
      std::map<std::string, std::vector<std::pair<int, int> > > v;
      std::vector<std::pair<int, int> > v2;
      v2.push_back(std::make_pair(n, m));
      v.insert(make_pair(geonlp_id, v2));
      this->_chain.insert(make_pair(key, v));
    }
  }

  // 地名語情報をコンテキスト関係に登録する
  void ContextRelation::add(int int_key, const std::string& geonlp_id, int n, int m) {
    std::string key;
    std::stringstream ss;
    ss << int_key;
    ss >> key;
    this->add(key, geonlp_id, n, m);
  }

  // 地名語情報をコンテキスト関係から取得する
  std::map<std::string, std::vector<std::pair<int, int> > >& ContextRelation::get(const std::string& key)
    throw (ContextException) {
    if (this->_chain.count(key) == 0) {  // このキーは存在しない
      throw ContextException(key + "がコンテキスト関係に見つかりません。");
    }
    return this->_chain[key];
  }
  
  // 地名語情報をコンテキスト関係から取得する
  std::map<std::string, std::vector<std::pair<int, int> > >& ContextRelation::get(int int_key)
    throw (ContextException) {
    std::string key;
    std::stringstream ss;
    ss << int_key;
    ss >> key;
    return this->get(key);
  }

  // @brief 地名語数を取得する（キーが文字列の場合）
  // @arg key キー
  // @arg self_id   この geonlp_id を持つ地名語はカウントしない（自分自身を除く）
  // @arg self_pos  n == self_pos となる地名語はカウントしない（自分自身を除く）
  // @arg lb  カウントする n の下限（lb 以上の n を持つエントリのみカウントする）
  // @arg hb  カウントする n の上限（hb 以下の n を持つエントリのみカウントする）
  int ContextRelation::count(const std::string& key, const std::string& self_id, int self_pos, int lb, int hb) const {
    std::map<int, int> counter; // m 番目の単語の地名語候補が key を持つ頻度
    counter.clear();

    int c = 0;
    std::map<std::string, std::map<std::string, std::vector<std::pair<int, int> > > >::const_iterator it = this->_chain.find(key);
    if (it == this->_chain.end()) return 0; // このキーを持つ地名語は存在しない
    const std::map<std::string, std::vector<std::pair<int, int> > >& v = (*it).second;
    for (std::map<std::string, std::vector<std::pair<int, int> > >::const_iterator it2 = v.begin(); it2 != v.end(); it2++) {
      const std::string& geonlp_id = (*it2).first;
      const std::vector<std::pair<int, int> >& v2 = (*it2).second;
      if (geonlp_id == self_id) continue; // 自分自身と同じ地名語はカウントしない
      for (std::vector<std::pair<int, int> >::const_iterator it3 = v2.begin(); it3 != v2.end(); it3++) {
	int n = (*it3).first;
	if ((lb < 0 || n >= lb) && (hb < 0 || n <= hb) && (n != self_pos)) {
	  // (*it2).first を geonlp_id とする地名語が
	  // lb ≦ n ≦ hb に一回以上出現した
	  if (counter.count(n) > 0) {
	    counter[n] = counter[n]++;
	  } else {
	    counter[n] = 1;
	  }
	  break;
	}
      }
    }

    // カウンタから出現頻度を計算
    for (std::map<int, int>::iterator it = counter.begin(); it != counter.end(); it++) {
      c++;
    }
    return c;
  }

  // @brief 地名語数を取得する（キーが数値の場合）
  // @arg key キー
  // @arg self_id   この geonlp_id を持つ地名語はカウントしない（自分自身を除く）
  // @arg self_pos  n == self_pos となる地名語はカウントしない（自分自身を除く）
  // @arg lb  カウントする n の下限（lb 以上の n を持つエントリのみカウントする）
  // @arg hb  カウントする n の上限（hb 以下の n を持つエントリのみカウントする）
  int ContextRelation::count(int int_key, const std::string& self_id, int self_pos, int lb, int hb) const {
    std::string key;
    std::stringstream ss;
    ss << int_key;
    ss >> key;
    return this->count(key, self_id, self_pos, lb, hb);
  }

  // コンテキスト関係のキーチェーンを空にする
  void ContextRelation::clear(void) {
    this->_chain.clear();
  }

  // 指定した n 以下の地名語をコンテキスト関係からエクスパイアする
  void ContextRelation::expire(int n) {
    std::map<std::string, std::map<std::string, std::vector<std::pair<int, int> > > >::iterator it;
    for (it = this->_chain.begin(); it != this->_chain.end(); it++) {
      std::map<std::string, std::vector<std::pair<int, int> > >& v0 = (*it).second;
      int len0 = v0.size();
      for (std::map<std::string, std::vector<std::pair<int, int> > >::iterator it0 = v0.begin(); it0 != v0.end(); it0++) {
	std::vector<std::pair<int, int> >& v = (*it0).second;
	int len = v.size();
	for (std::vector<std::pair<int, int> >::iterator it2 = v.begin(); it2 != v.end(); it2++) {
	  if ((*it2).first <= n) {
	    v.erase(it2);
	    len--;
	  }
	}
	if (len == 0) {
	  v0.erase(it0);
	  len0--;
	}
      }
      if (len0 == 0) {
	this->_chain.erase(it);
      }
    }
  }


  /// Context の実装

  void Context::clear(void) {
    this->_context_neclass.clear();
    this->_context_dictionary.clear();
    this->_context_hypernym.clear();
    this->_context_full_hypernym.clear();
    this->_context_name.clear();
    this->_nodes.clear();
    this->_selected_neclass.clear();
    this->_selected_dictionary.clear();
    this->_selected_hypernym.clear();
    this->_selected_full_hypernym.clear();
    this->_selected_name.clear();
    this->_cumulative_lat = this->_cumulative_lon = .0;
    this->_cumulative_points = 0;
  }

  // Geoword を一つコンテキスト関係に登録する
  void Context::addGeowordToContextRelations(const Geoword& geoword, int n, int m) {
    std::string geonlp_id = geoword.get_geonlp_id();
    const std::vector<std::string>& hypernyms = geoword.get_hypernym();
    this->_context_neclass.add(geoword.get_ne_class(), geonlp_id, n, m);
    this->_context_dictionary.add(geoword.get_dictionary_id(), geonlp_id, n, m);
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      this->_context_hypernym.add((*it), geonlp_id, n, m);
    }
    if (hypernyms.size() >= 2)
      this->_context_full_hypernym.add(geoword.get_value("hypernym").serialize(), geonlp_id, n, m);
    this->_context_name.add(geoword.get_typical_name(), geonlp_id, n, m);
  }

  // AddressElement を一つ空間関係に登録する
  // i.e. 重み付きで重心を取る
  void Context::addAddressElementToSpatialRelations(const picojson::value& elem, int size) {
    picojson::ext e(elem);
    int weight;
    switch (size) {
    case 1:
      weight = 10; break;
    case 2:
      weight = 5; break;
    case 3:
      weight = 1; break;
    default:
      weight = 0;
    }

    if (weight > 0) {
      int level;
      float lat, lon;
      level = e._get_int("level");
      lat   = e._get_double("latitude");
      lon   = e._get_double("longitude");
      this->_cumulative_lat += weight * level * lat;
      this->_cumulative_lon += weight * level * lon;
      this->_cumulative_points += weight * level;
    }
  }

  // Geoword を一つ空間関係に登録する
  // i.e. 重み付きで重心を取る
  void Context::addGeowordsToSpatialRelations(const picojson::array& geowords) {
    // Geoword& geoword, int size) {
    std::vector<std::pair<float, float> > latlon;
    for (picojson::array::const_iterator it = geowords.begin(); it != geowords.end(); it++) {
      Geoword geoword(*it);
      if (geoword.get_latitude().length() == 0 || geoword.get_longitude().length() == 0) continue;
      float lat, lon;
      std::stringstream is_lat(geoword.get_latitude()), is_lon(geoword.get_longitude());
      is_lat >> lat;
      is_lon >> lon;
      bool bIdentical = false;
      for (std::vector<std::pair<float, float> >::iterator it_latlon = latlon.begin(); it_latlon != latlon.end(); it_latlon++) {
	float lat_i, lon_i, dist;
	lat_i = (*it_latlon).first;
	lon_i = (*it_latlon).second;
	dist = (lat - lat_i) * (lat - lat_i) + (lon - lon_i) * (lon - lon_i);
	if (dist < 0.0001) {
	  bIdentical = true;
	  break;
	}
      }
      if (!bIdentical) latlon.push_back(std::make_pair(lat, lon));
    }
    
    int size = latlon.size();
    int weight;
    switch (size) {
    case 1:
      weight = 10; break;
    case 2:
      weight = 5; break;
    case 3:
      weight = 1; break;
    default:
      weight = 0;
    }

    if (weight > 0) {
      for (std::vector<std::pair<float, float> >::iterator it_latlon = latlon.begin(); it_latlon != latlon.end(); it_latlon++) {
	float lat_i, lon_i;
	lat_i = (*it_latlon).first;
	lon_i = (*it_latlon).second;
	this->_cumulative_lat += weight * lat_i;
	this->_cumulative_lon += weight * lon_i;
	this->_cumulative_points += weight;
      }
    }
  }

  // Geoword の出現スコアを計算する
  int Context::score(const Geoword& geoword, int n, int m) const {
    std::string geonlp_id = geoword.get_geonlp_id();
    // コンテキスト中に存在する親地名語数をカウント
    const std::vector<std::string>& hypernyms = geoword.get_hypernym();
    int nparent = 0;
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      nparent += this->_context_name.count((*it), geonlp_id, n);
    }

    // コンテキスト中に存在する子地名語数をカウント
    int nchild = this->_context_hypernym.count(geoword.get_typical_name(), geonlp_id, n);
    
    // コンテキスト中に存在する同クラス地名語数をカウント
    int nclass  = this->_context_neclass.count(geoword.get_ne_class(), geonlp_id, n);

    // コンテキスト中に存在する同辞書地名語数をカウント
    int ndictionary = this->_context_dictionary.count(geoword.get_dictionary_id(), geonlp_id, n);

    // コンテキスト中に存在する、親地名が一つでも重複する兄弟地名語数をカウント
    int nsibling = 0;
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      nsibling += this->_context_hypernym.count((*it), geonlp_id, n);
    }

    // コンテキスト中に存在する、親地名が完全に一致する兄弟地名語数をカウント
    int nfullsibling = 0;
    if (hypernyms.size() >= 2)
      nfullsibling = this->_context_full_hypernym.count(geoword.get_value("hypernym").serialize(), geonlp_id, n);

    // 重心からの距離によるスコア加算
    int spatial_bonus = 0;
    if (geoword.get_latitude().length() > 0 && geoword.get_longitude().length() > 0) {
      float clat, clon, lat, lon;
      std::istringstream is_lat(geoword.get_latitude());
      std::istringstream is_lon(geoword.get_longitude());
      is_lat >> lat;
      is_lon >> lon;
      int npoints = this->getCentroid(clat, clon);
      float square_dist = (lat - clat) * (lat - clat) + (lon - clon) * (lon - clon);
      if (square_dist < 1.0) spatial_bonus = 100;
      else spatial_bonus = (int)(100 / square_dist);
    }
    
    // スコア計算、パラメータは要調整
    int score = 0;
    score += int(1500.0 * _sigmoid(1.0, nfullsibling));
    score += int( 500.0 * _sigmoid(1.0, nsibling));
    score += int(1500.0 * _sigmoid(1.0, nchild));
    score += int(2000.0 * _sigmoid(1.0, nparent));
    score += int( 200.0 * _sigmoid(1.0, nclass));
    score += int( 100.0 * _sigmoid(1.0, ndictionary));
    score += spatial_bonus;
    score += geoword.get_priority_score() * 100;
    // debug 出力
#ifdef CONTEXT_LOG
    std::ofstream ofs("/tmp/geonlp.debug", std::ios::out | std::ios::app);
    ofs << geoword.get_typical_name() << ", hypernym:[";
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      ofs << (*it) << ",";
    }
    ofs << "], address:" << geoword.get_address() << ", ne_class:" << geoword.get_ne_class() << ", score:" << score << ", items(fullsibling:" << nfullsibling << ", sibling;" << nsibling << ", nchild:" << nchild << ", nparent:" << nparent << ", nclass:" << nclass << ", ndictionary:" << ndictionary << ",spatial_bonus:" << spatial_bonus << ", priority:" << geoword.get_priority_score() << ")" << std::endl;
    ofs.close();
#endif /* CONTEXT_LOG */
    return score;
  }

  // Geoword を一つ選択済みコンテキスト関係に登録する
  void Context::addGeowordToSelectedRelations(const Geoword& geoword, int n, int m) {
    std::string geonlp_id = geoword.get_geonlp_id();
    const std::vector<std::string>& hypernyms = geoword.get_hypernym();
    this->_selected_neclass.add(geoword.get_ne_class(), geonlp_id, n, m);
    this->_selected_dictionary.add(geoword.get_dictionary_id(), geonlp_id, n, m);
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      this->_selected_hypernym.add((*it), geonlp_id, n, m);
    }
    if (hypernyms.size() >= 2)
      this->_selected_full_hypernym.add(geoword.get_value("hypernym").serialize(), geonlp_id, n, m);
    this->_selected_name.add(geoword.get_typical_name(), geonlp_id, n, m);
  }

  // Geoword の選択済みコンテキストに対する出現スコアを計算する
  int Context::selectedScore(const Geoword& geoword, int n, int m) const {
    std::string geonlp_id = geoword.get_geonlp_id();
    // コンテキスト中に存在する親地名語数をカウント
    const std::vector<std::string>& hypernyms = geoword.get_hypernym();
    int nparent = 0;
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      nparent += this->_selected_name.count((*it), geonlp_id, n);
    }

    // コンテキスト中に存在する子地名語数をカウント
    int nchild = this->_selected_hypernym.count(geoword.get_typical_name(), geonlp_id, n);
    
    // コンテキスト中に存在する同クラス地名語数をカウント
    int nclass  = this->_selected_neclass.count(geoword.get_ne_class(), geonlp_id, n);

    // コンテキスト中に存在する同辞書地名語数をカウント
    int ndictionary = this->_selected_dictionary.count(geoword.get_dictionary_id(), geonlp_id, n);

    // コンテキスト中に存在する、親地名が一つでも重複する兄弟地名語数をカウント
    int nsibling = 0;
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      nsibling += this->_selected_hypernym.count((*it), geonlp_id, n);
    }

    // コンテキスト中に存在する、親地名が完全に一致する兄弟地名語数をカウント
    int nfullsibling = 0;
    if (hypernyms.size() >= 2)
      nfullsibling = this->_selected_full_hypernym.count(geoword.get_value("hypernym").serialize(), geonlp_id, n);
    
    // スコア計算、パラメータは要調整
    int score = 0;
    score += int(1500.0 * _sigmoid(1.0, nfullsibling));
    score += int( 500.0 * _sigmoid(1.0, nsibling));
    score += int(1500.0 * _sigmoid(1.0, nchild));
    score += int(2000.0 * _sigmoid(1.0, nparent));
    score += int( 200.0 * _sigmoid(1.0, nclass));
    score += int( 100.0 * _sigmoid(1.0, ndictionary));
    score += geoword.get_priority_score() * 100;
    // debug 出力
#ifdef CONTEXT_LOG
    std::ofstream ofs("/tmp/geonlp.debug", std::ios::out | std::ios::app);
    ofs << geoword.get_typical_name() << ", hypernym:[";
    for (std::vector<std::string>::const_iterator it = hypernyms.begin(); it != hypernyms.end(); it++) {
      ofs << (*it) << ",";
    }
    ofs << "], ne_class:" << geoword.get_ne_class() << ", score:" << score << ", items(fullsibling:" << nfullsibling << ", sibling;" << nsibling << ", nchild:" << nchild << ", nparent:" << nparent << ", nclass:" << nclass << ", ndictionary:" << ndictionary << ", priority:" << geoword.get_priority_score() << ")" << std::endl;
    ofs.close();
#endif /* CONTEXT_LOG */
    return score;
  }

  // parseNode の結果を追加する
  void Context::addNodes(const picojson::array& nodes) {
    int n = this->_nodes.size();
    for (picojson::array::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
      if (! (*it).is<picojson::null>()) {
	picojson::ext e((*it));
	if (e.has_key("address-candidates")) {
	  picojson::array varray = e.get_value("address-candidates").get<picojson::array>();
	  this->_addAddresses(varray, n); // 住所候補リストを登録
	  // Address address(e.get_value("address"));
	  // this->_addAddress(address, n); 
	} else if (e.has_key("candidates")) {
	  picojson::array varray = e.get_value("candidates").get<picojson::array>();
	  this->_addGeowords(varray, n); // 地名語候補リストを登録
	}
      }
      this->_nodes.push_back(*it);
      n++;
    }
  }

  void Context::_addAddresses(const picojson::array& varray, int n) {
    int size = varray.size();
    for (picojson::array::const_iterator it = varray.begin(); it != varray.end(); it++) {
      picojson::ext e(*it);
      Address address(e.get_value("candidate"));
      picojson::array elements = address.get_address_element().get<picojson::array>();
      int m = 0;
      for (picojson::array::iterator it_elem = elements.begin(); it_elem != elements.end(); it_elem++) {
	picojson::ext e(*it_elem);
	if (e.has_key("geoword")) {
	  Geoword geoword(e.get_value("geoword"));
	  if (!geoword.isValid()) throw ContextException(e.toJson());
	  this->addGeowordToContextRelations(geoword, n, m);
	}
	m++;
      }
      // 空間的位置を登録する
      {
	picojson::array::reverse_iterator it_elem = elements.rbegin();
	this->addAddressElementToSpatialRelations((*it_elem), size);
      }
    }
  }

  // １つの単語表記に割り当てられた地名語候補を登録する
  void Context::_addGeowords(const picojson::array& varray, int n) {
    int m = 0;
    for (picojson::array::const_iterator it = varray.begin(); it != varray.end(); it++) {
      Geoword geoword(*it);
      if (!geoword.isValid()) throw ContextException(geoword.toJson());
      // 上位語を利用した関係を登録する
      this->addGeowordToContextRelations(geoword, n, m);
      m++;
    }
    // 空間的位置を登録する
    this->addGeowordsToSpatialRelations(varray); //geoword, size);
  }

  // 登録済みの地名語候補のスコアを計算して評価する
  // スコアが最高となる候補の情報で geo 要素を更新する
  void Context::evaluate(void) {
    int n = 0;
    std::string prefix, suffix, surface;
    for (picojson::array::iterator it = this->_nodes.begin(); it != this->_nodes.end(); it++) {
      if ((*it).is<picojson::null>()) {
	n++;
	continue;
      }
      picojson::object& o = (*it).get<picojson::object>();
      surface = (*(o.find("surface"))).second.to_str();
      picojson::object::iterator it_geowords = o.find("candidates");
      if (it_geowords != o.end()) {
	int m = 0;
	int hiscore = -1, selected_hiscore = -1;
	Geoword bestGeoword;
	picojson::value& v_geowords = (*it_geowords).second;
	picojson::array& varray = v_geowords.get<picojson::array>();
	for (picojson::array::iterator it2 = varray.begin(); it2 != varray.end(); it2++) {
	  Geoword geoword(*it2);
	  if (!geoword.isValid()) throw ContextException(geoword.toJson());
	  int score = this->score(geoword, n, m);
	  score += this->selectedScore(geoword, n, m);
	  if (score > hiscore) {
	    hiscore = score;
	    bestGeoword = geoword;
	  }
	  (*it2).get<picojson::object>().insert(std::make_pair("score", picojson::value((long)score)));
	  m++;
	}
	// 接頭辞・接尾辞が含まれているかチェック
	if (bestGeoword.get_parts_for_surface(surface, prefix, suffix)) {
	  if (prefix.length() > 0) o.insert(std::make_pair("with_prefix", (picojson::value)prefix));
	  if (suffix.length() > 0) o.insert(std::make_pair("with_suffix", (picojson::value)suffix));
	}
	// geo 要素を更新
	picojson::object::iterator it_geo = o.find("geo");
	if (it_geo != o.end()) o.erase(it_geo);
	o.insert(std::make_pair("geo", (picojson::value)bestGeoword.getGeoObject()));
	o.insert(std::make_pair("score", (picojson::value)((double)hiscore)));
	// 選択済みコンテキストに追加
	this->addGeowordToSelectedRelations(bestGeoword, n, 0);
      } else {
	picojson::object::iterator it_addresses = o.find("address-candidates");
	if (it_addresses != o.end()) {
	  float clat, clon;
	  this->getCentroid(clat, clon);
	  float min_dist = -1;
	  Address best_address;
	  picojson::array& varray = (*it_addresses).second.get<picojson::array>();
	  for (picojson::array::iterator it2 = varray.begin(); it2 != varray.end(); it2++) {
	    picojson::ext e(*it2);
	    Address address(e.get_value("candidate"));
	    if (!address.isValid()) throw ContextException(address.toJson());
	    float lat = address.get_latitude();
	    float lon = address.get_longitude();
	    float square_dist = (clat - lat) * (clat - lat) + (clon - lon) * (clon - lon);
	    if (min_dist < 0 || square_dist < min_dist) {
	      min_dist = square_dist;
	      best_address = address;
	    }
#ifdef CONTEXT_LOG
	    std::ofstream ofs("/tmp/geonlp.debug", std::ios::out | std::ios::app);
	    ofs << address.get_standard_form() << "(" << lat << ", " << lon << ": dist= " << square_dist << ")" << std::endl;
	    ofs.close();
#endif /* CONTEXT_LOG */
	  }
#ifdef CONTEXT_LOG
	  std::ofstream ofs("/tmp/geonlp.debug", std::ios::out | std::ios::app);
	  ofs << "selected: " << best_address.get_standard_form() << "(" << clat << ", " << clon << ": dist= " << min_dist << ")" << std::endl;
	  ofs.close();
#endif /* CONTEXT_LOG */
	  o.insert(std::make_pair("address", picojson::value(best_address)));
	  o.insert(std::make_pair("geo", (picojson::value)best_address.getGeoObject()));
	}
      }
      n++;
    }
  }

  // 登録済みの地名語候補配列を返し、メモリから除去する
  // コンテキスト情報は消去されない
  picojson::array Context::flushNodes() {
    picojson::ext enull("null");
    picojson::array tmp_results, results;
    picojson::array::iterator it;
    std::string surface;
    // しきい値設定
    int threshold = 0;
    if (this->_options.has_key("threshold")) threshold = this->_options._get_int("threshold");
    // flush 済みの部分を早送りして頭出し
    for (it = this->_nodes.begin(); it != this->_nodes.end(); it++) {
      if (! (*it).is<picojson::null>()) break; 
    }
    // キューの先頭
    for (; it != this->_nodes.end(); it++) {
      if ((*it).is<picojson::null>()) break; // センテンスのエンドマーク
      picojson::ext e((*it));
      if (e.has_key("address")) { // 住所要素
	e.erase("address");
	if (!this->_options.has_key("show-candidate") || !this->_options.get_value("show-candidate")) {
	  // オプション指定がない場合は address-candidates を削除する
	  e.erase("address-candidates");
	}
	tmp_results.push_back((picojson::value)e);
      } else if (e.has_key("candidates")) { // 地名語
	if (e._get_int("score") < threshold && !e.has_key("with_prefix") && !e.has_key("with_suffix")) {
	  // しきい値以下で接頭辞・接尾辞が省略されていない場合は
	  // 地名語とみなさないので surface だけ残す
	  surface = e._get_string("surface");
	  e = picojson::ext();
	  e.set_value("surface", surface);
	} else { // しきい値以上なので score, with_prefix, with_suffix を削除する
	  e.erase("with_prefix");
	  e.erase("with_suffix");
	  if (!this->_options.has_key("show-score") || !this->_options.get_value("show-score")) {
	    // オプション指定がない場合はスコアを削除する
	    e.erase("score");
	  }
	  if (!this->_options.has_key("show-candidate") || !this->_options.get_value("show-candidate")) {
	    // オプション指定がない場合は candidates を削除する
	    e.erase("candidates");
	  }
	}
	tmp_results.push_back((picojson::value)e);
      } else { // 非地名語
	tmp_results.push_back((picojson::value)e);
      }
      // 処理済みのノードを空に
      it = this->_nodes.erase(it);
      this->_nodes.insert(it, (picojson::value)enull);
    }
    // 分割されている surface を連結する
    surface = "";
    for (it = tmp_results.begin(); it != tmp_results.end(); it++) {
      picojson::ext e((*it));
      if (e.has_key("geo")) {
	if (surface.length() > 0) {
	  picojson::ext new_surface("{}");
	  new_surface.set_value("surface", surface);
	  results.push_back((picojson::value)new_surface);
	}
	results.push_back((picojson::value)e);
	surface = "";
      } else {
	surface += e._get_string("surface");
      }
    }
    if (surface.length() > 0) {
      picojson::ext new_surface("{}");
      new_surface.set_value("surface", surface);
      results.push_back((picojson::value)new_surface);
    }
    return results;
  }

  // 重心を取得
  // lat, lon に緯度、経度が入る
  // 重み付き座標の数を返す
  int Context::getCentroid(float& lat, float& lon) const {
    if (this->_cumulative_points > 0) {
      lat = this->_cumulative_lat / this->_cumulative_points;
      lon = this->_cumulative_lon / this->_cumulative_points;
    }
    return this->_cumulative_points;
  }

}
