#include "r_tree.h"

#include <iomanip>

namespace r_tree {
  
  
  
  void header::print(std::ostream& os) const {
    os << std::setw(25) << std::left << "magic" << magic << '\n';
    os << std::setw(25) << std::left << "block_size" << block_size << '\n';
    os << std::setw(25) << std::left << "item_count" << item_count << '\n';
    os << std::setw(25) << std::left << "start_chrom_ix" << start_chrom_ix << '\n';
    os << std::setw(25) << std::left << "start_base" << start_base << '\n';
    os << std::setw(25) << std::left << "end_chrom_ix" << end_chrom_ix << '\n';
    os << std::setw(25) << std::left << "end_base" << end_base << '\n';
    os << std::setw(25) << std::left << "end_file_offset" << end_file_offset << '\n';
    os << std::setw(25) << std::left << "items_per_slot" << items_per_slot << '\n';
    os << std::setw(25) << std::left << "reserved" << reserved << '\n';
  }
  
  void header::pack(std::ostream& os) const {
    os.write((char*)&magic, sizeof magic);
    os.write((char*)&block_size, sizeof block_size);
    os.write((char*)&item_count, sizeof item_count);
    os.write((char*)&start_chrom_ix, sizeof start_chrom_ix);
    os.write((char*)&start_base, sizeof start_base);
    os.write((char*)&end_chrom_ix, sizeof end_chrom_ix);
    os.write((char*)&end_base, sizeof end_base);
    os.write((char*)&end_file_offset, sizeof end_file_offset);
    os.write((char*)&items_per_slot, sizeof items_per_slot);
    os.write((char*)&reserved, sizeof reserved);
  }
  
  void header::unpack(std::istream& os) {
    os.read((char*)&magic, sizeof magic);
    os.read((char*)&block_size, sizeof block_size);
    os.read((char*)&item_count, sizeof item_count);
    os.read((char*)&start_chrom_ix, sizeof start_chrom_ix);
    os.read((char*)&start_base, sizeof start_base);
    os.read((char*)&end_chrom_ix, sizeof end_chrom_ix);
    os.read((char*)&end_base, sizeof end_base);
    os.read((char*)&end_file_offset, sizeof end_file_offset);
    os.read((char*)&items_per_slot, sizeof items_per_slot);
    os.read((char*)&reserved, sizeof reserved);
  }
  
  
  void node_header::print(std::ostream &os) const {
    using std::setw; using std::left;
    os << setw(25) << left << "is_leaf" << std::boolalpha << (is_leaf == 1) << '\n';
    os << setw(25) << left << "reserved" << (int)reserved << '\n';
    os << setw(25) << left << "count" << count << '\n';
  }
  
  void node_header::pack(std::ostream &os) const {
    os.write((char*)&is_leaf, sizeof is_leaf);
    os.write((char*)&reserved, sizeof reserved);
    os.write((char*)&count, sizeof count);
  }
  
  void node_header::unpack(std::istream &is) {
    is.read((char*)&is_leaf, sizeof is_leaf);
    is.read((char*)&reserved, sizeof reserved);
    is.read((char*)&count, sizeof count);
  }
  
  void internal_node::print(std::ostream &os) const {
    using std::setw; using std::left;
    os << setw(25) << left << "start_chrom_ix" << start_chrom_ix << '\n';
    os << setw(25) << left << "start_base" << start_base << '\n';
    os << setw(25) << left << "end_chrom_ix" << end_chrom_ix << '\n';
    os << setw(25) << left << "end_base" << end_base << '\n';
    os << setw(25) << left << "data_offset" << data_offset << '\n';
  }
  
  void internal_node::pack(std::ostream &os) const {
    os.write((char*)&start_chrom_ix, sizeof start_chrom_ix);
    os.write((char*)&start_base, sizeof start_base);
    os.write((char*)&end_chrom_ix, sizeof end_chrom_ix);
    os.write((char*)&end_base, sizeof end_base);
    os.write((char*)&data_offset, sizeof data_offset);
  }
  
  void internal_node::unpack(std::istream &is) {
    is.read((char*)&start_chrom_ix, sizeof start_chrom_ix);
    is.read((char*)&start_base, sizeof start_base);
    is.read((char*)&end_chrom_ix, sizeof end_chrom_ix);
    is.read((char*)&end_base, sizeof end_base);
    is.read((char*)&data_offset, sizeof data_offset);
  }
  
  void leaf_node::print(std::ostream &os) const {
    using std::setw; using std::left;
    internal_node::print(os);
    os << setw(25) << left << "data_size" << data_size << '\n';
  }
  
  void leaf_node::pack(std::ostream &os) const {
    internal_node::pack(os);
    os.write((char*)&data_size, sizeof data_size);
  }
  
  void leaf_node::unpack(std::istream &is) {
    internal_node::unpack(is);
    is.read((char*)&data_size, sizeof data_size);
  }
  
}
