#include "total_summary_header.h"

#include <iomanip>

void total_summary_header::print(std::ostream& os) const {
  os << std::setw(25) << std::left << "bases_covered" << bases_covered << '\n';
  os << std::setw(25) << std::left << "min_val" << min_val << '\n';
  os << std::setw(25) << std::left << "max_val" << max_val << '\n';
  os << std::setw(25) << std::left << "sum_data" << sum_data << '\n';
  os << std::setw(25) << std::left << "sum_squares" << sum_squares<< '\n';
}

void total_summary_header::pack(std::ostream& os) const {
  os.write((char*)&bases_covered, sizeof bases_covered);
  os.write((char*)&min_val, sizeof min_val);
  os.write((char*)&max_val, sizeof max_val);
  os.write((char*)&sum_data, sizeof sum_data);
  os.write((char*)&sum_squares, sizeof sum_squares);    
}

void total_summary_header::unpack(std::istream& os) {
  os.read((char*)&bases_covered, sizeof bases_covered);
  os.read((char*)&min_val, sizeof min_val);
  os.read((char*)&max_val, sizeof max_val);
  os.read((char*)&sum_data, sizeof sum_data);
  os.read((char*)&sum_squares, sizeof sum_squares);    
}
