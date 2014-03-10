///
/// @file
/// @brief  固有名分類器の定義
/// @author 株式会社情報試作室
///
/// Copyright (c)2013, NII
///

#ifndef _CLASSIFIER_H
#define _CLASSIFIER_H

#include <fstream>
#include <string>
#include <vector>
#include "Geoword.h"
#include "Node.h"

namespace geonlp
{
  
  /// 固有名分類器クラス
  class Classifier {
  private:
    std::ofstream _logfs;

    // ログファイルに文字列を出力
    void log(const std::string& str);

    // 表記のハッシュ値を得る
    int get_surface_hash(const Node& node) const;

    // 品詞のハッシュ値を得る
    int get_feature_hash(const Node& node) const;

  public:
    // コンストラクタ
    Classifier(const std::string& logfilename = "");

    ~Classifier();

    // 地名である確率をチェック
    double check(const std::vector<Node>& nodes, std::vector<Node>::const_iterator it);

  };

}
#endif /* _CONTEXT_H */
