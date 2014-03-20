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
    
    struct bed_graph_record
    {
      uint32_t chrom_start;
      uint32_t chrom_end;
      
      bed_graph_record(std::istream& is);
      bed_graph_record();
      
      void pack(std::ostream&) const;
      void unpack(std::istream&);
      void print(std::ostream&) const;
    };
    
    struct var_step_record
    {
      uint32_t chrom_start;
      float val;
      
      var_step_record(std::istream& is);
      var_step_record();
      
      void pack(std::ostream&) const;
      void unpack(std::istream&);
      void print(std::ostream&) const;
    };
    
    struct fixed_step_record
    {
      float val;
      
      fixed_step_record(std::istream& is);
      fixed_step_record();
      
      void pack(std::ostream&) const;
      void unpack(std::istream&);
      void print(std::ostream&) const;
    };
    
    // TODO: overload this so that types are annotated using the header info, e.g.
    // the span and step, (if used).
    //
    template<typename T>
    std::vector<T> extract(std::istream& is, unsigned count)
    {
      std::vector<T> rs(count);
      std::generate(begin(rs), end(rs), [&]() -> T { return is; });
      return rs;
    }
    
  }
  template <> struct bbi_type<wig::header> : std::true_type { };
  template <> struct bbi_type<wig::bed_graph_record> : std::true_type { };
  template <> struct bbi_type<wig::var_step_record> : std::true_type { };
  template <> struct bbi_type<wig::fixed_step_record> : std::true_type { };
}



  
  
  
  
#endif /* DPJ_WIG_RECORD_H_ */
