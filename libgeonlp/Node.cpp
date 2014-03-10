///
/// @file
/// @brief 形態素情報クラスNodeの実装。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include "Node.h"

namespace geonlp
{

	const std::string Node::delim = ",";
	
	// コンストラクタ。
	// 引数に与えられたfeatureを素性情報に分解し、各メンバに設定する。
	// @arg @c surface 形態素の文字列情報(表層形)
	// @arg @c feature MeCab::Nodeの持つfeature。CSV で表記された素性情報。
	Node::Node( const std::string& surface, const std::string& feature)
	{
		set_surface( surface);
		this->feature = feature;
		
		// featureを分解
		std::vector<std::string> strlist;
		boost::split(strlist, feature, boost::is_any_of(","));
		
		// nodeの各フィールドを設定
		set_partOfSpeech( (strlist.size() > 0)? strlist[0]: "");
		set_subclassification1( (strlist.size() > 1)? strlist[1]: "");
		set_subclassification2( (strlist.size() > 2)? strlist[2]: "");
		set_subclassification3( (strlist.size() > 3)? strlist[3]: "");
		set_conjugatedForm( (strlist.size() > 4)? strlist[4]: "");
		set_conjugationType( (strlist.size() > 5)? strlist[5]: "");
		set_originalForm( (strlist.size() > 6)? strlist[6]: "");
		set_yomi( (strlist.size() > 7)? strlist[7]: "");
		set_pronunciation( (strlist.size() > 8)? strlist[8]: "");
	}
	
	// デバグ用のテキスト表記を得る。
	// 書式はMeCabのデフォルトに準じる。
	std::string Node::toString() const
	{
		std::ostringstream oss;
		oss << get_surface() << "\t";
		oss << get_partOfSpeech() << delim;
		oss << get_subclassification1() << delim;
		oss << get_subclassification2() << delim;
		oss << get_subclassification3() << delim;
		oss << get_conjugatedForm() << delim;
		oss << get_conjugationType() << delim;
		oss << get_originalForm() << delim;
		oss << get_yomi() << delim;
		oss << get_pronunciation();
		return oss.str();
	}
	
}
