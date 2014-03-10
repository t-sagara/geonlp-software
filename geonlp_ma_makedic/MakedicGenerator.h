///
/// @file
/// @brief 地名語辞書からIPADIC辞書を作成する MakedicGenerator の最上位のビジネスロジッククラス。
/// @author Picolab Co.ltd
///
/// Copyright (c)2010-2011, NII
///

#ifndef _IPADIC_GENERATOR_
#define _IPADIC_GENERATOR_

#include <vector>
#include <string>
#include "MakedicException.h"
#include "MakedicGeneratorAppArgument.h"
#include "MakedicGeneratorConfiguration.h"
#include "MakedicItem.h"
#include "Wordlist.h"
#include "darts.h"

class MakedicGenerator {

private:

	std::string mecab_dict_index_path;

	std::string sysdic_directory_path;

	std::string geoword_hinshi_id;

	std::vector<std::string> example_sentences;

	bool deleteTempFileFlag;

	bool verboseMode;

	bool geowordFeatureCheckMode;

  geonlp::Profile profile;

  std::string csv_output_filename;
	
  /*
	std::string geodic_filepath;
  
  std::string darts_filepath;
	
	std::string ipadic_filepath;
  */
	
	// 一時ファイルのプレフィックス
	std::string tmpfile_prefix;
	
	void _read_db(std::vector<geonlp::Wordlist>& wordlists) const
	throw(MakedicException);
	
	std::string _create_tmpfile_name(const char* suffix, int counter = 0) const;
	
	void _create_ipadic_item(const geonlp::Wordlist& wordlist, const long& cost, MakedicItem& item) const;

  void _createDartsIndex(std::vector<geonlp::Wordlist>& wordlists) const;

public:

	/// @brief コンストラクタ
	///
	/// @param [in] conf 構成
	/// @param [in] profile プロファイル
	MakedicGenerator(
			const MakedicGeneratorConfiguration& conf, 
			const MakedicGeneratorAppArgument& arg);

	/// @brief 地名語辞書からIPADIC辞書を作成する。
	void generate(bool doUpdateWordlist = false) const throw (MakedicException);

};

#endif
