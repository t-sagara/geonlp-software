///
/// @file
/// @brief IPADIC辞書ファイルの生成に失敗した場合にスローされる例外クラス。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _MAKEDIC_EXCEPTION_
#define _MAKEDIC_EXCEPTION_

#include <stdexcept>

class MakedicException : public std::runtime_error {

public:

	/// @brief コンストラクタ
	/// 指定された詳細メッセージを使用して、新規例外を構築する。
	///
	/// @param [in] msg メッセージ
	MakedicException(const char* msg): runtime_error(msg) {}

};

#endif
