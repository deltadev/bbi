#include "http_streambuf.h"

#include "dpj_utils.h"


namespace dpj
{
  namespace http
  {
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    // Public methods
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    streambuf::streambuf(std::string host, std::string port, std::string resource,
                         bool debug, std::size_t buf_size)
      : session(host, port, debug),
        resource(resource),
        BUF_SIZE(buf_size),
        buf(BUF_SIZE)
    {
      
      // Sets up the internal buffer with the call to std::streambuf::setg().
      //
      setg(buf.data(), buf.data(), buf.data());
      
      content_length = init_http_resource(resource);
      
      if (content_length == 0)
        throw std::runtime_error("http::streambuf got bad Content-Length");
    }
    
    
    void streambuf::debug_session(bool debug) { session.debug = debug; }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    // Protected methods - virtual overrides
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    std::streambuf::int_type streambuf::pbackfail(int_type c)
    {
      if (   eback() < gptr()
          && traits_type::eq_int_type(c, traits_type::eof()))
      {
        gbump(-1);
        return traits_type::not_eof(c);
      }
      return traits_type::eof();
    }

    std::streambuf::int_type streambuf::underflow() {
      
      if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
      
      auto n = fill_buffer(exte);
      
      // Interesting fact:
      //
      //   traits_type::int_type := int
      //   traits_type::to_int_type(traits_type::char_type c) -> traits_type::int_type
      //
      // But, for traits_type::char_type c, (e.g. c = *gptr()),
      //
      //   if
      //      c == traits_type::eof()
      //   then
      //      traits_type::int_type(c) != traits_type::to_int_type(c)
      //
      // 2014-02-13
      //
      if (n > 0)
        return traits_type::to_int_type(*gptr());
      else
        return traits_type::eof();
      
    }
    
//// This is essentially the same as std::streambuf::xsgetn. Is useful for debugging.
//
//    std::streamsize
//    streambuf::xsgetn(char* s, std::streamsize n)
//    {
//      const int_type eof = traits_type::eof();
//      int_type c = -1;
//      std::streamsize i = 0;
//      for (;i < n; ++i, ++s)
//      {
//        if (gptr() < egptr())
//          *s = sbumpc();
//        else if ((c = uflow()) != eof)
//          *s = traits_type::to_char_type(c);
//        else
//          break;
//      }
//      return i;
//    }
    
    std::streambuf::pos_type streambuf::seekoff(off_type off, std::ios_base::seekdir way,
                                                std::ios_base::openmode)
    {
      ptrdiff_t n;                        // Amount read from network.
      ptrdiff_t intn = gptr() - eback();  // Current internal pos.
      ptrdiff_t extn;                     // New remote position.
      
      switch (way)
      {
        case std::ios_base::cur:
          extn = extb + intn + off;
          break;
        case std::ios_base::beg:
          extn = off;
          break;
        case std::ios_base::end:
          extn = content_length + off;
        default:
          return pos_type(-1);
      }
      if (extb <= extn && extn < exte)
        setg(eback(), eback() + extn - extb, egptr());
      else
      {
        n = fill_buffer(extn);
        if (n == 0)
          return pos_type(-1);
      }
      return pos_type(extn);
    }
    
    std::streambuf::pos_type streambuf::seekpos(pos_type sp, std::ios_base::openmode m)
    {
      return seekoff(off_type(sp), std::ios::beg, m);
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////////////////////////
    ////
    //   Network connectivity:
    //
    //     * The http::streambuf does initial setup of the TCP connection
    //     * All the other work is done via calls to the http::session member.
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    
    // fillbuffer()
    //
    //   This is where we get BUF_SIZE bytes from the network.
    //
    //   * `a' is the byte offset in the file we are accessing.
    //
    std::size_t streambuf::fill_buffer(std::size_t a)
    {
      
      std::ostringstream oss;
      oss << "bytes=" << a << '-' << (a + BUF_SIZE - 1);
      
      //  If the server sent us a "Connection : close" header then we always re-connect.
      //
      if (server_closes_session)
        session.connect();
      
      unsigned status_code = session.request("GET", resource, {{"Range", oss.str()}});
      if (status_code != 206)
      {
        std::stringstream ss;
        ss << "http::streambuf::fill_buffer(): bad status code:" << status_code << '\n';
        throw std::runtime_error(ss.str());
      }
      
      auto hdrs = session.read_headers();
      
      if (session.debug)
        http::print_headers(hdrs);
      
      // The content length of the range request.
      // TODO: implement chunked encoding.
      //
      unsigned content_length = str::cast<int>(hdrs["Content-Length"]);
      
      // TOFU: What happens if we asked for more bytes than are available?
      //
      auto bytes = session.read_body(content_length);
      std::size_t n = bytes.size();
      
      if (n == 0)
        throw std::runtime_error("http::streambuf::fill_buffer(): body length zero");
      
      extb = a;
      exte = a + n;
      
      // TODO: There is an unecessary copy here.
      std::copy(bytes.begin(), bytes.end(), buf.data());
      setg(buf.data(), buf.data(), buf.data() + n);
      
      
      return bytes.size();
    }
    
    
    // init_http_resource()
    //
    //   Called by the ctor.
    //
    std::size_t streambuf::init_http_resource(std::string resource)
    {
      // Initial request to find out who we are talking to and what they are like.
      //
      //   TODO: Need to deal with other status codes.
      //
      auto status_code = session.request("HEAD", resource);
      std::stringstream ss;
      if (status_code != 200) {
        ss << "streambuf HEAD request, bad status code: " << status_code;
        throw std::runtime_error(ss.str());
      }
      
      // Deals with the HEAD request headers.
      //
      //   * check the server accepts range requests.
      //   * server_closes_session is set here.
      //   * content_length is set here.
      //
      auto hdrs = session.read_headers();
      if (session.debug)
        print_headers(hdrs);
      
      auto it = hdrs.find("Accept-Ranges");
      if (it == hdrs.end())
        throw std::runtime_error("No Accept-Ranges header");
      
      it = hdrs.find("Connection");
      if (it != hdrs.end() && it->second == "close")
        server_closes_session = true; // We don't like this server.
      
      it = hdrs.find("Content-Length");
      if (it == hdrs.end())
        throw std::runtime_error("No Content-Length header");
      
      return str::cast<std::size_t>(it->second);
    }
    
  }} // namespace http} dpj}
