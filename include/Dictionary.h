///
/// @file
/// @brief テーマクラス Dictionary の定義。
/// @author 国立情報学研究所
///
/// Copyright (c)2010-2013, NII
///

#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <string>
#include "picojsonExt.h"

namespace geonlp
{
  class Dictionary : public picojson::ext {
  private:

  public:
    /// コンストラクタ
    Dictionary() { this->clear(); }
    
    /// 初期化
    void clear() { this->initByJson("{}"); }

    /// JSON からオブジェクトを復元する
    static Dictionary fromJson(const std::string& json_str) throw (picojson::PicojsonException);

    /// 必須項目が揃っていることを確認する
    bool isValid(void) const;
    bool isValid(std::string& err) const;
	
    /// 定義済み項目についてはメソッドを用意し、型のチェックを行う

    // 辞書識別子
    inline void set_identifier(const std::string& v) { this->_set_string("identifier", v); }
    inline std::string get_identifier(void) const throw (picojson::PicojsonException) { return this->_get_string("identifier"); }

    // 辞書内部ID
    // ローカル辞書は負の値を取ることもある
    inline void set_internal_id(int v) { this->_set_int("internal_id", v); }
    inline const int get_internal_id() const throw (picojson::PicojsonException) { return this->_get_int("internal_id"); }

    // 作成者
    inline void set_creator(const std::string& v) { this->_set_string("creator", v); }
    inline std::string get_creator() const throw (picojson::PicojsonException) { return this->_get_string("creator"); }
    
    // 辞書名
    inline void set_title(const std::string& v) { this->_set_string("title", v); }
    inline std::string get_title() const throw (picojson::PicojsonException) { return this->_get_string("title"); }

    // 辞書説明
    inline void set_description(const std::string& v) { this->_set_string("description", v); }
    inline std::string get_description() const throw (picojson::PicojsonException) { return this->_get_string("description"); }

    // 情報源
    inline void set_source(const std::string& v) { this->_set_string("source", v); }
    inline std::string get_source() const throw (picojson::PicojsonException) { return this->_get_string("source"); }

    // 修正報告件数
    inline void set_report_count(int v) { this->_set_int("report_count", v); }
    inline int get_report_count() const throw (picojson::PicojsonException) { return this->_get_int("report_count"); }

    // 公開日時
    inline void set_issued(const std::string& v) { this->_set_string("issued", v); }
    inline std::string get_issued() const throw (picojson::PicojsonException) { return this->_get_string("issued"); }

    // 修正日時
    inline void set_modified(const std::string& v) { this->_set_string("modified", v); }
    inline std::string get_modified() const throw (picojson::PicojsonException) { return this->_get_string("modified"); }

    // アイコン画像 URL
    inline void set_icon(const std::string& v) { this->_set_string("icon", v); }
    inline std::string get_icon() const throw (picojson::PicojsonException) { return this->_get_string("icon"); }

    // 辞書 URL
    inline void set_url(const std::string& v) { this->_set_string("url", v); }
    inline std::string get_url() const throw (picojson::PicojsonException) { return this->_get_string("url"); }
    
    // レコード数
    inline void set_record_count(int v) { this->_set_int("record_count", v); }
    inline int get_record_count() const throw (picojson::PicojsonException) { return this->_get_int("record_count"); }
    
    // 空間範囲
    void set_spatial(const double lng0, const double lat0, const double lng1, const double lat1);
    void set_spatial(const std::pair<std::pair<double, double>, std::pair<double, double> >& v);
    void get_spatial(double& lng0, double& lat0, double& lng1, double& lat1) const throw (picojson::PicojsonException);
    std::pair<std::pair<double, double>, std::pair<double, double> > get_spatial() const throw (picojson::PicojsonException);

    // 収録固有名クラス
    inline void set_subject(const std::vector<std::string>& v) { this->_set_string_list("subject", v); }
    inline std::vector<std::string> get_subject() const throw (picojson::PicojsonException) { this->_get_string_list("subject"); }

    /// 定義済み項目から取得可能な利便性の高い情報

    // 辞書作成者コード（識別子から取得）
    std::string get_user_code() const throw (picojson::PicojsonException);

    // 辞書コード（識別子から取得）
    std::string get_code() const throw (picojson::PicojsonException);

    // ファイルパス（識別子から取得）
    std::string get_path() const throw (picojson::PicojsonException);

  };

}

#endif
