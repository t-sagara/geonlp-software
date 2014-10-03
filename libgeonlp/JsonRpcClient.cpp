///
/// @file   JsonRpcClient.cpp
/// @brief  分布情報管理サーバ通信用 JSON RPC クライアントの実装
/// @author 株式会社情報試作室
/// 
/// Copyright (c)2014, NII
/// 

#include <sstream>
#include "JsonRpcClient.h"

using boost::asio::ip::tcp;
namespace geonlp
{

  // コンストラクタ
  // @param server   サーバのホスト名
  // @param path     接続する URL のパス
  // @param message  ポストするメッセージ
  JsonRpcClient::JsonRpcClient(boost::asio::io_service& io_service,
			       const std::string& server, const std::string& path, const std::string& message, const std::string& port)
    throw (JsonRpcClientException, picojson::PicojsonException)
    :resolver_(io_service), socket_(io_service) {

    headers_.clear();
    content_ = "";
    
    std::ostream request_stream(&request_);
    request_stream << "POST " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    if (message.length() > 0) {
      request_stream << "Content-Type: text/plain\r\n";
      request_stream << "Content-Length: " << message.length() << "\r\n";
    }
    request_stream << "Connection: close\r\n\r\n";
    
    if (message.length() > 0) {
      request_stream << message << "\r\n";
    }

    // HTTP でサーバに接続を開始する
    // tcp::resolver::query query(server, "http");
    tcp::resolver::query query(server, port);

    resolver_.async_resolve(query,
			    boost::bind(&JsonRpcClient::handle_resolve, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::iterator));
  }

  // サーバレスポンスを JSON オブジェクトとして取得
  // @result    json オブジェクト (picojson::ext オブジェクト）
  picojson::ext JsonRpcClient::getJsonResponse(void) throw (picojson::PicojsonException) {
    return picojson::ext(this->content_);
  }

  void JsonRpcClient::handle_resolve(const boost::system::error_code& err,
				     tcp::resolver::iterator endpoint_iterator) {
    if (!err) {
      // 名前を解決して、得られたリストに順番に接続を試す
      tcp::endpoint endpoint = *endpoint_iterator;
      socket_.async_connect(endpoint,
			    boost::bind(&JsonRpcClient::handle_connect, this,
					boost::asio::placeholders::error, ++endpoint_iterator));
    } else {
      throw JsonRpcClientException(err.message());
    }
  }

  void JsonRpcClient::handle_connect(const boost::system::error_code& err,
				     tcp::resolver::iterator endpoint_iterator) {
    if (!err) {
      // 接続成功、リクエストメッセージ送信に進む
      boost::asio::async_write(socket_, request_,
			       boost::bind(&JsonRpcClient::handle_write_request, this,
					   boost::asio::placeholders::error));
    } else if (endpoint_iterator != tcp::resolver::iterator()) {
      // 接続失敗したので次のエンドポイントを試す
      socket_.close();
      tcp::endpoint endpoint = *endpoint_iterator;
      socket_.async_connect(endpoint,
			    boost::bind(&JsonRpcClient::handle_connect, this,
					boost::asio::placeholders::error, ++endpoint_iterator));
    } else {
      throw JsonRpcClientException(err.message());
    }
  }

  void JsonRpcClient::handle_write_request(const boost::system::error_code& err) {
    if (!err) {
      // ステータスラインの読み込みに進む
      boost::asio::async_read_until(socket_, response_, "\r\n",
				    boost::bind(&JsonRpcClient::handle_read_status_line, this,
						boost::asio::placeholders::error));
    } else {
      throw JsonRpcClientException(err.message());
    }
  }

  void JsonRpcClient::handle_read_status_line(const boost::system::error_code& err) {
    if (!err) {
      // HTTP レスポンスが OK であることを確認する
      std::istream response_stream(&response_);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
	throw JsonRpcClientException("Invalid response");
      }
      if (status_code != 200) {
	std::stringstream ss;
	ss << "Response returned with status code " << status_code;
	throw JsonRpcClientException(ss.str());
      }

      // レスポンスヘッダを読み込む（空行区切）
      boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
				    boost::bind(&JsonRpcClient::handle_read_headers, this,
						boost::asio::placeholders::error));
    } else {
      std::stringstream ss;
      ss << "Error: " << err;
      throw JsonRpcClientException(ss.str());
    }
  }

  void JsonRpcClient::handle_read_headers(const boost::system::error_code& err) {
    if (!err) {
      // レスポンスヘッダを解析
      std::istream response_stream(&response_);
      std::string header;
      while (std::getline(response_stream, header) && header != "\r") {
	//	std::cout << header << "\n";
	headers_.push_back(header);
      }
      // std::cout << "----\n";

      // ヘッダの残りはコンテンツ
      if (response_.size() > 0) {
	std::stringstream ss;
	ss << &response_;
	content_ += ss.str();
      }
      
      // 残りを EOF まで読む
      boost::asio::async_read(socket_, response_,
			      boost::asio::transfer_at_least(1),
			      boost::bind(&JsonRpcClient::handle_read_content, this,
					  boost::asio::placeholders::error));
    } else {
      std::stringstream ss;
      ss << "Error: " << err;
      throw JsonRpcClientException(ss.str());
    }
  }

  void JsonRpcClient::handle_read_content(const boost::system::error_code& err) {
    if (!err) {
      // 読み込み済みのコンテンツを保存
      std::stringstream ss;
      ss << &response_;
      // std::cout << ss.str();
      content_ += ss.str();
      
      // EOF まで読み込みを繰り返す
      boost::asio::async_read(socket_, response_,
			      boost::asio::transfer_at_least(1),
			      boost::bind(&JsonRpcClient::handle_read_content, this,
					  boost::asio::placeholders::error));
    } else if (err != boost::asio::error::eof) {
      std::stringstream ss;
      ss << "Error: " << err;
      throw JsonRpcClientException(ss.str());
    }
  }

};
