#include "http_session.h"

#include "dpj_utils.h"

#include <iomanip>

namespace dpj {
namespace http {

  ////////////////////////////////////////////////////////////////////////////////
  //
  // Public methods.
  //
  ////////////////////////////////////////////////////////////////////////////////

  session::session(std::string host, std::string port, bool debug)
    : socket(io_service), host(host), port(port), debug(debug)
  {
    try {
      resolve_endpoint();
      connect();
    } catch (std::system_error &e) {
      std::cerr << "setup_socket() failed in http_session ctor\n";
      throw e;
    }
  }
  
  void session::resolve_endpoint()
  {
    asio::ip::tcp::resolver resolver(io_service);
    asio::ip::tcp::resolver::query query(host, port);

    // TODO: does this iterator remain valid after resolver goes out of scope?
    //
    endpoint_iterator = resolver.resolve(query);
  }
  void session::connect() {
    
    if (socket.is_open())
      socket.close();
    
    asio::connect(socket, endpoint_iterator);
  }
  
  unsigned session::request(std::string type, std::string resource, const header_map &hm)
  {

    // First we build our range request.
    //
    std::ostream request_stream(&sbuf);
    request_stream << type << " " << resource << " HTTP/1.1\r\n";
    request_stream << "Host: "    << host     <<          "\r\n";
    for (auto const& kv : hm)
      request_stream << kv.first << ": " << kv.second <<  "\r\n";
    request_stream << "\r\n"; // End headers
    

    if (debug) // Prints the request to std::cerr.
    {
      auto b = asio::buffers_begin(sbuf.data());
      std::string hdrs(b, b + sbuf.size());
      std::cerr << "**** Request ****\n\n"<< hdrs << "*****************\n\n";
    }

    // Sends the request.
    //
    asio::write(socket, sbuf);


    // Attempts to read a response from the server.
    //
    // {error, status} is returned to us from read_status_line with the following post-condition.
    //
    //    if (error) then status == 0
    //
    // We have this returned here since we might want to send the request again if it didn't work
    // the first time and we have the details of the request in scope.
    //
    asio::error_code error;
    unsigned status;
    std::tie(error, status) = read_status_line(); // Calls asio::read_until( , ,error)


    // TODO: Am not sure why we get EOF here sometimes... need to do some digging. Is it
    // because the connection is down but not yet reporting as such?
    //
    if (error == asio::error::connection_reset || error == asio::error::eof)
    {
      if (debug)
        std::cerr << "re-connecting after error: " << error.message() << '\n';

      // So now we try again.
      //
      connect();
      status = request(type, resource, hm);
    }
    
    return status;
  }

  // read_headers()
  //
  header_map session::read_headers()
  {
  
    asio::read_until(socket, sbuf, "\r\n\r\n");

    header_map header_map;
    
    std::istream response_stream(&sbuf);
    std::string h;
    
    while (std::getline(response_stream, h) && h != "\r") {
      auto pos = h.find_first_of(':');
      if (pos != std::string::npos)
        header_map[str::strip(h.substr(0, pos))] = str::strip(h.substr(pos + 1));
      else
        throw std::runtime_error("couldn't find ':' when parsing header line: " + h);
    }
    
    return header_map;
  }

  

  // read_body()
  //
  //   TODO: nead to implement chunked encoding reads.
  //
  std::vector<char> session::read_body(unsigned content_length)
  {

    auto bytes_avail = sbuf.size();

    if (bytes_avail < content_length)
      asio::read(socket, sbuf, asio::transfer_exactly(content_length - bytes_avail));
    
    if (sbuf.size() < content_length)
      throw std::runtime_error("failed to read content length from body");
    
    auto b = asio::buffers_begin(sbuf.data());
    std::vector<char> bytes{b, b + content_length};
    sbuf.consume(content_length);
    return bytes;
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  //
  // Private methods.
  //
  ////////////////////////////////////////////////////////////////////////////////
  

  // session::read_status_line()
  //
  //     This method recieves the return value of this method since
  //     it is here we will discover if the server has closed on us.
  //
  //   TODO: make this non-recursive with a suitable MAX_RETRY_COUNT.
  //
  std::pair<asio::error_code, unsigned> session::read_status_line()
  {
    
    // At this point the server might have hung up so we collect the error and maybe re-try.
    //
    asio::error_code error;
    asio::read_until(socket, sbuf, "\r\n", error);
    
    // Sends the error back to session::request so we can maybe re-try the request.
    //
    if (error)
      return {error, 0};
    
    
    // All seems good so we can start to parse the response.
    //
    std::istream response_stream(&sbuf);
    unsigned int status_code;
    std::string http_version, status_message;
    response_stream >> http_version >> status_code;
    
    std::getline(response_stream, status_message);
    
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      throw std::runtime_error("Invalid Status-Line in response\n");
    
    return {error, status_code};
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // Utility functions.
  //
  ////////////////////////////////////////////////////////////////////////////////
  void print_headers(header_map const& header_map, std::ostream &os)
  {
    for (auto const& p : header_map)
      std::cout << std::setw(30) << p.first <<" : " << p.second << '\n';
  }
  
  unsigned check_range_header(std::string s, size_t a, size_t b)
  {
    
    std::stringstream ss{s};
    std::string bs;
    char h;
    unsigned a_r, b_r, c_r;
    
    ss >> bs;
    if (bs != "bytes")
      goto throw_label;
    
    ss >> a_r;
    
    if (a != a_r)
      goto throw_label;
    
    h = ss.get();
    if (h != '-')
      goto throw_label;
    
    ss >> b_r;
    if (b != b_r)
      goto throw_label;
    
    h = ss.get();
    if (h != '/')
      goto throw_label;
    
    ss >> c_r;
    
    return c_r;
    
  throw_label:
    
    throw std::runtime_error("bad parse of Content-Range header");
  }
}}
