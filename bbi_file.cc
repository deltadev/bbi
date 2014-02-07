#include "bbi_file.h"

#include "total_summary_header.h"



bbi_file::bbi_file(std::istream& is) : is_(is) {
    
  main_hdr.unpack(is_);
    
  init_chrom_tree();
  init_zoom_headers();
}
  
bbi_file::~bbi_file() {  }
  
void bbi_file::init_chrom_tree() {
  is_.seekg(main_hdr.chromosome_tree_offset);
  chrom_tree.make_in_memory_hash(is_);

}
  
void bbi_file::init_zoom_headers() {
  is_.seekg(main_hdr.byte_size);
  for (int i = 0; i < main_hdr.zoom_levels; ++i) {
    zoom_header z_h{0};
    z_h.unpack(is_);
    z_hdrs.push_back(z_h);
  }
}


  
std::vector<r_tree::leaf_node> const& 
bbi_file::search_r_tree(data_record r, int zoom_level) {

  leaves.clear();
    
  if (zoom_level == 0) {
      
    is_.seekg(main_hdr.full_index_offset);
      
    // This unpack only serves to move the get pointer to the correct place.
    //
    r_tree::header rt_header;
    rt_header.unpack(is_);      
      
    r_tree::node_header nh;
    nh.unpack(is_);
      
    recursive_rtree_find(nh, r);
      
  } else {
      
    is_.seekg(z_hdrs[zoom_level].index_offset);
      
    // This unpack only serves to move the get pointer to the correct place.
    //
    r_tree::header rt_header;
    rt_header.unpack(is_);
      
    r_tree::node_header nh;
    nh.unpack(is_);
      
    recursive_rtree_find(nh, r);
      
  }
    
  return leaves;
}
  
  
void bbi_file::print_headers(std::ostream& os) {
    
  // Print main header.
  //
  is_.seekg(0);
  main_header header;
  header.unpack(is_);
  os << "\n**** main_header ****\n\n";
  header.print(os);
    
  // Print total summary header.
  //
  is_.seekg(header.total_summary_offset);
  total_summary_header ts_header;
  ts_header.unpack(is_);
  os << "\n**** total_summary_header ****\n\n";
  ts_header.print(os);
    
  // Print chromosome tree offset
  //
  is_.seekg(header.chromosome_tree_offset);
  bp_tree::header bp_header;
  bp_header.unpack(is_);
  os << "\n**** chromosome tree header ****\n\n";
  bp_header.print(os);
    
    
  // Print the main r_tree header.
  //
  is_.seekg(header.full_index_offset);
  r_tree::header r_header;
  r_header.unpack(is_);
  os << "\n**** main r-tree index header ****\n\n";
  r_header.print(os);
    
  // Print the number of data records
  //
  is_.seekg(header.full_data_offset);
  uint64_t num_records = 0;
  is_.read((char*)&num_records, 8);
  os << "num records: " << num_records << '\n';
    
    
  // Print the zoom r_trees
  //
  // The zoom records start right after the header.
  for (int i = 0; i < header.zoom_levels; ++i) {
      
    is_.seekg(main_header::byte_size + i * zoom_header::byte_size);
      
    os << "\n**** " << (i+1) << "th zoom header ****\n\n";
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
void bbi_file::recursive_rtree_find(r_tree::node_header nh, data_record r) {
    
  if (nh.is_leaf) {
      
    // We have leaf nodes...
    //
    for (int i = 0; i < nh.count; ++i) {
      r_tree::leaf_node ln;
      ln.unpack(is_);
      if (ln.start_chrom_ix <= r.chrom_id && ln.end_chrom_ix >= r.chrom_id) {
        //
        // This interval of chromosomes covers our record. We think it's probably just one
        // chromosome so we'll deal with only that case at the moment.
        //
        // FIXME: Deal with the case where this r tree record covers several chromosomes.
        //
          
        // If either of the end points of our record lie within the
        //
        // [ln.start_base, ln.end_base)
        //
        // Then we are int he correct interval
        //
        if (   (ln.start_base <= r.chrom_end && ln.end_base > r.chrom_end)
               || (ln.start_base <= r.chrom_start && ln.end_base > r.chrom_start))
        {
          leaves.push_back(ln);
        }
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
      recursive_rtree_find(nnh, r);
        
      is_.seekg(sibling_offset);
    }
  }
}



