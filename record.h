#ifndef DPJ_RECORD_H_
#define DPJ_RECORD_H_

#include <cstdint>
#include <iosfwd>

////////////////////////////////////////////////////////////////////////////
// record
//
//     This is simply for code re-use. Only intended for static dispatch so
//     no virtual methods are needed.
//
////////////////////////////////////////////////////////////////////////////
namespace bbi
{
  
  struct record
  {
    uint32_t chrom_id;
    uint32_t chrom_start;
    uint32_t chrom_end;
    
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& is);
    
  };
  
}
#endif /* DPJ_RECORD_H_ */
