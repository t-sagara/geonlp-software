#ifndef _GEOWORD_JUDGE_
#define _GEOWORD_JUDGE_

///
/// @file
/// @brief 地名語を含む例文を形態素解析して地名語が地名語として識別可能か否か判定するクラス。また、地名語が地名語として識別するために必要な生起コストを求めるクラス。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#include "config.h"
#include <mecab.h>
#include <string>
#include <vector>
#include <map>
#include "NodeExt.h"
#include "GeowordJudgeFailedException.h"
typedef std::map<std::string, long int> TableLong;
typedef std::map<std::string, short int> TableShort;

class GeowordJudge {

 private:

  mecab_t *mecab;
  geonlp::PHBSDefs phbsdefs;
  std::string pre_sentence;
  std::string pst_sentence;

  static std::vector<std::string> locnames;
  TableShort pre_wcost;
  TableLong  pre_cost;

  long _cost_sub(const std::string &surface, bool &is_unk)
    throw (GeowordJudgeFailedException);

 public:

  /// @brief コンストラクタ。オブジェクトが適用する例文を引数で指定してください。
  ///        例文は以下の通り。
  ///
  ///        pre_sentence + 地名語 + pst_sentence
  ///
  /// @param [in] mecab 使用するMeCabへのポインタ
  /// @param [in] pre_sentence 地名語の前に付加するテキスト
  /// @param [in] pst_sentence 地名語の後に付加するテキスト
  GeowordJudge(mecab_t *mecab,
	       const std::string &pre_sentence,
	       const std::string &pst_sentence,
	       const geonlp::Profile& profile);

  /// @brief 引数で与えられた「地名語」が 地名語として識別可能か否か判定する。
  ///
  /// @param [in] surface 地名語表記
  /// @return
  /// @exception GeowordJudgeFailedException 判定に失敗した場合
  bool judge(const std::string &surface) const
    throw (GeowordJudgeFailedException);

  long cost(const std::string &surface,
	    bool &is_undefined)
    throw (GeowordJudgeFailedException);
};

#endif
