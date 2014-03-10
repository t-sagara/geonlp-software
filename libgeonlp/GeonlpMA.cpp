///
/// @file
/// @brief 地名語抽出API(インタフェース)の実装。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///
#include <config.h>
#include <iostream>
#ifdef HAVE_CONFIG_H
#include <stdlib.h>
#endif /* HAVE_CONFIG_H */
#include "GeonlpMA.h"
#include "Profile.h"
#include "MeCabAdapter.h"
#include "DBAccessor.h"
#include "GeonlpMAImplSq3.h"

/// @brief プロファイル定義ファイルのデフォルトディレクトリ
/// @note configure 時の prefix に合わせて Makefile 中に定義される
//#define PROFILE_DEFAULT_DIR_PATH "/usr/local/etc/"

namespace geonlp
{
  // MAインタフェースを取得する。
  // @arg @c profile プロファイル名。
  // @exception ServiceCreateFailedException 取得失敗。	
  MAPtr createMA(const std::string& profile) throw(ServiceCreateFailedException)
  {
    // Profileの読み込み
    ProfilePtr profilesp = ProfilePtr(new Profile());
    std::string profilepath = Profile::searchProfile(profile);
    try{
      profilesp->load(profilepath);
    } catch( std::runtime_error& e){
      throw ServiceCreateFailedException(e.what(), ServiceCreateFailedException::PROFILE);
    }
		
    // MeCabAdapterの初期化
    MeCabAdapterPtr mecabp;
    try{
      std::string userdic = profilesp->get_mecab_userdic();
      mecabp = MeCabAdapterPtr(new MeCabAdapter( userdic));
      mecabp->initialize();
    }catch( std::runtime_error& e){
      throw ServiceCreateFailedException( e.what(), ServiceCreateFailedException::MECAB);
    }
		
    // DBAccessorの初期化
    // std::string dbfilename;
    DBAccessorPtr dbap;
    try{
      dbap = DBAccessorPtr(new DBAccessor(*profilesp));
      dbap->open();
    }catch( std::runtime_error& e){
      throw ServiceCreateFailedException( e.what(), ServiceCreateFailedException::SQLITE);
    }
		
    // Dartsの初期化
    std::string darts;
    DoubleArrayPtr dap;
    try {
      darts = profilesp->get_darts_file();
      if (darts.length() > 0) {
	dap = DoubleArrayPtr(new Darts::DoubleArray());
	dap->open(darts.c_str());
      }
    } catch (std::runtime_error& e) {
      throw ServiceCreateFailedException(e.what(), ServiceCreateFailedException::DARTS);
    }

    try{
      MAPtr servicep = MAPtr(new MAImpl( mecabp, dbap, dap, profilesp));
      return servicep;
    } catch( std::runtime_error& e){
      throw ServiceCreateFailedException( e.what(), ServiceCreateFailedException::SERVICE);
    }
  }

}
