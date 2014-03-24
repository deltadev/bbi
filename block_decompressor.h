#ifndef DPJ_BLOCK_DECOMPRESSOR_H_
#define DPJ_BLOCK_DECOMPRESSOR_H_

#include <cstdint>

#include <zlib.h>

#include <vector>

class block_decompressor
{
public:
  
  using byte    = uint8_t;
  using pointer = byte*;
  using buffer  = std::vector<byte>;

  block_decompressor(int buf_size = 1 << 16);
  ~block_decompressor();


  std::pair<pointer, pointer> decompress(pointer first, pointer last);

  void buf_size(int size);

private:
  void init();
  void reset();
  void error();
  
  buffer out_buf;
  z_stream stream;
  int state;
};


#endif /* DPJ_BLOCK_DECOMPRESSOR_H_ */
