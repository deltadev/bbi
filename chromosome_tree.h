#ifndef _CHROMOSOME_TREE_H_
#define _CHROMOSOME_TREE_H_

#include <cstdint>

#include <string>
#include <iostream>

#include <unordered_map>

#include "bp_tree.h"



class chromosome_tree
{
  
  // This is bad.
  //
  bp_tree::header header;
  
public:

  uint32_t id_for_chrom_name(std::string chrom_name);
  uint32_t chrom_size_for_chrom_name(std::string chrom_name);

  void make_in_memory_hash(std::istream& is);

private:
  void recursive_fill_map(bp_tree::header_node n_h, std::istream& is);

  std::unordered_map<std::string, bp_tree::leaf_node> id_hash;
  
};


#endif /* _CHROMOSOME_TREE_H_ */
