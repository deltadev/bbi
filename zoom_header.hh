#ifndef DPJ_ZOOM_HEADER_HH_
#define DPJ_ZOOM_HEADER_HH_

#include <cstdint>

#include <ostream>
#include <iomanip>

struct zoom_header
{
  static const uint32_t byte_size = 24;
  
  uint32_t reduction_level;
  uint32_t reserved;
  uint64_t data_offset;
  uint64_t index_offset;

  
  //void pack(std::ostream& os) const;

friend void zoom_print(zoom_header const& h, std::ostream& os)
{
  using std::setw; using std::left;
  os << setw(25) << left << "reduction_level" << h.reduction_level << '\n';
  os << setw(25) << left << "reserved" << h.reserved << '\n';
  os << setw(25) << left << "data_offset" << h.data_offset << '\n';
  os << setw(25) << left << "index_offset" << h.index_offset << '\n';
}


  friend void unpack(zoom_header& h, std::streambuf* s)
  {
    s->sgetn((char*)&h.reduction_level, sizeof h.reduction_level);
    s->sgetn((char*)&h.reserved, sizeof h.reserved);
    s->sgetn((char*)&h.data_offset, sizeof h.data_offset);
    s->sgetn((char*)&h.index_offset, sizeof h.index_offset);
  }
};


#endif /* DPJ_ZOOM_HEADER_HH_ */
