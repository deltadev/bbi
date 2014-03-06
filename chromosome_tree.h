#ifndef _CHROMOSOME_TREE_H_
#define _CHROMOSOME_TREE_H_

#include <stdint.h>

#include <string>
#include <iostream>

#include <map>

#include "bp_tree.h"



class chromosome_tree
{
  
  // This is bad.
  //
  bp_tree::header header;
  
public:

  uint32_t chrom_id(std::string chrom_name);
  uint32_t chrom_size(std::string chrom_name);

  void make_in_memory_hash(std::istream& is);
  void print(std::ostream&);

private:
  void recursive_fill_map(bp_tree::header_node n_h, std::istream& is);

  std::map<std::string, bp_tree::leaf_node> id_hash;
  
};


#endif /* _CHROMOSOME_TREE_H_ */
