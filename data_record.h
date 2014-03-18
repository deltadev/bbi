#ifndef DPJ_DATA_RECORD_H_
#define DPJ_DATA_RECORD_H_

#include <cstdint>
#include <iosfwd>

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



#endif /* DPJ_DATA_RECORD_H_ */
