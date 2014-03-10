///
/// @file
/// @brief 引数が不正な場合にスローされるクラス。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _INVALID_ARGUMENT_EXCEPTION_
#define _INVALID_ARGUMENT_EXCEPTION_

#include <stdexcept>

class InvalidArgumentException : public std::runtime_error {

public:

	/// @brief コンストラクタ
	/// 指定された詳細メッセージを使用して、新規例外を構築する。
	///
	/// @param [in] msg メッセージ
	InvalidArgumentException(const char* msg): runtime_error(msg) {}

};

#endif
