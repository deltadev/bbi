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

std::streambuf* bbi::file_base::fill_stream(r_tree::leaf_node ln)
{
  buf.resize(ln.data_size);
  
  is_.seekg(ln.data_offset);
  is_.read((char*)buf.data(), buf.size());
  
  if (is_.gcount() != ln.data_size)
    throw std::runtime_error("file::inflate_records failed to read comp_sz bytes");
  
  if (main_header.uncompress_buf_size == 0)
  {
    setg((char*)buf.data(), (char*)buf.data(), (char*)buf.data() + buf.size());
  }
  else
  {
    auto p = decompressor.decompress(buf.data(), buf.data() + buf.size());
    setg((char*)p.first, (char*)p.first, (char*)p.second);
  }
  return this;
}

void bbi::file_base::init_total_summary_header()
{
  is_.seekg(main_header.total_summary_offset);
  ts_header.unpack(is_);
}
void bbi::file_base::init_num_records()
{
  is_.seekg(main_header.full_data_offset);
  num_records = 0;
  is_.read((char*)&num_records, 8);
}
  
void bbi::file_base::init_chrom_tree()
{
  is_.seekg(main_header.chromosome_tree_offset);
  chrom_tree.init(is_);
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
  if (level > zoom_headers.size())
  {
    level = static_cast<unsigned>(zoom_headers.size());
    std::cerr << "Warning: zoom level requested was greater than available.\n";
  }
  if (level == 0)
    is_.seekg(main_header.full_index_offset);
  else
    is_.seekg(zoom_headers[level - 1].index_offset);

  return bbi::index{is_};
}
  
void bbi::file_base::print_headers(std::ostream& os) {
    
  // Print main header.
  //
  os << "\n**** main_header ****\n\n";
  main_header.print(os);
    
  // Print total summary header.
  //
  os << "\n**** total_summary_header ****\n\n";
  ts_header.print(os);
    
  // Print chromosome tree header.
  //
  os << "\n**** chromosome tree header ****\n\n";
  chrom_tree.header.print(os);
  
  // Print the main r_tree header.
  //
  os << "\n**** main r-tree index header ****\n\n";
  os << index(0) << '\n';
    
  // Print the number of data records
  //
  os << "num records: " << num_records << '\n';
  
  // Print the zoom r_trees
  //
  // The zoom records start right after the main header.
  for (int i = 1; i <= main_header.zoom_levels; ++i)
  {
    os << "\n**** zoom header " << i << " ****\n\n";
    os << index(i) << '\n';
  }
}


