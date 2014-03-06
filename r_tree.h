#ifndef _R_TREE_H_
#define _R_TREE_H_

#include <stdint.h>

#include <string>
#include <iostream>


namespace r_tree {
  
  // The item_count field is set to the number of compressed block
  // for the main index header but to the number of records for zoom level
  // headers.
  //
  // TODO: Decide what to do about initialization. The default initial values
  //       might be of use for packing the data. When unpacking data it should
  //       be up to the user to value initialize if required.
  //
  struct header
  {
    static const uint32_t byte_size = 48;
    uint32_t magic;
    uint32_t block_size;
    uint64_t item_count;
    uint32_t start_chrom_ix;
    uint32_t start_base;
    uint32_t end_chrom_ix;
    uint32_t end_base;
    uint64_t end_file_offset;
    uint32_t items_per_slot;
    uint32_t reserved;
    
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os);
    
    header():magic(610839776), block_size(256),items_per_slot(512) {};
  };
  
  struct node_header 
  {
    static const uint32_t byte_size = 4;
    uint8_t is_leaf;
    uint8_t reserved;
    uint16_t count;
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os);
  };

  struct internal_node 
  {
    static const uint32_t byte_size = 24;

    uint32_t start_chrom_ix;
    uint32_t start_base;
    uint32_t end_chrom_ix;
    uint32_t end_base;
    uint64_t data_offset;

    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os);

  };

  struct leaf_node : internal_node
  {
    static const uint32_t byte_size = 32;
    uint64_t data_size;

    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os);   
  };
  
}



#endif /* _R_TREE_H_ */
