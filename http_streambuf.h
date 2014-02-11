#ifndef _HTTP_STREAMBUF_H_
#define _HTTP_STREAMBUF_H_

#include <streambuf>
#include <sstream>
#include <string>
#include <vector>

#include "http_session.h"

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
                bool debug = false, size_t buf_size = 512);
  
      void debug_session(bool debug);
  
    protected:
  
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
      virtual int_type pbackfail(int_type c);
  
  
      // overflow()
      //
      //    Surprisingly little to do here. The public interface of std::streambuf
      //    will call this at the necessary moments.
      //
      virtual int_type underflow();

      // seekoff()
      //
      //    If we can re-position within our current buffer we do. Otherwise we fetch a new
      //    one
      //
      virtual pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode);
  
      // seekpos()
      //
      //    Just use seekoff from ios_base::beg.
      //
      virtual pos_type seekpos(pos_type sp, std::ios_base::openmode m);
  
    private:

      // init_http_resource()
      //
      //   Does the initial HEAD request, sets the content_length member.
      //
      std::size_t init_http_resource(std::string resource);


      // Fills internal buffer so that remote offsets [a, b) are contained.
      //
      // Updates external file offsets:
      //
      //    - extb
      //    - exte
      //
      // the latter will typically be extb + BUF_SIZE.
      //
      std::size_t fill_buffer(size_t a);
  
    };
  }
} // namespace http

#endif /* _HTTP_STREAMBUF_H_ */
