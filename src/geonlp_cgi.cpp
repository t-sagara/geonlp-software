#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include "GeonlpService.h"

picojson::value default_id;

void error_response(const std::string& error_message) {
  std::cout << "Status: 400 Bad Request\n";
  std::cout << "Content-Type: text/html\n";
  std::cout << "Access-Control-Allow-Origin: *\n\n";
  std::cout << "<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\n";
  std::cout << "<BODY><H1>Error</H1>\n";
  std::cout << "<P>The request is not parsed as a JSON-RPC request</P>\n";
  std::cout << "<P>Reason: " << error_message << "</P>\n";
  std::cout << "</BODY></HTML>\n";
}

void json_error_response(const std::string& error_message, picojson::value& id) {
  picojson::ext response;
  response.initByJson(std::string("{\"result\":null,\"error\":\"") + error_message + "\",\"id\":" + id.serialize() + "}");
  std::cout << "Content-type: application/json; charset=utf-8\n";
  std::cout << "Access-control-allow-origin: *\n\n";
  std::cout << response.toJson();
}

// Get POST message from stdin to string
void get_post_message(std::string& message) {
  message = "";
  char* cp = getenv("CONTENT_LENGTH");
  if (!cp) {
    error_response(std::string("CONTENT_LENGTH is not defined."));
    exit(0);
  }
  int len = atoi(getenv("CONTENT_LENGTH"));
  char buf[len + 1];
  std::cin.read(buf, len);
  buf[len + 1] = '\0';
  message = buf;
}

bool proc (geonlp::ServicePtr service, const std::string& request_json, picojson::value& response) {
  picojson::ext req;

  if (request_json.length() == 0) return false;
  try {
    req.initByJson(request_json);
  } catch (picojson::PicojsonException e) {
    error_response(std::string("Request string is not a valid JSON representation."));
    exit(0);
  }
  response = service->proc(picojson::value(req));
  return true;
}

int main(int argc, char* argv[])
{
  default_id = picojson::value(long(0));
  
  char* cp = getenv("REQUEST_METHOD");
  if (!cp) {
    error_response(std::string("Unknown method '") + cp + "'.");
    exit(0);
  }
  if (!strcmp(cp, "POST")) {
    std::string request;
    get_post_message(request);
    
    try {
      geonlp::ServicePtr service = geonlp::createService();
      picojson::value response;
      proc(service, request, response);
      std::cout << "Content-Type: application/json; charset=utf-8\n";
      std::cout << "Access-Control-Allow-Origin: *\n\n";
      std::cout << response.serialize();
    } catch (geonlp::ServiceCreateFailedException& e) {
      json_error_response(e.what(), default_id);
      exit(0);
    }
  } else if (strcmp(cp, "GET") && strcmp(cp, "HEAD")) {
    // Preflignted requests
    std::cout << "Access-Control-Allow-Origin: *\n";
    std::cout << "Access-Control-Allow-Methods: POST, OPTIONS\n";
    std::cout << "Access-Control-Allow-Headers: X-GeoNLP-Authorization, Content-type\n";
    std::cout << "Access-Control-Max-Age: 3600\n";
    std::cout << "Content-Length: 0\n";
    std::cout << "Content-Type: text/plain\n\n";
    exit(0);
  }

  return 0;
}
