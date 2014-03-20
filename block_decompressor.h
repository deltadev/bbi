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

  block_decompressor();
  ~block_decompressor();

  std::pair<pointer, pointer> decompress(pointer first, pointer last);

  void decomp_buf_size(unsigned size);

private:
  unsigned out_buf_size_;
  buffer out_buf;
  z_stream stream;
  int state;
};


#endif /* DPJ_BLOCK_DECOMPRESSOR_H_ */
