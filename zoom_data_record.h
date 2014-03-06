#ifndef _DPJ_ZOOM_DATA_RECORD_H_
#define _DPJ_ZOOM_DATA_RECORD_H_

#include "data_record.h"


////////////////////////////////////////////////////////////////////////////
// zoom_data_record
// 
//     common to wig and bed
// 
////////////////////////////////////////////////////////////////////////////
struct zoom_data_record : data_record
{

  static const unsigned byte_size = 32;

  uint32_t valid_count;
  float min_val;
  float max_val;
  float sum_data;
  float sum_squares;

  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);

//  zoom_data_record(uint32_t chrom_id = 0,
//                   uint32_t chrom_start = std::numeric_limits<uint32_t>::max(),
//                   uint32_t chrom_end = 0,
//                   uint32_t valid_count = 0,
//                   float min_val = FLT_MAX,
//                   float max_val = FLT_MIN,
//                   float sum_data = 0,
//                   float sum_squares = 0);
};



#endif /* _DPJ_ZOOM_DATA_RECORD_H_ */
