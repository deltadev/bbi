#ifndef DPJ_CHROMOSOME_TREE_H_
#define DPJ_CHROMOSOME_TREE_H_

#include <cstdint>

#include <string>
#include <iostream>

#include <map>

#include "bp_tree.h"



class chromosome_tree
{  
public:
  bp_tree::header header;
  
  void init(std::istream& is);
  
  uint32_t chrom_id(std::string chrom_name);
  uint32_t chrom_size(std::string chrom_name);

  void make_in_memory_hash(std::istream& is);
  void print(std::ostream&);

private:
  void recursive_fill_map(bp_tree::header_node n_h, std::istream& is);

  std::map<std::string, bp_tree::leaf_node> id_hash;
  
};


#endif /* DPJ_CHROMOSOME_TREE_H_ */
