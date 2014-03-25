#ifndef DPJ_BLOCK_DECOMPRESSOR_HH_
#define DPJ_BLOCK_DECOMPRESSOR_HH_

#include <cstdint>

#include <zlib.h>

#include <string>
#include <sstream>

#include <vector>

class block_decompressor
{
public:
  
  using byte    = uint8_t;
  using pointer = byte*;
  using buffer  = std::vector<byte>;
  
  block_decompressor(int buf_size = 1 << 16) : out_buf(buf_size) { init(); }

  ~block_decompressor()
  {
    state = inflateEnd(&stream);
    if (state != Z_OK)
      error();
  }
  
  std::pair<pointer, pointer> decompress(pointer first, pointer last)
  {
    unsigned size = static_cast<unsigned>(out_buf.size());
    
    stream.avail_in = static_cast<unsigned>(std::distance(first, last));
    stream.next_in = first;
    stream.avail_out = size;
    
    stream.next_out = out_buf.data();
    
    state = inflate(&stream, Z_FINISH);
    
    if (state != Z_STREAM_END)
      error();
    
    reset();
    
    if (state != Z_OK)
      error();
    
    return {out_buf.data(), out_buf.data() + (size - stream.avail_out)};
  }
    
  void buf_size(int size)  { out_buf.resize(size); }
  
private:
  void init()
  {
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;
    state = inflateInit(&stream);
  }
  
  void reset() { state = inflateReset(&stream); }
  void error()
  {
    std::stringstream ss;
    ss << "zlib error: msg: " << stream.msg << ", error code: " << state;
    throw std::runtime_error{ss.str()};
  }
  
  
  buffer out_buf;
  z_stream stream;
  int state;
};


#endif /* DPJ_BLOCK_DECOMPRESSOR_HH_ */
