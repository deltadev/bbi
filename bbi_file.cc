#include "bbi_file.h"

#include "total_summary_header.h"



bbi_file::bbi_file(std::istream& is) : is_(is) {
    
  main_hdr.unpack(is_);
  if (main_hdr.magic == 2291137574)
  {
    file_type = bbi_type::wig;
  }
  decompressor.out_buf_size(main_hdr.uncompress_buf_size);
  
  init_chrom_tree();
  init_zoom_headers();
}
  
void bbi_file::init_chrom_tree() {
  is_.seekg(main_hdr.chromosome_tree_offset);
  chrom_tree.make_in_memory_hash(is_);

}
  
void bbi_file::init_zoom_headers() {
  is_.seekg(main_hdr.byte_size);
  for (int i = 0; i < main_hdr.zoom_levels; ++i) {
    zoom_header z_h{0};
    z_h.unpack(is_);
    zoom_headers.push_back(z_h);
  }
}


  
std::vector<r_tree::leaf_node>
bbi_file::search_r_tree(bbi::record r, int zoom_level)
{

  std::vector<r_tree::leaf_node> leaves;
    
  if (zoom_level == 0) {
      
    is_.seekg(main_hdr.full_index_offset);
      
    // This unpack only serves to move the get pointer to the correct place.
    //
    r_tree::header rt_header;
    rt_header.unpack(is_);      
      
    r_tree::node_header nh;
    nh.unpack(is_);
      
    recursive_rtree_find(nh, r, leaves);
      
  } else {
      
    is_.seekg(zoom_headers[zoom_level - 1].index_offset);
      
    // This unpack only serves to move the get pointer to the correct place.
    //
    r_tree::header rt_header;
    rt_header.unpack(is_);
      
    r_tree::node_header nh;
    nh.unpack(is_);
      
    recursive_rtree_find(nh, r, leaves);
      
  }
    
  return leaves;
}

void bbi_file::print_index_header(unsigned index, std::ostream& os)
{
  if (main_hdr.zoom_levels < index)
    throw std::runtime_error("print_index_header: index greater than number of zoom-levels");
  
  if (index == 0)
  {
    // Print the main data r_tree index header.
    //
    is_.seekg(main_hdr.full_index_offset);
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
  
void bbi_file::print_headers(std::ostream& os) {
    
  // Print main header.
  //
  os << "\n**** main_header ****\n\n";
  main_hdr.print(os);
    
  // Print total summary header.
  //
  is_.seekg(main_hdr.total_summary_offset);
  total_summary_header ts_header;
  ts_header.unpack(is_);
  os << "\n**** total_summary_header ****\n\n";
  ts_header.print(os);
    
  // Print chromosome tree header.
  //
  is_.seekg(main_hdr.chromosome_tree_offset);
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
  is_.seekg(main_hdr.full_data_offset);
  uint64_t num_records = 0;
  is_.read((char*)&num_records, 8);
  os << "num records: " << num_records << '\n';
  
  // Print the zoom r_trees
  //
  // The zoom records start right after the main header.
  for (int i = 0; i < main_hdr.zoom_levels; ++i) {
    os << "\n**** zoom header " << (i+1) << " ****\n\n";
    print_index_header(i+1, os);
  }
}

void bbi_file::recursive_rtree_find(r_tree::node_header nh, bbi::record r,
                                    std::vector<r_tree::leaf_node>& leaves)
{
    
  if (nh.is_leaf) {
      
    // We have leaf nodes...
    //
    for (int i = 0; i < nh.count; ++i) {
      r_tree::leaf_node l;
      l.unpack(is_);
      if (l.start_chrom_ix <= r.chrom_id && l.end_chrom_ix >= r.chrom_id)
      {
        // Strategy for leaf_nodes that span chroms:
        //
        //   * set r's endpoints to be uint32_t min/max values to ensure
        //     they bound any region on strictly lesser or greater chroms.
        //
        uint32_t r_a = r.chrom_start;
        uint32_t r_b = r.chrom_end;
        uint32_t const l_a = l.start_base;
        uint32_t const l_b = l.start_base;
        
        // "l_a is always less than r_b"
        //
        if (l.start_chrom_ix < r.chrom_id)
          r_b = std::numeric_limits<uint32_t>::max();

        // "l_b is always greater than r_a"
        //
        if (r.chrom_id < l.end_chrom_ix)
          r_a = 0;

        // Now check r overlaps l.
        //
        //              [l_a   l_b)
        //     [r_a    r_b)
        //
        //  &&
        //
        //     [l_a  l_b)
        //             [r_a  r_b)
        //
        if (l_a < r_b && r_a < l_b)
          leaves.push_back(l);
      }
    }
      
  } else {
      
    for (int i = 0; i < nh.count; ++i) {
        
      r_tree::internal_node in;
      in.unpack(is_);
        
      std::streamsize sibling_offset = is_.tellg();
        
      r_tree::node_header nnh; // Maybe this one will point to leaves...
      is_.seekg(in.data_offset);
      nnh.unpack(is_);
      recursive_rtree_find(nnh, r, leaves);
        
      is_.seekg(sibling_offset);
    }
  }
}



