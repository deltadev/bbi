#ifndef DPJ_WIG_DATA_RECORD_H_
#define DPJ_WIG_DATA_RECORD_H_

#include "data_record.h"

/////////////////////////////////////////////////////////////////////
// wig_data_record
// 
/////////////////////////////////////////////////////////////////////

struct wig_data_record : data_record
{
  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);

  uint32_t item_step;
  uint32_t item_span;
  uint8_t type;
  uint8_t reserved;
  uint16_t item_count;  
//  wig_data_record(uint32_t chrom_id = 0,
//                  uint32_t chrom_start = std::numeric_limits<uint32_t>::max(),
//                  uint32_t chrom_end = 0,
//                  uint32_t item_step = 0,
//                  uint32_t item_span = 0,
//                  uint8_t type = 1,  // 1 bed graph, 2 var step, 3 fixed step
//                  uint8_t reserved = 0,
//                  uint16_t item_count = 0); 
};


#endif /* DPJ_WIG_DATA_RECORD_H_ */
