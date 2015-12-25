///
/// @file
/// @brief  時間的条件 TemporalCondition の定義
/// @author 株式会社情報試作室
///
/// Copyright (c)2015, NII
///

#ifndef _TEMPORAL_CONDITION_H
#define _TEMPORALL_CONDITION_H

#include <string>
#include <vector>
#include <boost/regex.hpp>
#include "Geoword.h"
#include "picojson.h"

namespace geonlp
{
  // 例外クラス
  class TemporalConditionException:public std::runtime_error {
  public:
  TemporalConditionException(const std::string& message): runtime_error(message) {}
    ~TemporalConditionException() throw() {}
  };

  // 時間的な検索条件、重みづけなどの制約を管理するクラス
  class TemporalCondition {
  private:
    std::string _from_ymd;
    std::string _to_ymd;
    static boost::regex _ymd_pattern;

    // 年月日表記のチェックと正規化
    // YYYYMMDD または YYYY-MM-DD を受け付け、 YYYYMMDD を返す
    // 正しくない表記の場合は "" を返す
    std::string _get_ymd(const std::string& ymd);

  public:
    // コンストラクタ
    TemporalCondition();

    // デストラクタ
    ~TemporalCondition();

    // 開始年月日を指定
    void set_from(const std::string& ymd);

    // 終了年月日を指定
    void set_to(const std::string& ymd);

    // JSON オプションパラメータから条件を構築
    void set(const picojson::value& options) throw (TemporalConditionException);

    // Geoword が制約を満たすかどうかを判定する
    // 0または正の値を返した場合にはスコアに乗じる重み
    // 負の値を返した場合には制約条件を満たさない
    double judge(const Geoword*);

  }; /* class */

} /* namespace */
#endif /* _TEMPORAL_CONDITION_H */
