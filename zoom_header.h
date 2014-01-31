#ifndef _ZOOM_HEADER_H_
#define _ZOOM_HEADER_H_

#include <cstdint>

#include <iostream>

struct zoom_header
{
  static const uint32_t byte_size = 24;
  
  uint32_t reduction_level;
  uint32_t reserved;
  uint64_t data_offset;
  uint64_t index_offset;

  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);
};


#endif /* _ZOOM_HEADER_H_ */
