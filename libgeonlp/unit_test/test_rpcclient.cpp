#include <iostream>
#include "JsonRpcClient.h"

int main(int argc, char* argv[]) {
  try {
    if (argc != 3 && argc != 4)	{
      std::cout << "Usage: async_client <server> <path>\n";
      std::cout << "Example:\n";
      std::cout << "  test_rpcclient localhost /index.html\n";
      return 1;
    }

    char* message = (char*)"";
    if (argc == 4) {
      message = argv[3];
    } else {
      message = (char*)"{\"method\":\"getWeight\",\"params\":[[[37.393276,138.595812]]],\"id\":1}";
    }

    boost::asio::io_service io_service;
    geonlp::JsonRpcClient c(io_service, argv[1], argv[2], message, "80");
    io_service.run();
    std::cout << "Content: \n" << c.content_ << std::endl;

    picojson::ext e = c.getJsonResponse();
    std::cout << "JSON: '" << e.toJson() << "'" << std::endl;
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << "\n";
  }

  return 0;
}
