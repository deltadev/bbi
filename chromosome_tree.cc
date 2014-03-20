#include "chromosome_tree.h"

#include <ostream>
#include <iomanip>

#include <vector>

void chromosome_tree::init(std::istream& is)
{
  header.unpack(is);
  make_in_memory_hash(is);
}

void chromosome_tree::print(std::ostream& os)
{
  for (auto const& p : id_hash)
  {
    auto const& s = p.first;
    os << "(" << s << ", " << chrom_id(s) << ", " << chrom_size(s) << ")\n";
  }
}

uint32_t chromosome_tree::chrom_id(std::string chrom_name) {
  auto it = id_hash.find(chrom_name);
  if (it == id_hash.end())
    throw std::runtime_error("chrom_id: chrom-name not found in index");
  return it->second.chrom_id;
}

uint32_t chromosome_tree::chrom_size(std::string chrom_name) {
  auto it = id_hash.find(chrom_name);
  if (it == id_hash.end())
    throw std::runtime_error("chrom_size: chrom-name not found in index");
  return it->second.chrom_size;
}


void chromosome_tree::make_in_memory_hash(std::istream& is) {
  
  bp_tree::header_node n_hdr;
  n_hdr.unpack(is);
  
  recursive_fill_map(n_hdr, is);
  
}

void chromosome_tree::recursive_fill_map(bp_tree::header_node n_h, std::istream& is) {
  
  if (n_h.is_leaf == false) {
    
    // Drops down a level for each node header pointed to by this
    // internal_node. Probably never happens since the number of chromosomes is small.
    //
    for (int i = 0; i < n_h.count; ++i) {
      
      bp_tree::internal_node in_node;
      in_node.unpack(is, header.key_size);
      
      // This is the next internal_node position, out sibling, so save pos for next
      // time through the loop.
      //
      std::streamsize curr_off = is.tellg();
      
      // Now go down a level.
      //
      is.seekg(in_node.child_offset);
      bp_tree::header_node n_h_n;
      n_h_n.unpack(is);
      recursive_fill_map(n_h_n, is);
      
      // We're done below so seek back to our sibling's position.
      is.seekg(curr_off);
      
    }
    
  } else {

    // We're on the leaf level so we can just fill up the hash map.
    //
    for (int i = 0; i < n_h.count; ++i) {
      bp_tree::leaf_node ln;
      ln.unpack(is, header.key_size);
      id_hash[ln.key] = ln;
    }
    
  }
}
