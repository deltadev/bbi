#ifndef DPJ_BBI_STREAM_H_
#define DPJ_BBI_STREAM_H_

#include <type_traits>

#include <vector>
#include <fstream>

#include <zlib.h>
#include <sstream>

#include "main_header.h"
#include "zoom_header.h"

#include "r_tree.h"
#include "bp_tree.h"
#include "chromosome_tree.h"

#include "bed_record.h"
#include "wig_record.h"
#include "zoom_record.h"

#include "block_decompressor.h"

class bbi_file
{
public:
  enum class bbi_type : unsigned
  {
    wig = 2291157574, // little endian hex: 464a9088
    bed = 2273964779  // little endian hex: ebf28987
  };
  bbi_type file_type;
  
  std::vector<zoom_header> zoom_headers;
  chromosome_tree          chrom_tree;
  
  bbi_file(std::istream& is);

  // For info on headers including zoom headers when available.
  //
  void print_headers(std::ostream& os);
  
  void print_index_header(unsigned index, std::ostream& os);

  
  // The zoom level should be 0 for the main data or otherwise selected from the z_hdrs.
  //
  std::vector<r_tree::leaf_node> search_r_tree(bbi::record r, int zoom_level);

  // Obtains data records from r_tree leaf node.
  //
  template <typename T> std::vector<T> records(r_tree::leaf_node ln)
  {
    buf.resize(ln.data_size);
    
    is_.seekg(ln.data_offset);
    is_.read((char*)buf.data(), buf.size());

    if (is_.gcount() != ln.data_size)
      throw std::runtime_error("bbi_file::inflate_records failed to read comp_sz bytes");
    
    return records_dispatch<T>();
  }
  
private:
  std::vector<uint8_t> buf;
  
  template <typename T>
  std::vector<typename std::enable_if<bbi::is_wig_type<T>::value, T>::type>
  records_dispatch()
  {
    std::istringstream iss;
    bbi::wig::header hdr;
    if (main_hdr.uncompress_buf_size == 0)
    {
      iss.str({begin(buf), end(buf)});
    }
    else
    {
      auto pair = decompressor.decompress(buf.data(), buf.data() + buf.size());
      iss.str({pair.first, pair.second});
    }
    hdr.unpack(iss);
    return bbi::extract<T>(iss, hdr.item_count);
  }
  
  template <typename T>
  std::vector<typename std::enable_if<!bbi::is_wig_type<T>::value, T>::type>
  records_dispatch()
  {
    std::istringstream iss;
    if (main_hdr.uncompress_buf_size == 0)
    {
      iss.str({begin(buf), end(buf)});
    }
    else
    {
      auto pair = decompressor.decompress(buf.data(), buf.data() + buf.size());
      iss.str({pair.first, pair.second});
    }
    return bbi::extract<T>(iss);
  }
  
  std::istream& is_;
  main_header main_hdr;
  bp_tree::header bpt_hdr;
  r_tree::header main_rt_hdr;

  block_decompressor decompressor;

  // Called by constructor.
  //
  void init_chrom_tree();
  void init_zoom_headers();
  
  // Recursive helper for r_tree search.
  //
  void recursive_rtree_find(r_tree::node_header nh, bbi::record r,
                            std::vector<r_tree::leaf_node>& leaves);
  
};






#endif /* DPJ_BBI_STREAM_H_ */
