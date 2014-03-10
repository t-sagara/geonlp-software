#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "GeonlpService.h"

// Max buffer size
const int max_length = 1024;

// Default service tcp port
const int default_port = 8888;

// End of Request sequence
const std::string EOR_SEQ("{EOR}");

// End of Connection sequence
const std::string EOC_SEQ("{EOC}");

using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> socket_ptr;
geonlp::ServicePtr service;

void usage(const char* cmd) {
  std::cerr << "Usage: " << cmd << " [--rc=<rc filename>] [--port=<port>]" << std::endl;
  std::cerr << "or, " << cmd << " --version" << std::endl;
  std::cerr << "\nDefault port = " << default_port << std::endl;
  return;
}

std::string proc(std::string request) {
  picojson::ext req;
  picojson::value res;
  std::string response;

  if (request.length() == 0) return "";
  req.initByJson(request);
  res = service->proc(picojson::value(req));
  response = res.serialize();
  return response;
}

void session(socket_ptr sock)
{
  try
  {
    int pos;
    bool done = false;
    while (!done) {
      std::string request("");
      for (;;) {
	char data[max_length];
	
	boost::system::error_code error;
	size_t length = sock->read_some(boost::asio::buffer(data), error);
	if (error == boost::asio::error::eof) {
	  done = true;
	  request = "";
	  break; // Connection closed cleanly by peer.
	}
	else if (error)
	  throw boost::system::system_error(error); // Some other error.
	
	request += std::string(data, length);
	pos = request.find(EOR_SEQ.c_str());
	if (pos != std::string::npos) {
	  request = request.substr(0, pos);
	  break;
	} else if (request.find(EOC_SEQ.c_str()) != std::string::npos) {
	  request = "";
	  done = true;
	  break;
	}
      }
      if (request.length() > 0) {
	std::string result = proc(request);
	result += EOR_SEQ;
	boost::asio::write(*sock, boost::asio::buffer(result.c_str(), result.length()));
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
}

void server(boost::asio::io_service& io_service, short port)
{
  tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
  for (;;)
  {
    socket_ptr sock(new tcp::socket(io_service));
    a.accept(*sock);
    boost::thread t(boost::bind(session, sock));
  }
}

int main(int argc, char* argv[])
{
  try
  {
    std::string rcfilename = "";
    int port = default_port;

    for (int i = 1; i < argc; i++) {
      if (!strncmp("--version", argv[i], 9)) {
	std::cout << PACKAGE_VERSION << std::endl;
	exit(0);
      } else if (!std::strncmp("--rc=", argv[i], 5)) {
	rcfilename = std::string(argv[i] + 5);
      } else if (!std::strncmp("--port=", argv[i], 7)) {
	port = std::atoi(argv[i] + 7);
      } else {
	usage(argv[0]);
	exit(1);
      }
    }

    try {
      if (rcfilename == "") {
	service = geonlp::createService();
      } else {
	service = geonlp::createService(rcfilename);
      }
    } catch (geonlp::ServiceCreateFailedException& e) {
      std::cerr << e.what();
      return 1;
    }

    boost::asio::io_service io_service;

    server(io_service, port);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

