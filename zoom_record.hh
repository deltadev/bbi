#ifndef DPJ_ZOOM_RECORD_HH_
#define DPJ_ZOOM_RECORD_HH_

#include "record.hh"
#include <iomanip>

////////////////////////////////////////////////////////////////////////////
// zoom_record
//
//     common to wig and bed
//
////////////////////////////////////////////////////////////////////////////
namespace bbi
{
  namespace zoom
  {
    struct record : bbi::record
    {
      static const unsigned byte_size = 32;
      
      uint32_t valid_count;
      float min_val;
      float max_val;
      float sum_data;
      float sum_squares;
      
      //void pack(std::ostream& os) const;
      record() { }
      record(std::streambuf* s) { unpack(*this, s); }
      
      friend void print(record const& r, std::ostream& os)
      {
        print(static_cast<bbi::record const&>(r), os);
        os << std::setw(25) << std::left << "valid_count" << r.valid_count << '\n';
        os << std::setw(25) << std::left << "min_val" << r.min_val << '\n';
        os << std::setw(25) << std::left << "max_val" << r.max_val << '\n';
        os << std::setw(25) << std::left << "sum_data" << r.sum_data << '\n';
        os << std::setw(25) << std::left << "sum_squares" << r.sum_squares << '\n';
      }
      
      friend void unpack(record& r, std::streambuf* s)
      {
        unpack(static_cast<bbi::record&>(r), s);
        s->sgetn((char*)&r.valid_count, sizeof r.valid_count);
        s->sgetn((char*)&r.min_val, sizeof r.min_val);
        s->sgetn((char*)&r.max_val, sizeof r.max_val);
        s->sgetn((char*)&r.sum_data, sizeof r.sum_data);
        s->sgetn((char*)&r.sum_squares, sizeof r.sum_squares);
      }
    };
  }
  
  template <> struct bbi_type<zoom::record> : std::true_type { };
}

#endif /* DPJ_ZOOM_RECORD_HH_ */
