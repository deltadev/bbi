#include "zoom_header.h"

#include <iomanip>

void zoom_header::print(std::ostream& os) const
{
  using std::setw; using std::left;
  os << setw(25) << left << "reduction_level" << reduction_level << '\n';
  os << setw(25) << left << "reserved" << reserved << '\n';
  os << setw(25) << left << "data_offset" << data_offset << '\n';
  os << setw(25) << left << "index_offset" << index_offset << '\n';
}


void zoom_header::pack(std::ostream& os) const
{
  os.write((char*)&reduction_level, sizeof reduction_level);
  os.write((char*)&reserved, sizeof reserved);
  os.write((char*)&data_offset, sizeof data_offset);
  os.write((char*)&index_offset, sizeof index_offset);
}

void zoom_header::unpack(std::istream& is)
{
  is.read((char*)&reduction_level, sizeof reduction_level);
  is.read((char*)&reserved, sizeof reserved);
  is.read((char*)&data_offset, sizeof data_offset);
  is.read((char*)&index_offset, sizeof index_offset);
}
