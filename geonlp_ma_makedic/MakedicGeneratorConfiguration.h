///
/// @file
/// @brief MakedicGenerator(.exe)の構成定義クラス。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _IPADIC_GENERATOR_CONFIGURATION_
#define _IPADIC_GENERATOR_CONFIGURATION_

#include <boost/property_tree/ini_parser.hpp>
#include <string>
#include <vector>

class MakedicGeneratorConfiguration {

private:

	boost::property_tree::ptree conf_ini_file;

public:

	/// @brief コンストラクタ
	///
	/// @param [in] ini_file_path 読み込む ini ファイルへのパス
	MakedicGeneratorConfiguration(const std::string& ini_file_path);

	/// @brief mecab_dict_index へのパスを取得します。
	///
	/// @return mecab_dict_index へのパス
	std::string get_mecab_dict_index_path() const;

	/// @brief MeCabのシステム辞書があるディレクトリパスを取得します。
	///
	/// @return MeCabのシステム辞書があるディレクトリ
	//std::string get_sysdic_directory_path() const;

	/// @brief 一時ファイルを削除するか否かのフラグを取得します。
	///
	/// @return 一時ファイルを削除するか否かのフラグ
	bool get_delete_temp_file_flag() const;

	/// @brief 地名語判定用例文セットを取得します。
	///
	/// @return 地名語判定用例文セット
	std::vector<std::string> get_example_sentences() const;
};

#endif
