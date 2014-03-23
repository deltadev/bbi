#ifndef DPJ_BBI_INDEX_HH_
#define DPJ_BBI_INDEX_HH_

#include <vector>

#include "r_tree.hh"
#include "record.hh"

namespace bbi
{
  
  class index
  {
    r_tree::header main_header;

  public:
    
    // ctor upacks main header.
    //
    index(std::streambuf* streambuf) : streambuf{streambuf}, main_header{streambuf} { }
    
    friend std::ostream& operator<<(std::ostream& os, index const& idx)
    { print(idx.main_header, os); return os; }
    
    friend std::vector<r_tree::leaf_node> search(index& index, bbi::record r)
    {
      std::vector<r_tree::leaf_node> leaves;
      
      r_tree::node_header nh;
      unpack(nh, index.streambuf);
      
      index.search_recursive(nh, r, leaves);
      
      return leaves;
    }
    
  private:
    
    std::streambuf* streambuf;
    // Recursive helper for r_tree search.
    //
    void search_recursive(r_tree::node_header nh, bbi::record r, std::vector<r_tree::leaf_node>& leaves)
    {
      if (nh.is_leaf) {
        
        // We have leaf nodes...
        //
        for (int i = 0; i < nh.count; ++i) {
          r_tree::leaf_node l;
          unpack(l, streambuf);
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
            uint32_t const l_b = l.end_base;
            
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
          unpack(in, streambuf);
          
          std::streamsize sibling_offset = streambuf->pubseekoff(0, std::ios_base::cur);
          
          r_tree::node_header nnh; // Maybe this one will point to leaves...
          streambuf->pubseekpos(in.data_offset);

          unpack(nnh, streambuf);
          
          search_recursive(nnh, r, leaves);
          
          streambuf->pubseekpos(sibling_offset);
        }
      }
    }
    
  };
  

  
}
#endif /* DPJ_BBI_INDEX_HH_ */
