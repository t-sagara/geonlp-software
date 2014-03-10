///
/// @file
/// @brief MeCabにアクセスするためのクラスの定義。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _MECABADAPTER_H
#define _MECABADAPTER_H

#include <list>
#include <boost/shared_ptr.hpp>
#include <mecab.h>
#include "Exception.h"

namespace MeCab{
	class Tagger;
}

namespace geonlp
{
	
	class Node;

	/// @brief MeCabにアクセスするためのクラス。
	class MeCabAdapter {

	public:
		/// @brief @link parse() @endlinkの戻り値型定義。
		typedef std::list<Node> NodeList;
		
		/// @brief コンストラクタ。
		/// @arg @c userdic ユーザ辞書ファイル名
		MeCabAdapter(std::string& userdic): mecabp(NULL), userdic(userdic){};

		// 初期化。
		void initialize() throw(std::runtime_error);

		// 終了処理。
		void terminate();
	
	private:
		/// @brief MeCabのハンドラ。
		MeCab::Tagger* mecabp;
		
		/// @brief ユーザ辞書名
		std::string userdic;


	public:
		// パースする。
		NodeList parse(const std::string & sentence) throw(MeCabNotInitializedException, MeCabErrException);

	};
	
	typedef boost::shared_ptr<MeCabAdapter> MeCabAdapterPtr;
}
#endif
