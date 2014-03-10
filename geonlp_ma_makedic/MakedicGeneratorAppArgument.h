///
/// @file
/// @brief MakedicGenerator(.exe)の起動引数の解析クラス。
/// @author Picolab Co.ltd
///
/// Copyright (c)2010, NII
///

#ifndef _MAKEDIC_GENERATOR_APP_ARGUMENT_
#define _MAKEDIC_GENERATOR_APP_ARGUMENT_

#include <string>
#include <set>
#include "InvalidArgumentException.h"
#include "Profile.h"

class MakedicGeneratorAppArgument {

private:

	std::set<std::string> options;
	std::string opt_ini_file_path;
        std::string opt_geoword_hinshi_id;
  std::string opt_csv_output_filename;
	geonlp::Profile profile;
	
public:

	/// @brief コンストラクタ
	///
	/// @param [in] argc main 関数の引数 argc と同等
	/// @param [in] argv main 関数の引数 argv と同等
	MakedicGeneratorAppArgument(int argc, const char **argv)
	throw (InvalidArgumentException);

	/// @brief オプションの有無を判定します。
	///
	/// @param [in] key       オプションの値
	/// @return オプションが有れば true、そうでなければ false
	bool hasOption(const std::string& key) const;

	/// @brief -fオプションが指定された場合のオプション値を取得します。
	///
	/// @return -fオプション値
	std::string getOptIniFilePath() const { return opt_ini_file_path; };

  /// @brief -dオプションが指定された場合のオプション値を取得します。
  /// 
  /// @return -dオプション値
  std::string getOptGeowordHinshiId() const { return opt_geoword_hinshi_id; };
	
  /// @brief -oオプションが指定された場合のオプション値を取得します。
  /// 
  /// @return -dオプション値(CSV出力ファイル名）
  std::string getOptCsvOutputFilename() const { return opt_csv_output_filename; };
	
	/// @brief プロファイルを取得します。
	///
	/// @return プロファイル
	const geonlp::Profile& getProfile() const { return profile; };
};

#endif
