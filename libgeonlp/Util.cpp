///
/// @file
/// @brief ユーティリティクラスの実装。
/// @author 国立情報学研究所
///
/// Copyright (c)2010-2013, NII
///
#include "Util.h"

namespace
{
	struct TmpArray{
		unsigned char* p;
		TmpArray( const std::string& src): p(NULL){
			p = new unsigned char[src.length()];
		};
		~TmpArray(){
			delete[] p;
		};
	};
}

namespace geonlp
{
	/// @brief 引数がカタカナとひらがなのみの場合、ひらがなをカタカナに変換した文字列を返す。
	///
	/// 引数にカタカナひらがな意外の文字が含まれている場合には空文字列を返す。
	/// @arg @c src 変換対象の文字列
	/// @return カタカナに変換された文字列（あるいは空文字列）
	/// @note UTF8前提のコードである。
	std::string Util::checkKana( const std::string& src)
	{
		std::string emptystring;
		if ( src.length() % 3 != 0) return emptystring;
		::TmpArray ta( src);
		const unsigned char* ucp = reinterpret_cast<const unsigned char*>(src.data());
		for ( size_t i = 0; i< src.length(); i += 3){
			if ( ucp[i] != 0xE3) return emptystring;
			ta.p[i]= ucp[i];
			switch( ucp[i+1]){
				case 0x81:
					if ( 0x81 <= ucp[i+2] && ucp[i+2] <= 0x9F){
						// ぁ〜た
						ta.p[i+1]= 0x82;
						ta.p[i+2]= ucp[i+2] - 0x81 + 0xA1;
					}else if ( 0xA0 <= ucp[i+2] && ucp[i+2] <= 0xBF){
						// だ〜み
						ta.p[i+1]= 0x83;
						ta.p[i+2]= ucp[i+2] - 0xA0 + 0x80;
					}else{
						return emptystring;
					}
					break;
				case 0x82:
					if ( (0x80 <= ucp[i+2] && ucp[i+2] <= 0x96) || (0x9D <= ucp[i+2] && ucp[i+2] <= 0x9F)){
						// む〜んゔゕゖ || ゝゞ	ゟ
						ta.p[i+1]= 0x83;
						ta.p[i+2]= ucp[i+2] - 0x80 + 0xA0;
					}else if ( 0xA0 <= ucp[i+2] && ucp[i+2] <= 0xBF){
						// ゠ァ〜タ
						ta.p[i+1]= ucp[i+1];
						ta.p[i+2]= ucp[i+2];
					}else if ( 0x99 <= ucp[i+2] && ucp[i+2] <= 0x9C){
						// ゛゜
						ta.p[i+1]= ucp[i+1];
						ta.p[i+2]= ucp[i+2];
					}else{
						return emptystring;
					}
					break;
				case 0x83:
					if ( 0x80 <= ucp[i+2] && ucp[i+2] <= 0xBF){
						// ダ〜ヶヷヸヹヺ・ーヽヾヿ
						ta.p[i+1]= ucp[i+1];
						ta.p[i+2]= ucp[i+2];
					}else{
						return emptystring;
					}
					break;
				case 0x87:
					if ( 0xB0 <= ucp[i+2] && ucp[i+2] <= 0xBF){
						// ㇰㇱㇲㇳㇴㇵㇶㇷㇸㇹㇺㇻㇼㇽㇾㇿ(Unicode 片仮名の音声用用拡張)
						ta.p[i+1]= ucp[i+1];
						ta.p[i+2]= ucp[i+2];
					}else{
						return emptystring;
					}
					break;
				default:
					return emptystring;
			}
		}
		return std::string( reinterpret_cast<const char*>(ta.p), src.length());
	}
	
	/// @brief 引数文字列をデリミタで区切って文字列の配列として返す。
	///
	/// @arg @c src [in] 入力文字列
	/// @arg @c strlist [out] 分解された文字列の配列
	/// @arg @c delim デリミタとなるキャラクタ
	void Util::explode( const std::string& src, std::vector<std::string>& strlist, const char delim)
	{
		std::vector<std::string>::size_type delimpos;
		std::string temp = src;
		strlist.clear();
		while( (delimpos = temp.find_first_of(delim)) != temp.npos )
		{
			if(delimpos > 0)
			{
				strlist.push_back(temp.substr(0, delimpos));
			}
			temp = temp.substr(delimpos + 1);
		}
		if(temp.length() > 0)
		{
			strlist.push_back(temp);
		}
	}
}
