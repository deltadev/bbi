#include "bp_tree.h"

#include <vector>

#include <iomanip>

namespace bp_tree {
  
  
  void header::print(std::ostream& os) const {
    os << std::setw(25) << std::left << "magic" << magic << '\n';
    os << std::setw(25) << std::left << "block_size" << block_size << '\n';
    os << std::setw(25) << std::left << "key_size" << key_size << '\n';
    os << std::setw(25) << std::left << "val_size" << val_size << '\n';
    os << std::setw(25) << std::left << "item_count" << item_count << '\n';
    os << std::setw(25) << std::left << "reserved" << reserved << '\n';
  }
  
  void header::pack(std::ostream& os) const {
    os.write((char*)&magic, sizeof magic);
    os.write((char*)&block_size, sizeof block_size);
    os.write((char*)&key_size, sizeof key_size);
    os.write((char*)&val_size, sizeof val_size);
    os.write((char*)&item_count, sizeof item_count);
    os.write((char*)&reserved, sizeof reserved);
  }
  
  void header::unpack(std::istream& os) {
    os.read((char*)&magic, sizeof magic);
    os.read((char*)&block_size, sizeof block_size);
    os.read((char*)&key_size, sizeof key_size);
    os.read((char*)&val_size, sizeof val_size);
    os.read((char*)&item_count, sizeof item_count);
    os.read((char*)&reserved, sizeof reserved);
  }
  
  
  
  
  void header_node::print(std::ostream& os) const {
    os << std::setw(25) << std::left << "is_leaf" << std::boolalpha << (is_leaf == 1) << '\n';
    os << std::setw(25) << std::left << "reserved" << (int)reserved << '\n';
    os << std::setw(25) << std::left << "count" << count << '\n';
  }
  
  void header_node::pack(std::ostream& os) const {
    os.write((char*)&is_leaf, sizeof is_leaf);
    os.write((char*)&reserved, sizeof reserved);
    os.write((char*)&count, sizeof count);
  }
  
  void header_node::unpack(std::istream& os) {
    os.read((char*)&is_leaf, sizeof is_leaf);
    os.read((char*)&reserved, sizeof reserved);
    os.read((char*)&count, sizeof count);
  }
  
  void base_node::print(std::ostream& os) const {
    os << std::setw(25) << std::left << "key" << key << '\n';
  }
  void base_node::pack(std::ostream& os) const {
    os.write(key.c_str(), key.size());
  }
  void base_node::unpack(std::istream& is, unsigned key_size) {
    // FIXME: What is a reasonable upper bound for the size of the string rest?
    //        Have chosen 4 * 1024 here.
    //
    std::vector<char> buff(key_size);
    is.read(buff.data(), buff.size());
    key.assign(buff.begin(), buff.end());
  }
  
  
  void internal_node::print(std::ostream& os) const {
    base_node::print(os);
    os << std::setw(25) << std::left << "child_offset" << child_offset << '\n';
  }
  void internal_node::pack(std::ostream& os) const {
    base_node::pack(os);
    os.write((char*)&child_offset, sizeof child_offset);
  }
  void internal_node::unpack(std::istream& is, unsigned key_size) {
    base_node::unpack(is, key_size);
    is.read((char*)&child_offset, sizeof child_offset);
  }
  
  
  void leaf_node::print(std::ostream& os) const {
    base_node::print(os);
    os << std::setw(25) << std::left << "chrom_id" << chrom_id << '\n';
    os << std::setw(25) << std::left << "chrom_size" << chrom_size << '\n';
  }
  void leaf_node::pack(std::ostream& os) const {
    base_node::pack(os);
    os.write((char*)&chrom_id, sizeof chrom_id);
    os.write((char*)&chrom_size, sizeof chrom_size);
    
  }
  void leaf_node::unpack(std::istream& is, unsigned key_size) {
    base_node::unpack(is, key_size);
    is.read((char*)&chrom_id, sizeof chrom_id);
    is.read((char*)&chrom_size, sizeof chrom_size);
  }
  
}
