#ifndef _BP_TREE_H_
#define _BP_TREE_H_




#include <stdint.h>
#include <iostream>


namespace bp_tree {


  struct header 
  {
    uint32_t magic;
    uint32_t block_size;
    uint32_t key_size;
    uint32_t val_size;
    uint64_t item_count;
    uint64_t reserved;
  
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os);

    //default constructor
    header():
      magic(2026540177), 
      block_size(256),
      key_size(0),
      val_size(0),
      item_count(0),
      reserved(0) 
    {}
  };


  struct header_node {
    uint8_t is_leaf;
    uint8_t reserved;
    uint16_t count;
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os);
  };


  struct base_node {
    std::string key;
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os, unsigned key_size);
  };

  struct internal_node : base_node {
    uint64_t child_offset;
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os, unsigned key_size);
  };

  struct leaf_node : base_node {
    uint32_t chrom_id;
    uint32_t chrom_size;
    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& os, unsigned key_size);
  };

}


#endif /* _BP_TREE_H_ */
