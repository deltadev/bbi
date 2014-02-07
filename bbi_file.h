#ifndef _BBI_STREAM_H_
#define _BBI_STREAM_H_

#include <vector>
#include <fstream>

#include "main_header.h"
#include "zoom_header.h"
#include "data_records.h"
#include "r_tree.h"
#include "bp_tree.h"
#include "chromosome_tree.h"

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

  bbi_file(std::istream& is);
  ~bbi_file();
  

  // For info on headers including zoom headers when available.
  //
  void print_headers(std::ostream& os);
  
  
  // The zoom level should be 0 for the main data or otherwise selected
  // from the z_hdrs.
  //
  leaves_t const& search_r_tree(data_record r, int zoom_level);


  // Obtains data records from r_tree leaf node.
  //
  template <typename T>
  inline std::vector<T> records_for_leaf(r_tree::leaf_node ln);

  chromosome_tree chrom_tree;
  
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
  void recursive_rtree_find(r_tree::node_header nh, data_record r);
  

  // Helper.
  template <typename T>
  inline std::vector<T>
  inflate_records(std::istream& is, uint64_t comp_sz, size_t decomp_sz);


};

#include "bbi_file.hh"

#endif /* _BBI_STREAM_H_ */
