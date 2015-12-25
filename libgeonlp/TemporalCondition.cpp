///
/// @file
/// @brief  時間的条件 TemporalCondition の実装
/// @author 株式会社情報試作室
///
/// Copyright (c)2015, NII
///

#include <config.h>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <fstream>
#include "picojson.h"
#include "TemporalCondition.h"

namespace geonlp
{
  boost::regex TemporalCondition::_ymd_pattern("(\\d{4})\\-?(\\d{2})\\-?(\\d{2})");

  // コンストラクタ
  TemporalCondition::TemporalCondition() {
    this->_from_ymd = "";
    this->_to_ymd = "";
  }

  // デストラクタ
  TemporalCondition::~TemporalCondition() {
  }

  // 年月日表記のチェックと正規化
  std::string TemporalCondition::_get_ymd(const std::string& ymd)
  {
    std::string formal_ymd = "";
    boost::smatch what;
    if (boost::regex_search(ymd, what, TemporalCondition::_ymd_pattern)) {
      formal_ymd = std::string(what[1].first, what[1].second)
	+ std::string(what[2].first, what[2].second)
	+ std::string(what[3].first, what[3].second);
    } else {
      formal_ymd = "";
    }
    return formal_ymd;
  }
  
  // 開始日を追加する
  void TemporalCondition::set_from(const std::string& ymd) {
    this->_from_ymd = this->_get_ymd(ymd);
  }

  // 終了日を追加する
  void TemporalCondition::set_to(const std::string& ymd) {
    this->_to_ymd = this->_get_ymd(ymd);
  }

  // JSON オプションパラメータから条件を構築
  void TemporalCondition::set(const picojson::value& options) throw (TemporalConditionException) {
    std::string ymd;
    if (options.is<std::string>()) {
      // 文字列が一つの場合は開始・終了日を指定
      ymd = this->_get_ymd(options.to_str());
      if (ymd == "") goto error;
      this->set_from(ymd);
      this->set_to(ymd);
    } else if (options.is<picojson::array>()) {
      // 配列の場合
      picojson::array ymds = options.get<picojson::array>();
      if (ymds.size() == 1) {
	ymd = this->_get_ymd(ymds[0].to_str());
	if (ymd == "") goto error;
	this->set_from(ymd);
	this->set_to(ymd);
      } else if (ymds.size() == 2) {
	ymd = this->_get_ymd(ymds[0].to_str());
	if (ymd == "") goto error;
	this->set_from(ymd);
	ymd = this->_get_ymd(ymds[1].to_str());
	if (ymd == "") goto error;
	this->set_to(ymd);
      } else {
	goto error;
      }
    }
    return;
  error:
    std::string errmsg = "'" + options.serialize() + "' is not a valid date representation.";
    throw new TemporalConditionException(errmsg);
  }

  // Geoword が条件を満たすかどうかを判定する
  // 0または正の値を返した場合にはスコアに乗じる重み
  // 負の値を返した場合には条件を満たさない
  double TemporalCondition::judge(const Geoword* pGeoword) {
    double result = 1.0;

    std::string valid_from = this->_get_ymd(pGeoword->get_valid_from());
    std::string valid_to   = this->_get_ymd(pGeoword->get_valid_to());

    double isValid = 1.0;
    if (valid_from != "" && this->_to_ymd != ""
	&& valid_from > this->_to_ymd) {
      isValid = -1.0;
    } else if (valid_to != "" && this->_from_ymd != ""
	&& valid_to < this->_from_ymd) {
      isValid = -1.0;
    }

    return isValid;
  }

} /* namespace */
