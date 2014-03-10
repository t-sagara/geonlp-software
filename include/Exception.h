///
/// @file
/// @brief 例外の定義。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include <stdexcept>

namespace geonlp
{
	/// MeCabが初期化されていないことを示す例外。
	class MeCabNotInitializedException : public std::runtime_error {
	public:
		MeCabNotInitializedException(): runtime_error("MeCab not initialized.")
		{
		}
	};
	
	/// @brief MeCabのエラーが生じたことを示す例外。
	///
	/// MeCabのエラーメッセージが設定される。
	class MeCabErrException : public std::runtime_error {
	public:
		MeCabErrException(const char* msg): runtime_error(msg)
		{
		}
	};
	
	/// @link GeowordService @endlink の生成に失敗したことを示す例外。
	class ServiceCreateFailedException : public std::runtime_error {
	public:
		/// 生成に失敗した原因を示す。
		enum TYPE { 
			PROFILE, ///< プロファイル不正 
			SQLITE,  ///< Sqlite3初期化失敗
			MECAB,   ///< MeCab初期化失敗
			DARTS,   ///< DARTS初期化失敗
			GDBM,    ///< GDBM初期化失敗
			DAMS,    ///< ジオコーダ
			SERVICE  ///< 設定値不正等
		};
		/// 生成に失敗した原因
		TYPE type; 
		ServiceCreateFailedException(const char* msg, TYPE t): runtime_error(msg), type(t)
		{
		}
	};
}
#endif
