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
#ifdef HAVE_LIBGDAL
    this->_ogrDataSources.clear();
    this->_tmpfiles.clear();
    OGRRegisterAll();
#endif /* HAVE_LIBGDAL */    
  }

  // デストラクタ
  SelectCondition::~SelectCondition() {
    this->clear_spatial_sources();
#ifdef HAVE_LIBGDAL
    OGRCleanupAll();
#endif /* HAVE_LIBGDAL */
  }

  // JSON オプションパラメータから空間データを構築
  void SelectCondition::set_spatial_sources(const picojson::value& op_v) {
    this->clear_spatial_sources();
    if (op_v.is<picojson::null>()) return;
    if (op_v.is<std::string>()) {
      // 文字列が一つの場合は外部ファイルの URL を指定
      this->add_spatial_data_source(op_v.to_str());
    } if (op_v.is<picojson::object>()) {
      // オブジェクト一つの場合は GeoJSON として追加
      this->add_geojson_source(op_v);
    } else if (op_v.is<picojson::array>()) {
      // 配列の場合は、複数の外部ファイルもしくは GeoJSON を指定
      picojson::array sources = op_v.get<picojson::array>();
      for (picojson::array::iterator it = sources.begin(); it != sources.end(); it++) {
	picojson::value& v = (*it);
	if (v.is<std::string>()) {
	  // 外部ファイルの URL
	  this->add_spatial_data_source(v.to_str());
	} else if (v.is<picojson::object>()) {
	  // GeoJSON, 一時ファイルに保存する
	  this->add_geojson_source(v);
	}
      }
    }
  }

  // 用意した空間データを削除
  void SelectCondition::clear_spatial_sources(void) {
#ifdef HAVE_LIBGDAL
    if (this->_ogrDataSources.size() > 0) {
      // 読み込み済みのデータソースをクリアする
      for (std::vector<OGRDataSource*>::iterator it = this->_ogrDataSources.begin();
	   it != this->_ogrDataSources.end(); it++) {
	OGRDataSource::DestroyDataSource((*it));
      }
      this->_ogrDataSources.clear();
    }
    if (this->_tmpfiles.size() > 0) { // 一時ファイルを削除する
      for (std::vector<std::string>::iterator it = this->_tmpfiles.begin();
	   it != this->_tmpfiles.end(); it++) {
	std::remove((*it).c_str());
      }
      this->_tmpfiles.clear();
    }
#endif /* HAVE_LIBGDAL */
  }
  
  // データソースを追加する
  int SelectCondition::add_spatial_data_source(const std::string& url) {
#ifdef HAVE_LIBGDAL
    OGRSFDriver* poDriver;
    OGRDataSource* poDataSource;
    poDataSource = OGRSFDriverRegistrar::Open(url.c_str(), FALSE, &(poDriver));
    if (poDataSource == NULL) {
      std::stringstream ss;
      ss << "Fail to read spatial-condition data, url:'" << url << "'";
      throw SelectConditionException(ss.str());
    }
    // std::cerr << "Successfully read " << url << "." << std::endl;
    this->_ogrDataSources.push_back(poDataSource);
#endif /* HAVE_LIBGDAL */
  }

  // GeoJSON を追加する
  int SelectCondition::add_geojson_source(const picojson::value& v) {
    std::string tmp_filename = std::tmpnam(NULL);
    tmp_filename += "_geonlp_tmp.geojson";
    std::ofstream ofs;
    ofs.open(tmp_filename.c_str(), std::ios::out);
    ofs << v.serialize();
    ofs.close();
    this->_tmpfiles.push_back(tmp_filename);
    this->add_spatial_data_source(tmp_filename);
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
#ifdef HAVE_LIBGDAL
    if (!options.has_key("geo-contains")) return;
    const picojson::value& op_v = options.get_value("geo-contains");
    this->set_spatial_sources(op_v);
#endif /* HAVE_LIBGDAL */
  }

  double SelectConditionGeoContains::judge(const Geoword* pGeoword) {
    double result = 1.0;
#ifdef HAVE_LIBGDAL
    // std::cerr << "#sources = " << this->_ogrDataSources.size() << std::endl;
    if (this->_ogrDataSources.size() == 0) return result;
    // 内外判定
    int isInside = 0;
    double longitude, latitude;
    std::stringstream sslat, sslon;
    sslat << pGeoword->get_latitude();
    sslat >> latitude;
    sslon << pGeoword->get_longitude();
    sslon >> longitude;
    OGRPoint targetPoint(longitude, latitude);
    // データソースを巡回
    for (std::vector<OGRDataSource*>::iterator it = this->_ogrDataSources.begin();
	 it != this->_ogrDataSources.end(); it++) {
      OGRDataSource* poDataSource = (OGRDataSource*)(*it);
      // レイヤを取得
      int nLayers = poDataSource->GetLayerCount();
      // std::cerr << nLayers << " layers found." << std::endl;
      for (int iLayer = 0; iLayer < poDataSource->GetLayerCount(); iLayer++) {
        if (isInside > 0) break;
        OGRLayer* poLayer = poDataSource->GetLayer(iLayer); // Don't delete
        if (poLayer->GetLayerDefn()->GetGeomType() == wkbPolygon
	  || poLayer->GetLayerDefn()->GetGeomType() == wkbMultiPolygon) {
	  // std::cerr << "Layer " << iLayer << " is Polygon, " << OGRGeometryTypeToName(poLayer->GetLayerDefn()->GetGeomType()) << std::endl;
	  OGRFeature *poFeature;
	  poLayer->ResetReading();
	  while ((poFeature = poLayer->GetNextFeature()) != NULL) { // Must destroy
	    OGRGeometry* poGeometry = poFeature->GetGeometryRef();  // Don't delete
	    if (poGeometry->Contains(&targetPoint)) {
	      isInside = 1;
	    }
	    OGRFeature::DestroyFeature(poFeature);
	    if (isInside > 0) break;
	  }
	} else {
	  // std::cerr << "Layer " << iLayer << " is not Polygon, " << OGRGeometryTypeToName(poLayer->GetLayerDefn()->GetGeomType()) << std::endl;
	}
      }
    }
    result = (isInside == 0) ? -1.0 : 1.0;
    // std::cerr << result << std::endl;
#endif /* HAVE_LIBGDAL */
    return result;
  }

  /*****************************
   * SelectConditionGeoDisjoint
   *****************************/

  void SelectConditionGeoDisjoint::set(picojson::ext& options) {
#ifdef HAVE_LIBGDAL
    if (options.is_null("geo-disjoint")) return;
    const picojson::value& op_v = options.get_value("geo-disjoint");
    this->set_spatial_sources(op_v);
#endif /* HAVE_LIBGDAL */
  }

  double SelectConditionGeoDisjoint::judge(const Geoword* pGeoword) {
    double result = 1.0;
#ifdef HAVE_LIBGDAL
    // std::cerr << "#sources = " << this->_ogrDataSources.size() << std::endl;
    if (this->_ogrDataSources.size() == 0) return result;
    // 内外判定
    int isInside = 0;
    double longitude, latitude;
    std::stringstream sslat, sslon;
    sslat << pGeoword->get_latitude();
    sslat >> latitude;
    sslon << pGeoword->get_longitude();
    sslon >> longitude;
    OGRPoint targetPoint(longitude, latitude);
    // データソースを巡回
    for (std::vector<OGRDataSource*>::iterator it = this->_ogrDataSources.begin();
	 it != this->_ogrDataSources.end(); it++) {
      OGRDataSource* poDataSource = (OGRDataSource*)(*it);
      // レイヤを取得
      int nLayers = poDataSource->GetLayerCount();
      std::cerr << nLayers << " layers found." << std::endl;
      for (int iLayer = 0; iLayer < poDataSource->GetLayerCount(); iLayer++) {
        if (isInside > 0) break;
        OGRLayer* poLayer = poDataSource->GetLayer(iLayer); // Don't delete
        if (poLayer->GetLayerDefn()->GetGeomType() == wkbPolygon
	  || poLayer->GetLayerDefn()->GetGeomType() == wkbMultiPolygon) {
	  // std::cerr << "Layer " << iLayer << " is Polygon, " << OGRGeometryTypeToName(poLayer->GetLayerDefn()->GetGeomType()) << std::endl;
	  OGRFeature *poFeature;
	  poLayer->ResetReading();
	  while ((poFeature = poLayer->GetNextFeature()) != NULL) { // Must destroy
	    OGRGeometry* poGeometry = poFeature->GetGeometryRef();  // Don't delete
	    if (poGeometry->Contains(&targetPoint)) {
	      isInside = 1;
	    }
	    OGRFeature::DestroyFeature(poFeature);
	    if (isInside > 0) break;
	  }
	} else {
	  std::cerr << "Layer " << iLayer << " is not Polygon, " << OGRGeometryTypeToName(poLayer->GetLayerDefn()->GetGeomType()) << std::endl;
	}
      }
    }
    result = (isInside == 0) ? 1.0 : -1.0;
    // std::cerr << result << std::endl;
#endif /* HAVE_LIBGDAL */
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
    if (valid_from != "" && valid_from > this->_from_ymd) {
      isValid = -1.0;
    } else if (valid_to != "" && valid_to < this->_to_ymd) {
      isValid = -1.0;
    }

    return isValid;
  }

} /* namespace */
