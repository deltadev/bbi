#include "zoom_record.h"

#include <iosfwd>
#include <iomanip>

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
    
    record::record() { }
    record::record(std::istream& is) { unpack(is); }
    
    void record::print(std::ostream& os) const {
      
      bbi::record::print(os);
      os << std::setw(25) << std::left << "valid_count" << valid_count << '\n';
      os << std::setw(25) << std::left << "min_val" << min_val << '\n';
      os << std::setw(25) << std::left << "max_val" << max_val << '\n';
      os << std::setw(25) << std::left << "sum_data" << sum_data << '\n';
      os << std::setw(25) << std::left << "sum_squares" << sum_squares << '\n';
    }
    
    void record::pack(std::ostream& os) const {
      bbi::record::pack(os);
      os.write((char*)&valid_count, sizeof valid_count);
      os.write((char*)&min_val, sizeof min_val);
      os.write((char*)&max_val, sizeof max_val);
      os.write((char*)&sum_data, sizeof sum_data);
      os.write((char*)&sum_squares, sizeof sum_squares);
    }
    
    // void record::unpack(std::istream& os) {
    //   bbi::record::unpack(os);
    //   os.read((char*)&valid_count, sizeof valid_count);
    //   os.read((char*)&min_val, sizeof min_val);
    //   os.read((char*)&max_val, sizeof max_val);
    //   os.read((char*)&sum_data, sizeof sum_data);
    //   os.read((char*)&sum_squares, sizeof sum_squares);
    // }
  }
}
