#ifndef _DATA_RECORDS_H_
#define _DATA_RECORDS_H_

#include <iostream>
#include <cfloat>

////////////////////////////////////////////////////////////////////////////
// data_record
// 
//     This is simply for code re-use. Only intended for static dispatch so
//     no virtual methods are needed.
//     
////////////////////////////////////////////////////////////////////////////
struct data_record
{
  uint32_t chrom_id;
  uint32_t chrom_start;
  uint32_t chrom_end;

  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);

};

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

/////////////////////////////////////////////////////////////////////
// bed_data_record
// 
/////////////////////////////////////////////////////////////////////
struct bed_data_record : data_record
{

  std::string rest;

  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);

};

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

#endif /* _DATA_RECORDS_H_ */
