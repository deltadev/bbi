#ifndef DPJ_HTTP_SESSION_H_
#define DPJ_HTTP_SESSION_H_

#include <iostream>
#include <map>
#include <string>

#define ASIO_STANDALONE
#include <asio.hpp>

namespace dpj {
namespace http {

  typedef std::map<std::string, std::string> header_map;
  
  class session
  {
    session(session const&) = delete;
    session& operator=(session const&) = delete;
    
  public:
    

    // Constructs socket from host and port strings.
    //
    session(std::string host, std::string port, bool debug = false);
    

    // Synchronous request.
    //
    // Returns the status code after successfully reading the status line of the response.
    //
    // Otherwise throws...
    //
    //   Errors: throws runtime_errors and asio errors.
    //
    unsigned request(std::string type, std::string resource, header_map const& = header_map{});
    


    header_map read_headers();    
    std::vector<char> read_body(unsigned content_length);

    void connect();
    bool debug;
    
  private:

    std::string host, port;
    
    void resolve_endpoint();
    
    // Called by request. Will recursively call request if the
    // connection was closed or on other errors.
    std::pair<asio::error_code, unsigned> read_status_line();
    
    asio::io_service io_service;
    asio::streambuf sbuf;
    asio::ip::tcp::socket socket;
    asio::ip::tcp::resolver::iterator endpoint_iterator;
  };
  
  void print_headers(header_map const& header_map, std::ostream &os = std::cout);
}
}

#endif /* DPJ_HTTP_SESSION_H_ */

