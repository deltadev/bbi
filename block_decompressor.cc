#include "block_decompressor.h"

#include <string>
#include <sstream>

using pointer = block_decompressor::pointer;

block_decompressor::block_decompressor(int buf_size) : out_buf(buf_size) { init(); }

void block_decompressor::buf_size(int size) { out_buf.resize(size); }

void block_decompressor::reset() { state = inflateReset(&stream); }
void block_decompressor::init()
{
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;
  stream.avail_in = 0;
  stream.next_in = Z_NULL;
  state = inflateInit(&stream);
}
void block_decompressor::error()
{
  std::stringstream ss;
  ss << "zlib error: msg: " << stream.msg << ", error code: " << state;
  throw std::runtime_error{ss.str()};
}

block_decompressor::~block_decompressor()
{
  state = inflateEnd(&stream);
  if (state != Z_OK)
    error();
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
    error();
 
  reset();

  if (state != Z_OK)
    error();

  return {out_buf.data(), out_buf.data() + (size - stream.avail_out)};
}
