#ifndef _BBI_STREAM_H_
#define _BBI_STREAM_H_

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
#include "wig_data_record.h"
#include "zoom_data_record.h"

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
  

  // Calls zlib to inflate the records.
  //
  template <typename T>
  std::vector<T>
  inflate_records(std::istream& is, uint64_t comp_sz, size_t decomp_sz);
  

};

template <typename T>
std::vector<T> bbi_file::records_for_leaf(r_tree::leaf_node ln)
{
  std::vector<T> bdrs;
  is_.seekg(ln.data_offset);
  
  uint32_t uncomp_buf_sz = main_hdr.uncompress_buf_size;
  if (uncomp_buf_sz == 0)
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
    bdrs = inflate_records<T>(is_, ln.data_size, uncomp_buf_sz);
  }
  
  return bdrs;
}

template <typename T>
std::vector<T>
bbi_file::inflate_records(std::istream& is, uint64_t comp_sz, size_t decomp_sz)
{
  int ret;
  unsigned have;
  z_stream strm;
  
  /* allocate inflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  if (ret != Z_OK) {
    std::string err_str = std::string(zError(ret));
    throw std::runtime_error("bbi_file::inflate_records inflateInit error: " + err_str);
  }
  
  
  // Buffers.
  //
  std::vector<unsigned char> out_buff(decomp_sz);
  std::vector<unsigned char> in_buff(comp_sz);
  is.read((char*)in_buff.data(), comp_sz);
  if (is.gcount() != comp_sz)
    throw std::runtime_error("bbi_file::inflate_records failed to read comp_sz bytes");
  
  strm.avail_in = (unsigned)in_buff.size();
  strm.next_in = in_buff.data();
  
  do {
    strm.avail_out = (unsigned)out_buff.size();
    strm.next_out = out_buff.data();
    ret = inflate(&strm, Z_NO_FLUSH);
    have = (unsigned)out_buff.size() - strm.avail_out;
    if (strm.avail_in == 0)
      break;
    
  } while (strm.avail_out == 0);
  
  if (ret != Z_STREAM_END) {
    std::string err_str = "ret != Z_STREAM_END";
    throw std::runtime_error("bbi_file::inflate_records " + err_str);
  }
  
  
  ret = inflateEnd(&strm);
  if (ret != Z_OK) {
    std::string err_str = "bad rv for inflateEnd";
    throw std::runtime_error("bbi_file::inflate_records " + err_str);
  }
  
  std::string output(out_buff.begin(), out_buff.end());
  std::istringstream iss;
  iss.str(output);
  
  std::vector<T> bdrs;
  while (iss) {
    T bdr;
    bdr.unpack(iss);
    if (bdr.chrom_start != bdr.chrom_end)
      bdrs.push_back(bdr);
  }
  
  return bdrs;
}


#endif /* _BBI_STREAM_H_ */
