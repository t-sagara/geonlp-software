#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include "GeonlpService.h"

picojson::value default_id;

void error_response(const std::string& error_message, picojson::value& id) {
  picojson::ext response;
  response.initByJson(std::string("{\"result\":null,\"error\":\"") + error_message + "\",\"id\":" + id.serialize() + "}");
  std::cout << response.toJson();
}

// Get POST message from stdin to string
void get_post_message(std::string& message) {
  message = "";
  char* cp = getenv("CONTENT_LENGTH");
  if (!cp) {
    error_response(std::string("CONTENT_LENGTH is not defined."), default_id);
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
    error_response(std::string("Request string is not a valid JSON representation."), default_id);
    exit(0);
  }
  response = service->proc(picojson::value(req));
  return true;
}

int main(int argc, char* argv[])
{
  default_id = picojson::value(long(0));
  
  std::cout << "Content-type: application/json; charset=utf-8\n\n";
  char* cp = getenv("REQUEST_METHOD");
  if (!cp || strcmp(cp, "POST")) {
    error_response(std::string("Use POST method."), default_id);
    exit(0);
  }

  std::string request;
  get_post_message(request);

  try {
    geonlp::ServicePtr service = geonlp::createService();
    picojson::value response;
    proc(service, request, response);
    std::cout << response.serialize();
  } catch (geonlp::ServiceCreateFailedException& e) {
    error_response(e.what(), default_id);
    exit(0);
  }

  return 0;
}
