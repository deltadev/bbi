#ifndef DPJ_HTTP_STREAMBUF_HH_
#define DPJ_HTTP_STREAMBUF_HH_

#include <streambuf>
#include <sstream>
#include <string>
#include <vector>

#include "http_session.hh"

namespace dpj
{
  namespace http
  {
    class streambuf : public std::streambuf
    {
      
      // Remote file positions for record keeping.
      //
      ptrdiff_t extb             = 0; // The remote position of eback()
      ptrdiff_t exte             = 0; // The remote position of egptr()
      std::size_t content_length = 0; // "Content-Length" of remote.
      
      
      // Internal buffer size.
      //
      std::size_t const BUF_SIZE;
      std::vector<char> buf;
      
      session session;
      std::string resource;
      
      bool server_closes_session = false;
      
    public:
      
      // ctor sets up the TCP connection and calls private member streambuf::init_http_resource.
      //
      streambuf(std::string host, std::string port, std::string resource,
                bool debug = false, size_t buf_size = 4096)
      : session(host, port, debug)
      , resource(resource)
      , BUF_SIZE(buf_size)
      , buf(BUF_SIZE)
      {
        
        // Sets up the internal buffer with the call to std::streambuf::setg().
        //
        setg(buf.data(), buf.data(), buf.data());
        
        content_length = init_http_resource(resource);
        
        if (content_length == 0)
          throw std::runtime_error("http::streambuf got bad Content-Length");
      }
      
      void debug_session(bool debug) { session.debug = debug; }
      
      
    protected:
      
      // Use super's version of this. Uncomment to debug.
      //
      // virtual std::streamsize xsgetn(char* s, std::streamsize __n);
      
      ////
      //  TODO: not implemented from std::streambuf are:
      //
      //        * swap
      //        * setbuf
      //        * sync
      //        * imbue
      //        * overflow
      //
      //      from std::filebuf we don't have
      //
      //        * open
      //        * is_open
      //        * close
      //
      
      // pbackfail()
      //
      //    Am not sure whether we actually want this. Need to check the spec...
      //
      virtual int_type pbackfail(int_type c)
      {
        if (eback() < gptr() && traits_type::eq_int_type(c, traits_type::eof()))
        {
          gbump(-1);
          return traits_type::not_eof(c);
        }
        return traits_type::eof();
      }
      
      
      
      // overflow()
      //
      //    Surprisingly little to do here. The public interface of std::streambuf
      //    will call this at the necessary moments.
      //
      virtual int_type underflow()
      {
        
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
      
      // seekoff()
      //
      //    If we can re-position within our current buffer we do. Otherwise we fetch a new
      //    one
      //
      virtual pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode)
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
      
      
      // seekpos()
      //
      //    Just use seekoff from ios_base::beg.
      //
      virtual pos_type seekpos(pos_type sp, std::ios_base::openmode m)
      {
        return seekoff(off_type(sp), std::ios::beg, m);
      }
      
      
    private:
      
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
      // Fills internal buffer so that remote offsets [a, b) are contained.
      //
      // Updates external file offsets:
      //
      //    - extb
      //    - exte
      //
      // the latter will typically be extb + BUF_SIZE.
      //
      std::size_t fill_buffer(size_t a)
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
      //   Does the initial HEAD request, sets the content_length member.
      //
      std::size_t init_http_resource(std::string resource)
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
    };
  }
} // namespace http

#endif /* DPJ_HTTP_STREAMBUF_HH_ */
