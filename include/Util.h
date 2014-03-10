///
/// @file
/// @brief ユーティリティクラスの定義。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _UTIL_H
#define _UTIL_H

#include <string>
#include <vector>

namespace geonlp
{
	/// @brief ユーティリティクラス。
	///
	class Util {
	private:

	public:
		// ひらがなカタカナのみからなる文字列であればカタカナに変換して返す。
		static std::string checkKana( const std::string& );
		
		// 引数文字列をデリミタで区切って文字列の配列として返す。
		static void explode( const std::string& src, std::vector<std::string>& strlist, const char delim = ',');

	};
	
}
#endif
