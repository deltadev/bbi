#include "data_record.h"

#include <array>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////
// These methods are not virtual, they are intended to be statically
// dispatched only.
// 
////////////////////////////////////////////////////////////////////////////
void data_record::print(std::ostream& os) const {
  os << std::setw(25) << std::left << "chrom_id" << chrom_id << '\n';
  os << std::setw(25) << std::left << "chrom_start" << chrom_start << '\n';
  os << std::setw(25) << std::left << "chrom_end" << chrom_end << '\n';
}
void data_record::pack(std::ostream& os) const {
  os.write((char*)&chrom_id, sizeof chrom_id);
  os.write((char*)&chrom_start, sizeof chrom_start);
  os.write((char*)&chrom_end, sizeof chrom_end);
}
void data_record::unpack(std::istream& os) {
  os.read((char*)&chrom_id, sizeof chrom_id);
  os.read((char*)&chrom_start, sizeof chrom_start);
  os.read((char*)&chrom_end, sizeof chrom_end);
}






