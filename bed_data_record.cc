#include "bed_data_record.h"

///////////////////////////////////////////////////////////////////////////
// bed_data_record
// 
///////////////////////////////////////////////////////////////////////////
void bed_data_record::print(std::ostream& os) const {
  data_record::print(os);
  os << std::setw(25) << std::left << "rest" << rest << '\n';
}

void bed_data_record::pack(std::ostream& os) const {
  data_record::pack(os);
  os.write(rest.c_str(), rest.size());
}

void bed_data_record::unpack(std::istream& is) {
  data_record::unpack(is);

  // FIXME: What is a reasonable upper bound for the size of the string rest?
  //        Have chosen 4 * 1024 here.
  //
  std::array<char, 4 * 1024> buff;
  is.getline(buff.data(), 4 * 1024, '\0');
  std::streamsize count = is.gcount();
  if (count > 0)
    rest.assign(buff.data(), count - 1);
}
