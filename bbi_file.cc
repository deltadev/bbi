#include "bbi_file.h"

#include "total_summary_header.h"



bbi::file_base::file_base(std::istream& is) : is_(is) {
  is_.seekg(0);
  main_header.unpack(is_);
  
  bool is_bed = main_header.magic == static_cast<unsigned>(bbi::file_type::bed);
  file_type = is_bed ? bbi::file_type::bed : bbi::file_type::wig;

  decompressor.decomp_buf_size(main_header.uncompress_buf_size);
  
  init_chrom_tree();
  init_zoom_headers();
}
  
void bbi::file_base::init_chrom_tree()
{
  is_.seekg(main_header.chromosome_tree_offset);
  chrom_tree.make_in_memory_hash(is_);
}
  
void bbi::file_base::init_zoom_headers()
{
  is_.seekg(main_header.byte_size);
  for (int i = 0; i < main_header.zoom_levels; ++i) {
    zoom_header z_h{0};
    z_h.unpack(is_);
    zoom_headers.push_back(z_h);
  }
}

bbi::index bbi::file_base::index(unsigned level)
{
  if (level == 0)
    is_.seekg(main_header.full_index_offset);
  else
    is_.seekg(zoom_headers[level - 1].index_offset);

  return bbi::index{is_};
}
  

void bbi::file_base::print_index_header(unsigned index, std::ostream& os)
{
  if (main_header.zoom_levels < index)
    throw std::runtime_error("print_index_header: index greater than number of zoom-levels");
  
  if (index == 0)
  {
    is_.seekg(main_header.full_index_offset);
    r_tree::header r_header;
    r_header.unpack(is_);
    r_header.print(os);
  }
  else
  {
    // Otherwise print the zoom r-tree index header
    //
    is_.seekg(main_header::byte_size + index * zoom_header::byte_size);
    zoom_header z_h;
    z_h.unpack(is_);
    z_h.print(os);
    os << "\n";
    
    r_tree::header z_rt_h;
    is_.seekg(z_h.index_offset);
    z_rt_h.unpack(is_);
    z_rt_h.print(os);
  }
}
  
void bbi::file_base::print_headers(std::ostream& os) {
    
  // Print main header.
  //
  os << "\n**** main_header ****\n\n";
  main_header.print(os);
    
  // Print total summary header.
  //
  is_.seekg(main_header.total_summary_offset);
  total_summary_header ts_header;
  ts_header.unpack(is_);
  os << "\n**** total_summary_header ****\n\n";
  ts_header.print(os);
    
  // Print chromosome tree header.
  //
  is_.seekg(main_header.chromosome_tree_offset);
  bp_tree::header bp_header;
  bp_header.unpack(is_);
  os << "\n**** chromosome tree header ****\n\n";
  bp_header.print(os);
  
  // Print the main r_tree header.
  //
  os << "\n**** main r-tree index header ****\n\n";
  print_index_header(0, os);
    
  // Print the number of data records
  //
  is_.seekg(main_header.full_data_offset);
  uint64_t num_records = 0;
  is_.read((char*)&num_records, 8);
  os << "num records: " << num_records << '\n';
  
  // Print the zoom r_trees
  //
  // The zoom records start right after the main header.
  for (int i = 0; i < main_header.zoom_levels; ++i) {
    os << "\n**** zoom header " << (i+1) << " ****\n\n";
    print_index_header(i+1, os);
  }
}


