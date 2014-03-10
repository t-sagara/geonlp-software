///
/// @file
/// @brief  地名解決API（インタフェース）の定義
/// @author 株式会社情報試作室
///
/// Copyright (c)2013, NII
///

#ifndef _GEONLP_SERVICE_H
#define _GEONLP_SERVICE_H

#include "config.h"
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include "Profile.h"
#include "Geoword.h"
#include "Dictionary.h"
#include "Address.h"
#include "GeonlpMA.h"
#include "Exception.h"
#include "picojsonExt.h"
#include "Context.h"
#include "Classifier.h"

#ifdef HAVE_LIBDAMS
#include <dams.h>
#endif /* HAVE_LIBDAMS */

namespace geonlp
{
  // リクエストフォーマットが不正の場合に発生する例外
  class ServiceRequestFormatException : public std::runtime_error {
  public:
    ServiceRequestFormatException(): runtime_error("Invalid request format") {}
    ServiceRequestFormatException(const std::string& message): runtime_error(message.c_str()) {}
  };

  /// @brief Service のインタフェース定義
  class Service {
  private:
    MAPtr _ma_ptr;
    boost::shared_ptr<Profile> _profilesp;
    picojson::ext _options;
    Context _context;
    Classifier _classifier;

  protected:
    /// @brief オプションをセットする
    /// @arg @c options  オプション指定 json オブジェクト
    /// @return なし
    /// @exception PicojsonException  オプション json 解析処理時のエラー
    /// @exception ServiceRequestFormatException オプション不正時のエラー
    void set_options(const picojson::value& options) throw (picojson::PicojsonException, ServiceRequestFormatException);

    /// @brief オプションをリセットする
    /// @return なし
    void reset_options(void);

    /// @brief  コンテキスト情報をリセットする
    /// @arg    なし
    /// @return なし
    void reset_context(void) { this->_context.clear(); }
    
    /// @brief １文を現在のコンテキスト、オプションのままで解析する
    /// @arg @c sentence  解析する自然言語文
    /// @return 解析結果の JSON オブジェクトの配列
    picojson::value parse_sentence(const std::string& sentence);

    /// @brief １文をキューに積む
    /// @arg @c sentence  解析する自然言語文
    void queue_sentence(const std::string& sentence);

    /// @brief キューに積まれた文集合を評価し、地名解決を行う
    void resolve(void);

    /// @brief 解決済みの１文をキューから取り出す
    /// @return 解析結果の JSON オブジェクトの配列
    picojson::value dequeue_sentence(void);

#ifdef HAVE_LIBDAMS
    /// @brief 住所文字列をジオコーディングする
    /// @arg   address  ジオコーディング結果を格納する Address オブジェクト
    /// @arg   it_s     住所の可能性のある Node 配列の先頭を指すイテレータ
    /// @arg   it_e     住所の可能性のある Node 配列の最後尾を指すイテレータ
    /// 住所として結果を出力する場合、
    ///  (1) address["address"] に住所要素のハッシュをセットし、
    ///  (2) it_s を必要な分進め、
    ///  (3) true を返す
    bool tryDams(std::vector<geonlp::Address>& addresses, std::vector<Node>::iterator& it_s, const std::vector<Node>::iterator &it_e) const
      throw (damswrapper::DamsException);

    /// @brief  ジオコーディング結果を Address に積み替える
    /// @arg    address    ジオコーディング結果を格納する Address オブジェクト
    /// @arg    surface    surface に格納すべき文字列
    /// @arg    candidate  DAMS が返すジオコーディング結果（複数存在する場合も一つだけ）
    /// @return 積み込んだ住所要素数（階層の深さ）
    int damsCandidateToAddress(Address& address, const std::string& surface, const damswrapper::Candidate& candidate) const;
#endif /* HAVE_LIBDAMS */
    
  public:
    // コンストラクタ
    Service(MAPtr maptr, boost::shared_ptr<Profile> profilesp)
      :_classifier(profilesp->get_log_dir() + "classify.log") { 
      this->_ma_ptr = maptr;
      this->_context.clear();
      this->_profilesp = profilesp;
      this->reset_options(); // コンテキストのオプションも初期化される
    }

    inline std::string version(void) { return std::string(PACKAGE_VERSION); }

    // 拡張形態素解析器 (MA）インタフェースの取得
    inline MAPtr getMA(void) const { return this->_ma_ptr; }

    /// @brief JSON-RPC のリクエストを受け取って実行する
    /// @param @c json_request  リクエストオブジェクト
    /// @return JSON-RPC のレスポンスオブジェクト
    picojson::value proc(const picojson::value& json_request);

    /// @brief バージョン番号を返す
    /// @return バージョン
    /// @exception PicojsonException  json 解析処理時のエラー
    picojson::value version(const picojson::array& params) 
      throw (picojson::PicojsonException);

    /// @brief 自然言語文を解析し、地名語を抽出した結果を返す
    /// @arg @c params 解析する自然言語文＋オプション
    /// @return 解析結果
    /// @exception PicojsonException  json 解析処理時のエラー
    /// @exception ServiceRequestFormatException  リクエストフォーマットの不正
    picojson::value parse(const picojson::array& params) 
      throw (picojson::PicojsonException, ServiceRequestFormatException);

    /// @brief 他のタグ付け器等で構造化されているテキスト、または一連のテキストのジオパース
    ///        コンテキストは相互に影響する
    /// @arg @c params 解析する構造化テキスト＋オプション
    /// @return 解析結果
    /// @exception PicojsonException  json 解析処理時のエラー
    /// @exception ServiceRequestFormatException  リクエストフォーマットの不正
    picojson::value parseStructured(const picojson::array& params)
      throw (picojson::PicojsonException, ServiceRequestFormatException);

    /// @brief 表記または読みから地名語を検索する
    /// @arg @c params 表記又は読み＋オプション
    /// @return 検索結果
    /// @exception PicojsonException  json 解析処理時のエラー
    /// @exception ServiceRequestFormatException  リクエストフォーマットの不正
    picojson::value search(const picojson::array& params)
      throw (picojson::PicojsonException, ServiceRequestFormatException);

    /// @brief geonlp_id から地名語の詳細情報を取得する
    /// @arg @c params 第一パラメータに geonlp_id または geonlp_id の配列
    /// @return 検索結果, 見つからなかった場合は isValid() で false を返す
    /// @exception ServiceRequestFormatException  リクエストフォーマットの不正
    picojson::value getGeoInfo(const picojson::array& params) const 
      throw (ServiceRequestFormatException);
    
    /// @brief 利用可能な辞書一覧を取得する
    /// @arg params  サイズ0
    /// @return 辞書の配列
    picojson::value getDictionaries(const picojson::array& params) const
      throw (ServiceRequestFormatException);

    /// @brief 指定した辞書に関する詳細情報を取得する
    /// @arg @c params  第一パラメータに詳細情報が必要な辞書ID, または辞書IDの配列
    /// @return 辞書のIDをキー、辞書の詳細情報を値とするマップ
    picojson::value getDictionaryInfo(const picojson::array& params)
      throw (ServiceRequestFormatException);

    /// @brief 利用可能なコード体系の一覧を取得する
    /// @arg @c pattern  検索対象となるコード体系の正規表現
    /// @return 正規表現に一致するコード体系の一覧
    picojson::value getCodeKeys(const std::string& pattern) const;

    /// @brief  コード体系とコード値を含む地名語の情報を取得する
    /// @arg @c code-pairs      コード体系, コード値のペアの配列
    /// @arg @c option_json_str 処理オプションを指定する JSON 文字列
    /// @return コード体系、コード値を含む地名語の配列
    /// @exception PicojsonException  json 解析処理時のエラー
    picojson::value getGeoFromCodes(const std::vector<std::pair<std::string, std::string> >& codepairs, const std::string& option_json_str) const
      throw (picojson::PicojsonException);

#ifdef HAVE_LIBDAMS
    /// @brief  住所をジオコーディングする
    /// @arg @c @params 第一パラメータに住所文字列、または住所文字列の配列
    /// @return 住所要素の GeoJSON 形式オブジェクト、もしくはその配列
    picojson::value addressGeocoding(const picojson::array& params)
      throw (ServiceRequestFormatException);
#endif /* HAVE_LIBDAMS */
  }; /* class Service */

  // Service へのポインタ
  typedef boost::shared_ptr<Service> ServicePtr;

  /// @brief Service への共有ポインタ取得する
  /// @arg @c profile プロファイル名
  /// @exception ServiceCreateFailedException  ポインタ取得失敗
  ServicePtr createService(const std::string& profile = PACKAGE_NAME)
    throw (ServiceCreateFailedException);

}

#endif /* _GEONLP_SERVICE_H */
