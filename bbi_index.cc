#include "bbi_index.h"

namespace bbi
{
  index::index(std::istream& is) : is_(is) { main_header.unpack(is_); }

  std::ostream& operator<<(std::ostream& os, index const& idx)
  { idx.main_header.print(os); return os; }
  
  std::vector<r_tree::leaf_node> index::search(bbi::record r)
  {
    std::vector<r_tree::leaf_node> leaves;
    
    r_tree::node_header nh;
    nh.unpack(is_);
    
    search_recursive(nh, r, leaves);
    
    return leaves;
  }
  
  
  void bbi::index::search_recursive(r_tree::node_header nh, bbi::record r,
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

        search_recursive(nnh, r, leaves);
        
        is_.seekg(sibling_offset);
      }
    }
  }
}
