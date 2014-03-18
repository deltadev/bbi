#ifndef DPJ_BBI_STREAM_H_
#define DPJ_BBI_STREAM_H_

#include <vector>
#include <fstream>

#include <zlib.h>
#include <sstream>

#include "main_header.h"
#include "zoom_header.h"

#include "r_tree.h"
#include "bp_tree.h"
#include "chromosome_tree.h"

#include "bed_data_record.h"

#include "wig_record.h"

#include "zoom_data_record.h"

#include "block_decompressor.h"

// Local bbi file searching.
//
// The constructer calls the init functions.
//
// After this the hash_map of chrom names is set up to begin a search.
//
//
class bbi_file
{
public:
  
  // BigBed magic: 2273964779, little endian hex: ebf28987
  // BigWig magic: 2291157574, little endian hex: 464a9088
  
  enum class bbi_type : unsigned
  {
    wig = 2291157574,
    bed = 2273964779
  };
  
  typedef std::vector<r_tree::leaf_node> leaves_t;

  std::vector<zoom_header> z_hdrs;
  chromosome_tree chrom_tree;
  
  bbi_file(std::istream& is);
  ~bbi_file();

  // For info on headers including zoom headers when available.
  //
  void print_headers(std::ostream& os);
  
  void print_index_header(unsigned index, std::ostream& os);

  
  
  // The zoom level should be 0 for the main data or otherwise selected
  // from the z_hdrs.
  //
  leaves_t const& search_r_tree(data_record r, int zoom_level);


  // Obtains data records from r_tree leaf node.
  //
  template <typename T>
  std::vector<T> records_for_leaf(r_tree::leaf_node ln);
  
private:
  
  std::istream& is_;
  main_header main_hdr;
  bp_tree::header bpt_hdr;
  r_tree::header main_rt_hdr;

  block_decompressor decompressor;

  // For the return results of the recursive r_tree search.
  //
  leaves_t leaves;


  // Called by constructor.
  //
  void init_chrom_tree();
  void init_zoom_headers();
  

  // Recursive helper for r_tree search.
  //
  void recursive_rtree_find(r_tree::node_header nh, data_record r);

};


// This is no longer a template and so must be an inline function in order to avoid link errors.
template <> inline
std::vector<wig::bed_graph> bbi_file::records_for_leaf<wig::bed_graph>(r_tree::leaf_node ln)
{
  is_.seekg(ln.data_offset);
  
  if (main_hdr.uncompress_buf_size == 0)
  {
    wig::header hdr;
    hdr.unpack(is_);
    return wig::extract<wig::bed_graph>(is_, hdr.item_count);
  }
  else
  {
    std::vector<uint8_t> in_buf(ln.data_size);
    is_.read((char*)in_buf.data(), in_buf.size());
    if (is_.gcount() != ln.data_size)
      throw std::runtime_error("bbi_file::inflate_records failed to read comp_sz bytes");

    auto pair = decompressor.decompress(in_buf.data(), in_buf.data() + in_buf.size());

    wig::header hdr{pair.first};
    return wig::extract<wig::bed_graph>(pair.first + sizeof (wig::header), hdr.item_count);
  }
}

template <typename T>
std::vector<T> bbi_file::records_for_leaf(r_tree::leaf_node ln)
{
  std::vector<T> bdrs;
  is_.seekg(ln.data_offset);
  
  if (main_hdr.uncompress_buf_size == 0)
  {
    T bdr;
    while (is_.tellg() < ln.data_offset + ln.data_size)
    {
      bdr.unpack(is_);
      bdrs.push_back(bdr);
    }
  }
  else
  {
    std::vector<unsigned char> in_buf(ln.data_size);
    is_.read((char*)in_buf.data(), in_buf.size());
    if (is_.gcount() != in_buf.size())
      throw std::runtime_error("bbi_file::inflate_records failed to read comp_sz bytes");
    
    auto pair = decompressor.decompress(in_buf.data(), in_buf.data() + in_buf.size());
    
    std::istringstream iss{{pair.first, pair.second}};
    
    while (iss) {
      T bdr;
      bdr.unpack(iss);
      if (bdr.chrom_start != bdr.chrom_end)
        bdrs.push_back(bdr);
    }
  }
  
  return bdrs;
}

#endif /* DPJ_BBI_STREAM_H_ */
