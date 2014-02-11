#include <iostream>

#include "dpj_utils.h"
#include "http_session.h"

int main(int argc, const char * argv[])
{
  
  try {
    dpj::http::session session("localhost", "80");
    
    unsigned status_code = session.request("GET", "/");
    std::cout << "did request, got status: " << status_code << "\n\n";
    
    auto hdrs = session.read_headers();
    dpj::http::print_headers(hdrs);
    
    auto body = session.read_body(dpj::str::cast<int>(hdrs["Content-Length"]));
    
    
    std::cout << "body returned:\n\n" << std::string(body.begin(), body.end()) << "\n\n";
    
  } catch (std::system_error & e) {
    std::cerr << "system_error: " << e.what() << '\n';
  } catch (std::exception& e) {
    std::cerr << "exception:    " << e.what() << '\n';
  }
  return 0;
}

