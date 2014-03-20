#ifndef DPJ_BBI_INDEX_H_
#define DPJ_BBI_INDEX_H_

#include <vector>

#include "r_tree.h"
#include "record.h"

namespace bbi
{
  
  class index
  {
    r_tree::header main_header;
    
  public:
    
    // ctor upacks main header.
    //
    index(std::istream& is);
    
    // The zoom level should be 0 for the main data or otherwise selected from the z_hdrs.
    //
    std::vector<r_tree::leaf_node> search(bbi::record r);

    private:

    std::istream& is_;
    // Recursive helper for r_tree search.
    //
    void search_recursive(r_tree::node_header nh, bbi::record r, std::vector<r_tree::leaf_node>& leaves);

    friend std::ostream& operator<<(std::ostream&, index const& idx);
  };
  

  
}
#endif /* DPJ_BBI_INDEX_H_ */
