///
/// @file
/// @brief 地名語の判定に失敗した時にスローされるクラス。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _GEOWORD_JUDGE_FAILED_EXCEPTION_
#define _GEOWORD_JUDGE_FAILED_EXCEPTION_

#include <stdexcept>

class GeowordJudgeFailedException : public std::runtime_error {

public:

	/// @brief コンストラクタ
	/// 指定された詳細メッセージを使用して、新規例外を構築する。
	///
	/// @param [in] msg メッセージ
	GeowordJudgeFailedException(const char* msg): runtime_error(msg) {}

};

#endif
