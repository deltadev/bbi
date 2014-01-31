#include "main_header.h"

#include <iomanip>

void main_header::pack(std::ostream& os) const
{
  os.write((char*)&magic, sizeof magic);
  os.write((char*)&version, sizeof version);
  os.write((char*)&zoom_levels, sizeof zoom_levels);
  os.write((char*)&chromosome_tree_offset, sizeof chromosome_tree_offset);
  os.write((char*)&full_data_offset, sizeof full_data_offset);
  os.write((char*)&full_index_offset, sizeof full_index_offset);
  os.write((char*)&field_count, sizeof field_count);
  os.write((char*)&defined_field_count, sizeof defined_field_count);
  os.write((char*)&auto_sql_offset, sizeof auto_sql_offset);
  os.write((char*)&total_summary_offset, sizeof total_summary_offset);
  os.write((char*)&uncompress_buf_size, sizeof uncompress_buf_size);
  os.write((char*)&extension_offset, sizeof extension_offset);
}

void main_header::unpack(std::istream& os)
{
  os.read((char*)&magic, sizeof magic);
  os.read((char*)&version, sizeof version);
  os.read((char*)&zoom_levels, sizeof zoom_levels);
  os.read((char*)&chromosome_tree_offset, sizeof chromosome_tree_offset);
  os.read((char*)&full_data_offset, sizeof full_data_offset);
  os.read((char*)&full_index_offset, sizeof full_index_offset);
  os.read((char*)&field_count, sizeof field_count);
  os.read((char*)&defined_field_count, sizeof defined_field_count);
  os.read((char*)&auto_sql_offset, sizeof auto_sql_offset);
  os.read((char*)&total_summary_offset, sizeof total_summary_offset);
  os.read((char*)&uncompress_buf_size, sizeof uncompress_buf_size);
  os.read((char*)&extension_offset, sizeof extension_offset);
}

void main_header::print(std::ostream& os) const
{
  using std::setw; using std::left;
  os << setw(25) << left << "magic" << magic << '\n';
  os << setw(25) << left << "version" << version << '\n';
  os << setw(25) << left << "zoom_levels" << zoom_levels << '\n';
  os << setw(25) << left << "chromosome_tree_offset" << chromosome_tree_offset << '\n';
  os << setw(25) << left << "full_data_offset" << full_data_offset << '\n';
  os << setw(25) << left << "full_index_offset" << full_index_offset << '\n';
  os << setw(25) << left << "field_count" << field_count << '\n';
  os << setw(25) << left << "defined_field_count" << defined_field_count << '\n';
  os << setw(25) << left << "auto_sql_offset" << auto_sql_offset << '\n';
  os << setw(25) << left << "total_summary_offset" << total_summary_offset << '\n';
  os << setw(25) << left << "uncompress_buf_size" << uncompress_buf_size << '\n';
  os << setw(25) << left << "extension_offset" << extension_offset << '\n';
}
