#ifndef DPJ_ZOOM_RECORD_H_
#define DPJ_ZOOM_RECORD_H_

#include "record.h"


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
      
      record();
      record(std::istream& is);
      
      void print(std::ostream& os) const;
      void pack(std::ostream& os) const;
      //void unpack(std::istream& os);
      
      //  zoom_record(uint32_t chrom_id = 0,
      //                   uint32_t chrom_start = std::numeric_limits<uint32_t>::max(),
      //                   uint32_t chrom_end = 0,
      //                   uint32_t valid_count = 0,
      //                   float min_val = FLT_MAX,
      //                   float max_val = FLT_MIN,
      //                   float sum_data = 0,
      //                   float sum_squares = 0);
      friend void unpack(record const& r, std::streambuf& s)
      {
        unpack(static_cast<bbi::record>(r), s);
        s.sgetn((char*)&r.valid_count, sizeof r.valid_count);
        s.sgetn((char*)&r.min_val, sizeof r.min_val);
        s.sgetn((char*)&r.max_val, sizeof r.max_val);
        s.sgetn((char*)&r.sum_data, sizeof r.sum_data);
        s.sgetn((char*)&r.sum_squares, sizeof r.sum_squares);
      }
    };
  }
  template <> struct bbi_type<zoom::record> : std::true_type { };
}

#endif /* DPJ_ZOOM_RECORD_H_ */
