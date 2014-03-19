#ifndef DPJ_BED_RECORD_H_
#define DPJ_BED_RECORD_H_

#include "record.h"
#include <sstream>
#include <string>
#include <vector>
/////////////////////////////////////////////////////////////////////
// bed_record
//
/////////////////////////////////////////////////////////////////////
namespace bbi
{
  namespace bed
  {    
    struct record : bbi::record
    {
      record();
      record(std::istream& is);
      std::string rest;
      
      void print(std::ostream& os) const;
      void pack(std::ostream& os) const;
      void unpack(std::istream& os);
      
    };
    
    
  }
  
  template <> struct is_wig_type<bed::record> : std::false_type { };

}




#endif /* DPJ_BED_RECORD_H_ */
