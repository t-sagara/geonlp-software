#include "config.h"
#include "GeowordJudge.h"
#include <iostream>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */
#include <mecab.h>

// 代表的な地名語を準備
std::vector<std::string> GeowordJudge::locnames;

GeowordJudge::GeowordJudge(mecab_t *mecab, 
			   const std::string &pre_sentence,
			   const std::string &pst_sentence,
			   const geonlp::Profile& profile) {
  this->mecab = mecab;
  this->pre_sentence = pre_sentence;
  this->pst_sentence = pst_sentence;
  this->phbsdefs.readProfile(profile);
  
  if (this->locnames.size() == 0) {
    locnames.push_back("ロンドン");
    locnames.push_back("東京");
  }
}

bool GeowordJudge::judge(const std::string &surface) const
  throw (GeowordJudgeFailedException) {

  // 形態素解析を行う
  std::string sentence = pre_sentence + surface + pst_sentence;
  const mecab_node_t *node;
  {
    mecab_set_lattice_level(mecab, 0);
    node = mecab_sparse_tonode(mecab, sentence.c_str());
    if (node == NULL) throw GeowordJudgeFailedException("failed to parse by mecab.");
  }

  // 地名語を構成するノード（列）が、特定の条件にマッチしているか判定
  // ２．地名語を構成するノード列が、パターン /^P?HB*$/ にマッチするか否か判定する

  unsigned int len = 0;
  for (; node; node = node->next) {
    len += node->length;
    if (len == pre_sentence.length()) break;
    else if (len > pre_sentence.length()) return false;
  }
  node = node->next;

  const unsigned int geo_len = surface.length();
  len = 0;
  std::string step = "^";
  for ( ; node; node = node->next) {
    if (node->stat == MECAB_UNK_NODE)
      return false;

    // std::cout.write(node->surface, node->length) << "\t" << node->feature << std::endl; // DEBUG

    geonlp::Node n(std::string(node->surface, node->length), node->feature);
    geonlp::NodeExt nodex(n);
    nodex.evaluatePossibility(phbsdefs, false);

    if (step.compare("^") == 0) {
      if (nodex.canBeHead()) {
	step = "H";
      } else if (nodex.canBePrefix()) {
	step = "P";
      } else {
	return false;
      }
    } else if (step.compare("P") == 0) {
      if (nodex.canBeHead()) {
	step = "H";
      } else {
	return false;
      }
    } else if (step.compare("H") == 0) {
      if (nodex.canBeBody()) {
	step = "B";
      } else {
	return false;
      }
    } else if (step.compare("B") == 0) {
      if (nodex.canBeBody()) {
	step = "B";
      } else {
	return false;
      }
    }

    len += node->length;
    if (len == geo_len) break;
    else if (len > geo_len) return false;
  }

  return (step.compare("H") || step.compare("B"));
}

long GeowordJudge::cost(const std::string &surface,
			     bool &is_undefined)
  throw (GeowordJudgeFailedException){
  int i;
  std::string locname;
  is_undefined = false;

#ifdef DEBUG
  if (geoword == "明治" || geoword == "昭和" || geoword == "平成") {
    std::cerr << "年号：" << geoword << std::endl;
  }
#endif /* DEBUG */

  short wcost_london = SHRT_MAX; // 代表地名の単語生起コスト
  long cost_london = LONG_MAX;   // 例文の累積コスト

  // 計算済みの代表地名生起コストを調べる
  const std::string key("");
  TableShort::iterator it = pre_wcost.find(key);
  if (it != this->pre_wcost.end()) {
    wcost_london = it->second;
    cost_london  = pre_cost.find(key)->second;
  } else {
    for (i = 0; i < locnames.size(); i++) {
      locname = locnames[i];
      // 代表地名を用いて例文の形態素解析を行い、累計コストと代表地名の生起コストを求める。
      mecab_set_lattice_level(mecab, 0);
      std::string sentence = pre_sentence + locname + pst_sentence;
      
      const mecab_node_t *node = mecab_sparse_tonode(mecab, sentence.c_str());
      if (node == NULL) {
	std::cerr << "unprocessible sentence '" << sentence << "'" << std::endl;
	throw GeowordJudgeFailedException("parse error occured in mecab.");
      }
    
      for ( ; node; node = node->next) {
	if (node->stat == MECAB_BOS_NODE) {
	  continue;
	} else if (node->stat == MECAB_EOS_NODE) {
	  cost_london = node->cost;
	  continue;
	} else {
	  if (strncmp(node->surface, locname.c_str(), node->length) == 0) {
	    wcost_london = node->wcost;
	  }
	}
      }
      if (wcost_london == SHRT_MAX || cost_london == LONG_MAX) {
	// 「代表地名＋語尾」が適切に分割できず、
	// 単語生起コストが計算できない場合の処理
	std::cerr << surface << " can't analyze with typical location name '" << locname << "'" << std::endl;
	//		throw GeowordJudgeFailedException("unexpected error was occured.");
	continue;
      }
#ifdef DEBUG
      //    std::cout << sentence << " => 累積コスト：" << cost_london << "(単語生起コスト：" << wcost_london << ")" << std::endl;
#endif // DEBUG
    }
    pre_cost[key] = cost_london;
    pre_wcost[key] = wcost_london;
  }

  // 対象地名を用いて例文の形態素解析を行い、累計コストと代表地名の生起コストを求める。
  short wcost_xxx = SHRT_MAX;
  long cost_xxx = LONG_MAX;

  std::string sentence = pre_sentence + surface + pst_sentence;
  mecab_set_lattice_level(mecab, 0);
  const mecab_node_t *node = mecab_sparse_tonode(mecab, sentence.c_str());
  if (node == NULL) throw GeowordJudgeFailedException("failed to parse by mecab.");

  static std::string mecab_surface;
  for ( ; node; node = node->next) {
    mecab_surface = std::string(node->surface, node->length);
    if (mecab_surface == surface) {
      //      std::cerr << surface << '\t' << node->feature << std::endl;
      is_undefined = false;
      //if (strncmp(node->feature, "名詞,固有名詞", 19) == 0) {

      geonlp::Node n(mecab_surface, node->feature);
      geonlp::NodeExt nodex(n);
      nodex.evaluatePossibility(phbsdefs, false);

      if (nodex.canBeHead()) {
	return -1; // 既に H に属する語なので MeCab ユーザ辞書への登録不要
      }
      // std::cerr << "KNOWNWORD\t" << mecab_surface << "\t" << node->feature << std::endl;
    }
    // if (node->stat == MECAB_UNK_NODE) is_unk = true;
    if (node->stat == MECAB_EOS_NODE) {
      cost_xxx = node->cost;
    }
  }

  if (cost_xxx == LONG_MAX) {
    throw GeowordJudgeFailedException("unexpected error was occured.");
  }
  // 単語生起コストをセット
  if (is_undefined) {
    // 未定義語（複合語を含む）なので、既存生起コストより低く（+1）する
    wcost_xxx = cost_xxx - cost_london + wcost_london - 1;
  } else {
    // 定義されている単語なので、既存生起コストより高く(+1)する
    // wcost_xxx = cost_xxx - cost_london + wcost_london + 1;
    // 定義されている単語なので、既存生起コストと同じにする
    wcost_xxx = cost_xxx - cost_london + wcost_london;
  }

#ifdef DEBUG
  //    std::cout << sentence << " => 累積コスト：" << cost_xxx << "(単語生起コスト：" << wcost_xxx << ")" << ", omittable = " << (omittable ? "t" : "f") << ", is_undefined = " << (is_undefined ? "t" : "f") << std::endl;
#endif // DEBUG

  return wcost_xxx;
}
