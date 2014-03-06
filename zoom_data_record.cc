#include "zoom_data_record.h"

////////////////////////////////////////////////////////////////////////////
// zoom_data_record
// 
//     common to wig and bed
// 
////////////////////////////////////////////////////////////////////////////

void zoom_data_record::print(std::ostream& os) const {
  data_record::print(os);
  os << std::setw(25) << std::left << "valid_count" << valid_count << '\n';
  os << std::setw(25) << std::left << "min_val" << min_val << '\n';
  os << std::setw(25) << std::left << "max_val" << max_val << '\n';
  os << std::setw(25) << std::left << "sum_data" << sum_data << '\n';
  os << std::setw(25) << std::left << "sum_squares" << sum_squares << '\n';
}

void zoom_data_record::pack(std::ostream& os) const {
  data_record::pack(os);
  os.write((char*)&valid_count, sizeof valid_count);
  os.write((char*)&min_val, sizeof min_val);
  os.write((char*)&max_val, sizeof max_val);
  os.write((char*)&sum_data, sizeof sum_data);
  os.write((char*)&sum_squares, sizeof sum_squares);
}

void zoom_data_record::unpack(std::istream& os) {
  data_record::unpack(os);
  os.read((char*)&valid_count, sizeof valid_count);
  os.read((char*)&min_val, sizeof min_val);
  os.read((char*)&max_val, sizeof max_val);
  os.read((char*)&sum_data, sizeof sum_data);
  os.read((char*)&sum_squares, sizeof sum_squares);
}
