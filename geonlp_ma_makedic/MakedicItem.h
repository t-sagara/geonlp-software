///
/// @file
/// @brief MeCab アイテムクラス。
/// @author 国立情報学研究所
///
/// Copyright (c)2010, NII
///

#ifndef _MAKEDIC_ITEM_
#define _MAKEDIC_ITEM_

#include <string>

class MakedicItem {

private:

	std::string surface; // 表層形

	std::string leftContextId; // 左文脈ID

	std::string rightContextId; // 右文脈ID

	long cost; // コスト

	std::string partOfSpeech; // 品詞

	std::string subclassification1; // 品詞細分類1

	std::string subclassification2; // 品詞細分類2

	std::string subclassification3; // 品詞細分類3

	std::string conjugatedForm; // 活用形

	std::string conjugationType; // 活用型

	std::string originalForm; // 原形

	std::string yomi; // 読み

	std::string pronunciation; // 発音

public:

	/// @brief 表層形を取得します。
	///
	/// @return 表層形
	inline const std::string get_surface() const {return surface;}

	/// @brief 表層形を設定します。
	///
	/// @param [in] value 表層形
	inline void set_surface(std::string value) {surface = value;}

	/// @brief 左文脈IDを取得します。
	///
	/// @return 左文脈ID
	inline const std::string get_leftContextId() const {return leftContextId;}

	/// @brief 左文脈IDを設定します。
	///
	/// @param [in] value 左文脈ID
	inline void set_leftContextId(std::string value) {leftContextId = value;}

	/// @brief 右文脈IDを取得します。
	///
	/// @return 右文脈ID
	inline const std::string get_rightContextId() const {return rightContextId;}

	/// @brief 右文脈IDを設定します。
	///
	/// @param [in] value 右文脈ID
	inline void set_rightContextId(std::string value) {rightContextId = value;}

	/// @brief 生起コストを取得します。
	///
	/// @return 生起コスト
	inline const long get_cost() const {return cost;}

	/// @brief 生起コストを設定します。
	///
	/// @param [in] value 生起コスト
	inline void set_cost(long value) {cost = value;}

	/// @brief 品詞を取得します。
	///
	/// @return 品詞
	inline const std::string get_partOfSpeech() const {return partOfSpeech;}

	/// @brief 品詞を設定します。
	///
	/// @param [in] value 品詞
	inline void set_partOfSpeech(std::string value) {partOfSpeech = value;}

	/// @brief 品詞細分類1を取得します。
	///
	/// @return 品詞細分類1
	inline const std::string get_subclassification1() const {return subclassification1;}

	/// @brief 品詞細分類1を設定します。
	///
	/// @param [in] value 品詞細分類1
	inline void set_subclassification1(std::string value) {subclassification1 = value;}

	/// @brief 品詞細分類2を取得します。
	///
	/// @return 品詞細分類2
	inline const std::string get_subclassification2() const {return subclassification2;}

	/// @brief 品詞細分類2を設定します。
	///
	/// @param [in] value 品詞細分類2
	inline void set_subclassification2(std::string value) {subclassification2 = value;}

	/// @brief 品詞細分類3を取得します。
	///
	/// @return 品詞細分類3
	inline const std::string get_subclassification3() const {return subclassification3;}

	/// @brief 品詞細分類3を設定します。
	///
	/// @param [in] value 品詞細分類3
	inline void set_subclassification3(std::string value) {subclassification3 = value;}

	/// @brief 活用形を取得します。
	///
	/// @return 活用形
	inline const std::string get_conjugatedForm() const {return conjugatedForm;}

	/// @brief 活用形を設定します。
	///
	/// @param [in] value 活用形
	inline void set_conjugatedForm(std::string value) {conjugatedForm = value;}

	/// @brief 活用型を取得します。
	///
	/// @return 活用型
	inline const std::string get_conjugationType() const {return conjugationType;}

	/// @brief 活用型を設定します。
	///
	/// @param [in] value 活用型
	inline void set_conjugationType(std::string value) {conjugationType = value;}

	/// @brief 原形を取得します。
	///
	/// @return 原形
	inline const std::string get_originalForm() const {return originalForm;}

	/// @brief 原形を設定します。
	///
	/// @param [in] value 原形
	inline void set_originalForm(std::string value) {originalForm = value;}

	/// @brief 読みを取得します。
	///
	/// @return 読み
	inline const std::string get_yomi() const {return yomi;}

	/// @brief 読みを設定します。
	///
	/// @param [in] value 読み
	inline void set_yomi(std::string value) {yomi = value;}

	/// @brief 発音を取得します。
	///
	/// @return 発音
	inline const std::string get_pronunciation() const {return pronunciation;}

	/// @brief 発音を設定します。
	///
	/// @param [in] value 発音
	inline void set_pronunciation(std::string value) {pronunciation = value;}
};

#endif
