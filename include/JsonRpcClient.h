///
/// @file   JsonRpcClient.h
/// @brief  分布情報管理サーバ通信用 JSON RPC クライアントの定義
/// @author 株式会社情報試作室
/// 
/// Copyright (c)2014, NII
/// 

#ifndef _GEONLP_JSONRPC_CLIENT_H
#define _GEONLP_JSONRPC_CLIENT_H

#include <vector>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "picojsonExt.h"

using boost::asio::ip::tcp;
namespace geonlp
{
  // 通信時例外
  class JsonRpcClientException : public std::runtime_error {

  public: 
    JsonRpcClientException(): runtime_error("jsonrpc communication failed") {}
    JsonRpcClientException(const std::string& message): runtime_error(message.c_str()) {}
  };

  // 通信インタフェース定義
  class JsonRpcClient {
  public:
    std::vector<std::string> headers_; // ヘッダのリスト
    std::string content_; // レスポンスコンテンツ

  private:
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;

  public:
    // コンストラクタ, JSON_RPC request の送信まで非同期で自動実行
    JsonRpcClient(boost::asio::io_service& io_service,
		  const std::string& server, const std::string& path, const std::string& message, const std::string& port = "80")
      throw (JsonRpcClientException, picojson::PicojsonException);

    // 受信したレスポンスをJSONオブジェクトとして取得
    picojson::ext getJsonResponse(void) throw (picojson::PicojsonException);
  
  private:
    // 非同期でイベントごとに実行されるメソッド群

    // 名前が解決された際に呼び出される（connect に進む）
    void handle_resolve(const boost::system::error_code& err,
			tcp::resolver::iterator endpoint_iterator);
    
    // 接続された際に呼び出される（write_request に進む）
    void handle_connect(const boost::system::error_code& err,
			tcp::resolver::iterator endpoint_iterator);

    // 送信完了時に呼び出される（read_status_line に進む）
    void handle_write_request(const boost::system::error_code& err);
    
    // ステータスライン受信時に呼び出される（read_headers に進む）
    void handle_read_status_line(const boost::system::error_code& err);

    // ヘッダ受信時に呼び出される（read_content に進む）
    void handle_read_headers(const boost::system::error_code& err);

    // コンテンツ受信時に呼び出される（送受信終了）
    void handle_read_content(const boost::system::error_code& err);
  
  }; /* class JsonRpcClient */

}

#endif  /* _GEONLP_JSONRPC_CLIENT_H */
