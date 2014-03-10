///
/// @file
/// @brief 固有名分類器 Classifier の実装
/// @author 株式会社情報試作室
///
/// Copyright (c)2013, NII
///

#include <sstream>
#include <fstream>
#include "Classifier.h"

static int _hfunc(const std::string& str, int max) {
  int i = str.length();
  int hv = 0;
  while (i-- > 0) {
    hv = (hv << 5) - hv + (unsigned char)(str.at(i));
    hv %= max;
  }
  return hv;
}

namespace geonlp
{

  Classifier::Classifier(const std::string& logfilename) {
    if (logfilename.length() > 0)
      this->_logfs.open(logfilename.c_str(), std::ofstream::out | std::ofstream::app);
  }

  Classifier::~Classifier() {
    if (this->_logfs.is_open()) this->_logfs.close();
  }

  // 表記のハッシュ値を得る
  int Classifier::get_surface_hash(const Node& node) const {
    int hv = _hfunc(node.get_surface(), 99000);
    return hv;
  }

  // 品詞のハッシュ値を得る
  int Classifier::get_feature_hash(const Node& node) const {
    std::string feature = node.get_partOfSpeech()
      + node.get_subclassification1()
      + node.get_subclassification2()
      + node.get_subclassification3();
    int hv = _hfunc(feature, 1000);
    return hv;
  }
  
  /// 文に含まれる地名語がどのぐらいの確率で地名語らしいか計算する
  /// @param nodes   文を形態素解析した Node 列
  /// @param it_target 対象となる地名語を指すイテレータ
  /// @return        地名らしさ (0.0 ≦ p ≦ 1.0), 1.0 なら確実に地名語
  double Classifier::check(const std::vector<Node>& nodes, std::vector<Node>::const_iterator it_target) {
    std::vector<Node>::const_iterator it_start, it_end;
    int pos = 0;
    it_start = it_target;
    it_end = it_target + 1;
    if (it_start != nodes.begin()) {
      it_start = it_start - 1;
      pos = -1;
      if (it_start != nodes.begin()) {
	it_start = it_start - 1;
	pos = -2;
      }
    }
    if (it_end != nodes.end()) {
      it_end = it_end + 1;
      if (it_end != nodes.end()) {
	it_end = it_end + 1;
      }
    }

    std::stringstream slog, ss;
    ss << "0";
    for (std::vector<Node>::const_iterator it = it_start; it != it_end; it++) {
      slog << "// " << pos << "\t" << (*it).toString() << std::endl;
      int hv_surface = this->get_surface_hash(*it) + 100000 * pos + 200000;
      int hv_feature = this->get_feature_hash(*it) + 100000 * pos + 200000;
      ss << " " << hv_surface << ":1 " << hv_feature << ":1";
      pos ++;
    }
    slog << ss.str() << std::endl;
    this->log(slog.str());
    return 0.5;
  }

  // ログファイルに結果を出力する
  void Classifier::log(const std::string& str) {
    if (this->_logfs.is_open()) {
      this->_logfs << str.c_str();
      this->_logfs.flush();
    }
  }

}
