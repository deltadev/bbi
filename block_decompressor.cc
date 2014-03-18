#include "block_decompressor.h"

#include <string>

using pointer = block_decompressor::pointer;

block_decompressor::block_decompressor() : out_buf_size_(1 << 16), out_buf(out_buf_size_)
{
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;
  stream.avail_in = 0;
  stream.next_in = Z_NULL;
  state = inflateInit(&stream);
}
void block_decompressor::out_buf_size(unsigned size)
{
  out_buf.resize(size);
}


block_decompressor::~block_decompressor()
{
  state = inflateEnd(&stream);
  if (state != Z_OK)
  {
    std::string err_str = "bad rv for inflateEnd";
    throw std::runtime_error("bbi_file::inflate_records " + err_str);
  }
}

std::pair<pointer, pointer>
block_decompressor::decompress(pointer first, pointer last)
{
  unsigned size = static_cast<unsigned>(out_buf.size());
  
  stream.avail_in = static_cast<unsigned>(std::distance(first, last));
  stream.next_in = first;
  stream.avail_out = size;
  
  stream.next_out = out_buf.data();
  
  state = inflate(&stream, Z_FINISH);
  
  if (state != Z_STREAM_END)
  {
    std::string err_str = "ret != Z_STREAM_END";
    throw std::runtime_error("bbi_file::zstream::inflate_block " + err_str);
  }
  
  state = Z_OK;
  
  return {out_buf.data(), out_buf.data() + (size + stream.avail_out)};
}
