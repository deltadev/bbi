#include "bed_record.h"

#include <iosfwd>
#include <iomanip>

#include <array>

///////////////////////////////////////////////////////////////////////////
// bed_record
//
///////////////////////////////////////////////////////////////////////////
namespace bbi
{
  namespace bed
  {
    
    record::record() { }
    record::record(std::istream& is) { unpack(is); }
    
    void record::print(std::ostream& os) const
    {
      bbi::record::print(os);
      os << std::setw(25) << std::left << "rest" << rest << '\n';
    }
    
    void record::pack(std::ostream& os) const
    {
      bbi::record::pack(os);
      os.write(rest.c_str(), rest.size());
    }
    
    void record::unpack(std::istream& is)
    {
      bbi::record::unpack(is);
      
      // FIXME: What is a reasonable upper bound for the size of the string rest?
      //        Have chosen 4 * 1024 here.
      //
      std::array<char, 4 * 1024> buff;
      is.getline(buff.data(), 4 * 1024, '\0');
      std::streamsize count = is.gcount();
      if (count > 0)
        rest.assign(buff.data(), count - 1);
    }
    
  }  
}