#ifndef DPJ_WIG_RECORD_H_
#define DPJ_WIG_RECORD_H_

#include "record.h"


#include <sstream>
#include <vector>



/////////////////////////////////////////////////////////////////////
// wig_record
//
/////////////////////////////////////////////////////////////////////


//  wig_record(uint32_t chrom_id = 0,
//                  uint32_t chrom_start = std::numeric_limits<uint32_t>::max(),
//                  uint32_t chrom_end = 0,
//                  uint32_t item_step = 0,
//                  uint32_t item_span = 0,
//                  uint8_t type = 1,  // 1 bed graph, 2 var step, 3 fixed step
//                  uint8_t reserved = 0,
//                  uint16_t item_count = 0);


namespace bbi
{
  namespace wig
  {
    struct header : bbi::record
    {
      uint32_t item_step;
      uint32_t item_span;
      uint8_t type;
      uint8_t reserved;
      uint16_t item_count;
      
      header(std::istream& is);
      header(uint8_t* bytes);
      header();
      
      void print(std::ostream&) const;
      void pack(std::ostream&) const;
      void unpack(std::istream&);
      
    };
    
    
    struct bed_graph
    {
      uint32_t chrom_start;
      uint32_t chrom_end;
      
      bed_graph(std::istream& is);
      bed_graph();
      
      void pack(std::ostream&) const;
      void unpack(std::istream&);
      void print(std::ostream&) const;
    };
    
    struct var_step
    {
      uint32_t chrom_start;
      float val;
      
      var_step(std::istream& is);
      var_step();
      
      void pack(std::ostream&) const;
      void unpack(std::istream&);
      void print(std::ostream&) const;
    };
    
    struct fixed_step
    {
      float val;
      
      fixed_step(std::istream& is);
      fixed_step();
      
      void pack(std::ostream&) const;
      void unpack(std::istream&);
      void print(std::ostream&) const;
    };
    

    
    
  }
  template <> struct is_wig_type<wig::bed_graph> : std::true_type { };
  template <> struct is_wig_type<wig::var_step> : std::true_type { };
  template <> struct is_wig_type<wig::fixed_step> : std::true_type { };
}



  
  
  
  
#endif /* DPJ_WIG_RECORD_H_ */
