///
/// @file
/// @brief  検索条件 SelectCondition およびその継承クラスの定義
/// @author 株式会社情報試作室
///
/// Copyright (c)2015, NII
///

#ifndef _SELECT_CONDITION_H
#define _SELECT_CONDITION_H

#include <string>
#include <vector>
#include <boost/regex.hpp>
#include "Geoword.h"
#include "picojsonExt.h"

namespace geonlp
{
  // 例外クラス
  class SelectConditionException:public std::runtime_error {
  public:
  SelectConditionException(const std::string& message): runtime_error(message) {}
    ~SelectConditionException() throw() {}
  };

  // 検索条件、重みづけなどの制約を管理するベースクラス
  class SelectCondition {
  protected:

    // 年月日表記のチェックと正規化
    // YYYYMMDD または YYYY-MM-DD を受け付け、 YYYYMMDD を返す
    // 正しくない表記の場合は "" を返す
    static boost::regex _ymd_pattern;
    static std::string _get_ymd(const std::string& ymd) throw (SelectConditionException);
    std::string _from_ymd;
    std::string _to_ymd;
    void set_from(const std::string& ymd) throw (SelectConditionException) 
    { this->_from_ymd = SelectCondition::_get_ymd(ymd); }
    void set_to(const std::string& ymd) throw (SelectConditionException) 
    { this->_to_ymd = SelectCondition::_get_ymd(ymd); }

    // 実行環境によって利用不可能な条件が実行されたときの処理
    void unsupported_action(const std::string& name) throw (SelectConditionException);

  public:
    // コンストラクタ
    SelectCondition();

    // デストラクタ
    virtual ~SelectCondition();

    // JSON オプションパラメータから条件を構築
    virtual void set(picojson::ext& options);

    // Geoword が制約を満たすかどうかを判定する
    // 0または正の値を返した場合にはスコアに乗じる重み
    // 負の値を返した場合には制約条件を満たさない
    virtual double judge(const Geoword*);

  }; /* class SelectCondition */

  /*****************************************
   * 各演算子に対応する検索条件クラスの定義
   *****************************************/

  // geo-contains 条件
  class SelectConditionGeoContains : public SelectCondition {
  public:
  SelectConditionGeoContains() : SelectCondition() {};
    ~SelectConditionGeoContains() {}
    void set(picojson::ext& options); // "geo-contains" のオプションを読み込む
    double judge(const Geoword* pGeoword);  // "geo-contains" の処理
  }; /* class SelectConditionGeoContains */

  // geo-disjoint 条件
  class SelectConditionGeoDisjoint : public SelectCondition {
  public:
  SelectConditionGeoDisjoint() : SelectCondition() {};
    ~SelectConditionGeoDisjoint() {}
    void set(picojson::ext& options); // "geo-disjoint" のオプションを読み込む
    double judge(const Geoword* pGeoword);  // "geo-disjoint" の処理
  }; /* class SelectConditionGeoDisjoint */

  // time-exists 条件
  class SelectConditionTimeExists : public SelectCondition {
  public:
  SelectConditionTimeExists() : SelectCondition() {};
    ~SelectConditionTimeExists() {};
    void set(picojson::ext& options); // "time-exists" のオプションを読み込む
    double judge(const Geoword* pGeoword);  // "time-exists" の処理
  }; /* class SelectConditionTimeExists */

  // time-before 条件
  class SelectConditionTimeBefore : public SelectCondition {
  public:
  SelectConditionTimeBefore() : SelectCondition() {};
    ~SelectConditionTimeBefore() {};
    void set(picojson::ext& options); // "time-before" のオプションを読み込む
    double judge(const Geoword* pGeoword);  // "time-before" の処理
  }; /* class SelectConditionTimeBefore */

  // time-after 条件
  class SelectConditionTimeAfter : public SelectCondition {
  public:
  SelectConditionTimeAfter() : SelectCondition() {};
    ~SelectConditionTimeAfter() {};
    void set(picojson::ext& options); // "time-after" のオプションを読み込む
    double judge(const Geoword* pGeoword);  // "time-after" の処理
  }; /* class SelectConditionTimeAfter */

  // time-overlaps 条件
  class SelectConditionTimeOverlaps : public SelectCondition {
  public:
  SelectConditionTimeOverlaps() : SelectCondition() {};
    ~SelectConditionTimeOverlaps() {};
    void set(picojson::ext& options); // "time-overlaps" のオプションを読み込む
    double judge(const Geoword* pGeoword);  // "time-overlaps" の処理
  }; /* class SelectConditionTimeOverlaps */

  // time-contains 条件
  class SelectConditionTimeContains : public SelectCondition {
  public:
  SelectConditionTimeContains() : SelectCondition() {};
    ~SelectConditionTimeContains() {};
    void set(picojson::ext& options); // "time-contains" のオプションを読み込む
    double judge(const Geoword* pGeoword);  // "time-contains" の処理
  }; /* class SelectConditionTimeContains */

} /* namespace */
#endif /* _SELECT_CONDITION_H */
