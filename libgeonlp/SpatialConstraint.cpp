///
/// @file
/// @brief  空間的制約 SpatialConstraint の実装
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
#include "SpatialConstraint.h"

namespace geonlp
{
  
  // データソースを追加する
  int SpatialConstraint::add_data_source(const std::string& url) {
#ifdef HAVE_LIBGDAL
    OGRSFDriver* poDriver;
    OGRDataSource* poDataSource;
    poDataSource = OGRSFDriverRegistrar::Open(url.c_str(), FALSE, &(poDriver));
    if (poDataSource == NULL) {
      std::stringstream ss;
      ss << "Fail to read spatial-constraint data, url:'" << url << "'";
      throw SpatialConstraintException(ss.str());
    }
    // std::cerr << "Successfully read " << url << "." << std::endl;
    this->_ogrDataSources.push_back(poDataSource);
#endif /* HAVE_LIBGDAL */
  }

  // GeoJSON を追加する
  int SpatialConstraint::add_geojson_source(const picojson::value& v) {
    std::string tmp_filename = std::tmpnam(NULL);
    tmp_filename = tmp_filename + ".geojson";
    std::ofstream ofs;
    ofs.open(tmp_filename.c_str(), std::ios::out);
    ofs << v.serialize();
    ofs.close();
    this->_tmpfiles.push_back(tmp_filename);
    this->add_data_source(tmp_filename);
  }

  // JSON オプションパラメータから条件を構築
  void SpatialConstraint::set(const picojson::value& options) {
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
    }
#endif /* HAVE_LIBGDAL */
    if (options.is<std::string>()) {
      // 文字列が一つの場合は外部ファイルの URL を指定
      this->add_data_source(options.to_str());
    } if (options.is<picojson::object>()) {
      // オブジェクト一つの場合は GeoJSON として追加
      this->add_geojson_source(options);
    } else if (options.is<picojson::array>()) {
      // 配列の場合は、複数の外部ファイルもしくは GeoJSON を指定
      picojson::array sources = options.get<picojson::array>();
      for (picojson::array::iterator it = sources.begin(); it != sources.end(); it++) {
	picojson::value& v = (*it);
	if (v.is<std::string>()) {
	  // 外部ファイルの URL
	  this->add_data_source(v.to_str());
	} else if (v.is<picojson::object>()) {
	  // GeoJSON, 一時ファイルに保存する
	  this->add_geojson_source(v);
	}
      }
    }
  }


  // Geoword が制約を満たすかどうかを判定する
  // 0または正の値を返した場合にはスコアに乗じる重み
  // 負の値を返した場合には制約条件を満たさない
  double SpatialConstraint::judge(const Geoword* pGeoword) {
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

} /* namespace */
