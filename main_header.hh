#ifndef DPJ_MAIN_HEADER_H_
#define DPJ_MAIN_HEADER_H_


#include <cstdint>
#include <iostream>

struct main_header
{
  static const uint16_t byte_size = 64;

  uint32_t magic;
  uint16_t version;
  uint16_t zoom_levels;
  uint64_t bp_tree_offset;
  uint64_t full_data_offset;
  uint64_t full_index_offset;
  uint16_t field_count;
  uint16_t defined_field_count;
  uint64_t auto_sql_offset;
  uint64_t total_summary_offset;
  uint32_t uncompress_buf_size;
  uint64_t extension_offset;

  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);

};

#endif /* DPJ_MAIN_HEADER_H_ */
