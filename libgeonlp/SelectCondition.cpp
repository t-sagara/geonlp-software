///
/// @file
/// @brief  検索条件 SelectCondition およびその継承クラスの実装
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
#include "SelectCondition.h"

namespace geonlp
{

  /*****************************
   * SelectCondition 
   *****************************/

  // 年月日表記の正規表現
  boost::regex SelectCondition::_ymd_pattern("(\\d{4})\\-?(\\d{2})\\-?(\\d{2})");

  // 年月日表記のチェックと正規化
  std::string SelectCondition::_get_ymd(const std::string& ymd) throw (SelectConditionException)
  {
    std::string formal_ymd = "";
    boost::smatch what;
    if (ymd == "") return "";
    if (boost::regex_search(ymd, what, SelectCondition::_ymd_pattern)) {
      formal_ymd = std::string(what[1].first, what[1].second)
	+ std::string(what[2].first, what[2].second)
	+ std::string(what[3].first, what[3].second);
    } else {
      std::string errmsg = "'" + ymd + "' is not a valid date format. Dates must be represented as 'YYYY-MM-DD'.";
      throw SelectConditionException(errmsg);
    }
    return formal_ymd;
  }

  // コンストラクタ
  SelectCondition::SelectCondition() {
  }

  // デストラクタ
  SelectCondition::~SelectCondition() {
  }

  void SelectCondition::unsupported_action(const std::string& name) throw (SelectConditionException) 
  {
    // 環境変数をチェック
    char* p = std::getenv("GEONLP_IGNORE_ERROR");
    if (p && (!strncmp(p, "Y", 1) || !strncmp(p, "y", 1))) {
      // 環境変数 GEONLP_IGNORE_ERROR が 'Y*' または 'y*' の場合
      // 無視して先に進む
      return;
    }
    // それ以外の場合はエラー終了
    throw SelectConditionException(std::string("'") + name + "' is not supported. (Hint: Set environmetal variable GEONLP_IGNORE_ERROR' to 'YES' to ignore this exception)");
  }

  void SelectCondition::set(picojson::ext& options) {
    ;
  }

  double SelectCondition::judge(const Geoword* pGeoword) {
    return 1.0;
  }

  /*****************************
   * SelectConditionGeoContains
   *****************************/

  void SelectConditionGeoContains::set(picojson::ext& options) {
    this->unsupported_action("geo-contains");
  }

  double SelectConditionGeoContains::judge(const Geoword* pGeoword) {
    double result = 1.0;
    return result;
  }

  /*****************************
   * SelectConditionGeoDisjoint
   *****************************/

  void SelectConditionGeoDisjoint::set(picojson::ext& options) {
    this->unsupported_action("geo-disjoint");
  }

  double SelectConditionGeoDisjoint::judge(const Geoword* pGeoword) {
    double result = 1.0;
    return result;
  }

  /*****************************
   * SelectConditionTimeExists
   *****************************/

  void SelectConditionTimeExists::set(picojson::ext& options) {
    std::string ymd;
    std::string errmsg = "'time-exists' takes 1 string value.";
    if (options.is_null("time-exists")) throw SelectConditionException(errmsg);
    const picojson::value& op_v = options.get_value("time-exists");
    if (op_v.is<std::string>()) {
      // 判定日を指定
      ymd = SelectCondition::_get_ymd(op_v.to_str());
      if (ymd == "") goto error;
      this->set_from(ymd);
      return;
    } else if (op_v.is<picojson::array>()) {
      // 配列の場合
      picojson::array ymds = op_v.get<picojson::array>();
      if (ymds.size() == 1) {
	ymd = SelectCondition::_get_ymd(ymds[0].to_str());
	if (ymd == "") goto error;
	this->set_from(ymd);
	return;
      }
    }
    throw SelectConditionException(errmsg);
    return;
  error:
    errmsg = "'" + op_v.serialize() + "' is not a valid date representation.";
    throw SelectConditionException(errmsg);
  }

  double SelectConditionTimeExists::judge(const Geoword* pGeoword) {
    double result = 1.0;

    std::string valid_from = SelectCondition::_get_ymd(pGeoword->get_valid_from());
    std::string valid_to   = SelectCondition::_get_ymd(pGeoword->get_valid_to());

    double isValid = 1.0;
    if (valid_from != "" && valid_from > this->_from_ymd) {
      isValid = -1.0;
    } else if (valid_to != "" && valid_to < this->_from_ymd) {
      isValid = -1.0;
    }

    return isValid;
  }

  /*****************************
   * SelectConditionTimeBefore
   *****************************/

  void SelectConditionTimeBefore::set(picojson::ext& options) {
    std::string ymd;
    std::string errmsg = "'time-before' takes 1 string value.";
    if (options.is_null("time-before")) throw SelectConditionException(errmsg);
    const picojson::value& op_v = options.get_value("time-before");
    if (op_v.is<std::string>()) {
      // 判定日を指定
      ymd = SelectCondition::_get_ymd(op_v.to_str());
      if (ymd == "") goto error;
      this->set_from(ymd);
      return;
    } else if (op_v.is<picojson::array>()) {
      // 配列の場合
      picojson::array ymds = op_v.get<picojson::array>();
      if (ymds.size() == 1) {
	ymd = SelectCondition::_get_ymd(ymds[0].to_str());
	if (ymd == "") goto error;
	this->set_from(ymd);
	return;
      }
    }
    throw SelectConditionException(errmsg);
    return;
  error:
    errmsg = "'" + op_v.serialize() + "' is not a valid date representation.";
    throw SelectConditionException(errmsg);
  }

  double SelectConditionTimeBefore::judge(const Geoword* pGeoword) {
    double result = 1.0;

    std::string valid_from = SelectCondition::_get_ymd(pGeoword->get_valid_from());
    std::string valid_to   = SelectCondition::_get_ymd(pGeoword->get_valid_to());

    double isValid = 1.0;
    if (valid_from != "" && valid_from > this->_from_ymd) {
      isValid = -1.0;
    }

    return isValid;
  }

  /*****************************
   * SelectConditionTimeAfter
   *****************************/

  void SelectConditionTimeAfter::set(picojson::ext& options) {
    std::string ymd;
    std::string errmsg = "'time-after' takes 1 string value.";
    if (options.is_null("time-after")) throw SelectConditionException(errmsg);
    const picojson::value& op_v = options.get_value("time-after");
    if (op_v.is<std::string>()) {
      // 判定日を指定
      ymd = SelectCondition::_get_ymd(op_v.to_str());
      if (ymd == "") goto error;
      this->set_from(ymd);
      return;
    } else if (op_v.is<picojson::array>()) {
      // 配列の場合
      picojson::array ymds = op_v.get<picojson::array>();
      if (ymds.size() == 1) {
	ymd = SelectCondition::_get_ymd(ymds[0].to_str());
	if (ymd == "") goto error;
	this->set_from(ymd);
	return;
      }
    }
    throw SelectConditionException(errmsg);
    return;
  error:
    errmsg = "'" + op_v.serialize() + "' is not a valid date representation.";
    throw SelectConditionException(errmsg);
  }

  double SelectConditionTimeAfter::judge(const Geoword* pGeoword) {
    double result = 1.0;

    std::string valid_from = SelectCondition::_get_ymd(pGeoword->get_valid_from());
    std::string valid_to   = SelectCondition::_get_ymd(pGeoword->get_valid_to());

    double isValid = 1.0;
    if (valid_to != "" && valid_to < this->_from_ymd) {
      isValid = -1.0;
    }

    return isValid;
  }

  /*****************************
   * SelectConditionTimeOverlaps
   *****************************/

  void SelectConditionTimeOverlaps::set(picojson::ext& options) {
    std::string ymd;
    std::string errmsg = "'time-overlaps' takes 1 string value, or 1 array of 2 values.";
    if (options.is_null("time-overlaps")) throw SelectConditionException(errmsg);
    const picojson::value& op_v = options.get_value("time-overlaps");
    if (op_v.is<std::string>()) {
      // 文字列が一つの場合は開始・終了日を指定
      ymd = SelectCondition::_get_ymd(op_v.to_str());
      if (ymd == "") goto error;
      this->set_from(ymd);
      this->set_to(ymd);
      return;
    } else if (op_v.is<picojson::array>()) {
      // 配列の場合
      picojson::array ymds = op_v.get<picojson::array>();
      if (ymds.size() == 1) {
	ymd = SelectCondition::_get_ymd(ymds[0].to_str());
	if (ymd == "") goto error;
	this->set_from(ymd);
	this->set_to(ymd);
	return;
      } else if (ymds.size() == 2) {
	ymd = SelectCondition::_get_ymd(ymds[0].to_str());
	this->set_from(ymd);
	ymd = SelectCondition::_get_ymd(ymds[1].to_str());
	this->set_to(ymd);
	return;
      }
    }
    throw SelectConditionException(errmsg);
    return;
  error:
    errmsg = "'" + op_v.serialize() + "' is not a valid date representation.";
    throw SelectConditionException(errmsg);
  }

  double SelectConditionTimeOverlaps::judge(const Geoword* pGeoword) {
    double result = 1.0;

    std::string valid_from = SelectCondition::_get_ymd(pGeoword->get_valid_from());
    std::string valid_to   = SelectCondition::_get_ymd(pGeoword->get_valid_to());

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

  /*****************************
   * SelectConditionTimeContains
   *****************************/

  void SelectConditionTimeContains::set(picojson::ext& options) {
    std::string ymd;
    std::string errmsg = "'time-contains' takes 1 array of 2 values.";
    if (options.is_null("time-contains")) throw SelectConditionException(errmsg);
    const picojson::value& op_v = options.get_value("time-contains");
    if (op_v.is<picojson::array>()) {
      // 配列の場合
      picojson::array ymds = op_v.get<picojson::array>();
      if (ymds.size() == 2) {
	ymd = SelectCondition::_get_ymd(ymds[0].to_str());
	if (ymd == "") goto error;
	this->set_from(ymd);
	ymd = SelectCondition::_get_ymd(ymds[1].to_str());
	if (ymd == "") goto error;
	this->set_to(ymd);
	return;
      }
    }
    throw SelectConditionException(errmsg);
    return;
  error:
    errmsg = "'" + op_v.serialize() + "' is not a valid date representation.";
    throw SelectConditionException(errmsg);
  }

  double SelectConditionTimeContains::judge(const Geoword* pGeoword) {
    double result = 1.0;

    std::string valid_from = SelectCondition::_get_ymd(pGeoword->get_valid_from());
    std::string valid_to   = SelectCondition::_get_ymd(pGeoword->get_valid_to());

    double isValid = 1.0;
    if (valid_from != "" && valid_from < this->_from_ymd) {
      isValid = -1.0;
    } else if (valid_to != "" && valid_to > this->_to_ymd) {
      isValid = -1.0;
    }

    return isValid;
  }

} /* namespace */
