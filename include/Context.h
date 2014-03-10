///
/// @file
/// @brief  コンテキスト情報 Context の定義
/// @author 株式会社情報試作室
///
/// Copyright (c)2013, NII
///

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <string>
#include <vector>
#include <map>
#include "picojson.h"
#include "picojsonExt.h"
#include "Geoword.h"
#include "Address.h"

namespace geonlp
{
  
  // 例外クラス
  class ContextException:public std::runtime_error {
  public:
    ContextException(const std::string& message): runtime_error(message) {}
    ~ContextException() throw() {}
  };

  /// クラス、辞書などが共通する地名語の関係を管理するクラス
  class ContextRelation {
  private:
    // キー key が共通の地名語を束ねるキーチェーン
    // 値には地名語の geonlp_id ごとにまとめた出現箇所のリストとして格納する
    // 出現箇所は「文の先頭から n 番目の単語」の「m 番目の候補」を
    // std::pair<n, m> という形式で保持する
    std::map<std::string, std::map<std::string, std::vector<std::pair<int, int> > > > _chain;

  public:
    // コンストラクタ
    ContextRelation() {}
    
    // 地名語情報を登録する（キーが文字列の場合）
    void add(const std::string& key, const std::string& geonlp_id, int n, int m);

    // 地名語情報を登録する（キーが数値の場合）
    void add(int key, const std::string& geonlp_id, int n, int m);

    // 地名語情報を取得する（キーが文字列の場合）
    std::map<std::string, std::vector<std::pair<int, int> > >& get(const std::string& key)
      throw (ContextException);

    // 地名語情報を取得する（キーが数値の場合）
    std::map<std::string, std::vector<std::pair<int, int> > >& get(int key)
      throw (ContextException);

    // @brief 地名語数を取得する（キーが文字列の場合）
    // @arg key キー
    // @arg self_id   この geonlp_id を持つ地名語はカウントしない（自分自身を除く）
    // @arg self_pos  n == self_pos となる地名語はカウントしない（自分自身を除く）
    // @arg lb  カウントする n の下限（lb 以上の n を持つエントリのみカウントする）
    // @arg hb  カウントする n の上限（hb 以下の n を持つエントリのみカウントする）
    int count(const std::string& key, const std::string& self_id, int self_pos, int lb = -1, int hb = -1) const;

    // @brief 地名語数を取得する（キーが数値の場合）
    // @arg key キー
    // @arg self_id   この geonlp_id を持つ地名語はカウントしない（自分自身を除く）
    // @arg self_pos  n == self_pos となる地名語はカウントしない（自分自身を除く）
    // @arg lb  カウントする n の下限（lb 以上の n を持つエントリのみカウントする）
    // @arg hb  カウントする n の上限（hb 以下の n を持つエントリのみカウントする）
    int count(int key, const std::string& self_id, int self_pos, int lb = -1, int hb = -1) const;

    // キーチェーンを空にする
    void clear(void);

    // 指定した n 以下の地名語をエクスパイアする
    void expire(int n);
  };

  
  /// 地名語解決用コンテキストクラス
  class Context {
  private:
    // 地名語候補との関連
    ContextRelation _context_neclass;       // ne_class が共通
    ContextRelation _context_dictionary;    // dictionary_id が共通
    ContextRelation _context_hypernym;      // hypernym のいずれかが共通
    ContextRelation _context_full_hypernym; // hypernym のすべてが共通
    ContextRelation _context_name;          // typical_name が共通

    // 選択された地名語候補との関連
    ContextRelation _selected_neclass;
    ContextRelation _selected_dictionary;
    ContextRelation _selected_hypernym;
    ContextRelation _selected_full_hypernym;
    ContextRelation _selected_name;

    // 空間的中心座標
    float _cumulative_lat;
    float _cumulative_lon;
    int   _cumulative_points;

    picojson::array _nodes;                 // parseNode の結果、全地名語候補を含む
    picojson::ext _options;          // parse オプション

    // @brief Geoword を一つコンテキスト関係に登録する
    // @arg geoword 地名語
    // @arg n       この地名語が出現した位置（文の先頭から数えた単語数）
    // @arg m       この地名語が同綴語の中で何番目の候補か
    void addGeowordToContextRelations(const Geoword& geoword, int n, int m);

    // @brief 指定した位置にある Geoword の出現スコアを計算する
    // @arg geoword 地名語
    // @arg n       この地名語が出現した位置（文の先頭から数えた単語数）
    // @arg m       この地名語が同綴語の中で何番目の候補か
    int score(const Geoword& geoword, int n, int m) const;

    // @brief Geoword を一つ選択済みコンテキスト関係に登録する
    // @arg geoword 地名語
    // @arg n       この地名語が出現した位置（文の先頭から数えた単語数）
    // @arg m       この地名語が同綴語の中で何番目の候補か
    void addGeowordToSelectedRelations(const Geoword& geoword, int n, int m);

    // @brief 指定した位置にある Geoword の
    // 選択済みコンテキストに対する出現スコアを計算する
    // @arg geoword 地名語
    // @arg n       この地名語が出現した位置（文の先頭から数えた単語数）
    // @arg m       この地名語が同綴語の中で何番目の候補か
    int selectedScore(const Geoword& geoword, int n, int m) const;

    // @brief 住所要素をコンテキストに登録する
    // @arg varray  住所候補を含む配列
    // @arg n       この語が出現した位置（文の先頭から数えた単語数）
    void _addAddresses(const picojson::array& varray, int n);

    // @brief 地名語候補リストをコンテキストに登録する
    // @arg varray  地名語候補を含む配列
    // @arg n       この語が出現した位置（文の先頭から数えた単語数）
    void _addGeowords(const picojson::array& varray, int n);

    // @brief AddressElement を一つ空間関係に登録する
    // @arg elem    住所要素
    // @arg size    この住所が出現した単語にいくつの候補が含まれるか
    void addAddressElementToSpatialRelations(const picojson::value& elem, int size);

    // @brief 同綴地名語をまとめて空間関係に登録する
    // @arg geowords 同綴地名語集合を含む picojson::array
    void addGeowordsToSpatialRelations(const picojson::array& geowords);

  public:
    // コンストラクタ
    Context() {
      this->clear();
      this->_options.initByJson("{}");
    }

    // parse オプションセット
    inline void setOptions(const picojson::ext& options) { this->_options = options; }

    // コンテキスト情報のクリア
    void clear(void);

    // 地名語解析結果の追加
    void addNodes(const picojson::array& nodes);

    // 登録済みの地名語候補のスコアを評価
    void evaluate(void);

    // 解決後の結果を取得
    picojson::array flushNodes(void);

    // 登録完了後の計算済み重心を取得
    int getCentroid(float& lat, float& lon) const;

  };

}
#endif /* _CONTEXT_H */
