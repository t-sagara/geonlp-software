///
/// @file
/// @brief CSV形式のIPADIC定義ファイル向け入出力クラス。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _MAKEDIC_CSV_FILE_IO_
#define _MAKEDIC_CSV_FILE_IO_

#include <vector>
#include "MakedicItem.h"

class MakedicCsvFileIO {

public:

	/// @brief 引数で与えられた辞書コンテンツを引数で与えられたファイルに書き込みます。
	///
	/// @param [in] path 書き込み先ファイルパス
	/// @param [in] contents 書き込むコンテンツ
	static void write(const std::string& path, const std::vector<MakedicItem>& contents);

};

#endif
