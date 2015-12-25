///
/// @file
/// @brief  空間的条件 SpatialCondition の定義
/// @author 株式会社情報試作室
///
/// Copyright (c)2015, NII
///

#ifndef _SPATIAL_CONDITION_H
#define _SPATIAL_CONDITION_H

#include <string>
#include <vector>
#include "Geoword.h"
#include "picojson.h"

#ifdef HAVE_LIBGDAL
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "ogr_feature.h"
#include "ogr_geometry.h"
#endif /* HAVE_LIBGDAL */

namespace geonlp
{
  // 例外クラス
  class SpatialConditionException:public std::runtime_error {
  public:
  SpatialConditionException(const std::string& message): runtime_error(message) {}
    ~SpatialConditionException() throw() {}
  };

  // 空間的な検索条件、重みづけなどの制約を管理するクラス
  class SpatialCondition {
  private:

#ifdef HAVE_LIBGDAL
    // 境界条件を表わすデータ
    std::vector<OGRDataSource*> _ogrDataSources;
    int add_data_source(const std::string& url);
    int add_geojson_source(const picojson::value& v);
    std::vector<std::string> _tmpfiles;
#endif /* HAVE_LIBGDAL */    

  public:
    // コンストラクタ
    SpatialCondition();

    // デストラクタ
    ~SpatialCondition();

    // JSON オプションパラメータから条件を構築
    void set(const picojson::value& options);

    // Geoword が制約を満たすかどうかを判定する
    // 0または正の値を返した場合にはスコアに乗じる重み
    // 負の値を返した場合には制約条件を満たさない
    double judge(const Geoword*);

  }; /* class */

} /* namespace */
#endif /* _SPATIAL_CONDITION_H */
