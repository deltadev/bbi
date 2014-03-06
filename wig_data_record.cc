#include "wig_data_record.h"


///////////////////////////////////////////////////////////////////////////
// wig_data_record
// 
///////////////////////////////////////////////////////////////////////////
void wig_data_record::print(std::ostream& os) const {
  data_record::print(os);
  os << std::setw(25) << std::left << "item_step" << item_step << '\n';
  os << std::setw(25) << std::left << "item_span" << item_span << '\n';
  os << std::setw(25) << std::left << "type" << type << '\n';
  os << std::setw(25) << std::left << "reserved" << reserved << '\n';
  os << std::setw(25) << std::left << "item_count" << item_count << '\n';
}

void wig_data_record::pack(std::ostream& os) const {
  data_record::pack(os);
  os.write((char*)&item_step, sizeof item_step);
  os.write((char*)&item_span, sizeof item_span);
  os.write((char*)&type, sizeof type);
  os.write((char*)&reserved, sizeof reserved);
  os.write((char*)&item_count, sizeof item_count);
}

void wig_data_record::unpack(std::istream& os) {
  data_record::unpack(os);
  os.read((char*)&item_step, sizeof item_step);
  os.read((char*)&item_span, sizeof item_span);
  os.read((char*)&type, sizeof type);
  os.read((char*)&reserved, sizeof reserved);
  os.read((char*)&item_count, sizeof item_count);
}
