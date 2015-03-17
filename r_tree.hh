#ifndef DPJ_R_TREE_HH_
#define DPJ_R_TREE_HH_

#include <cstdint>

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
struct header {
    static const uint32_t byte_size = 48;
    uint32_t magic = 610839776;
    uint32_t block_size = 256;
    uint64_t item_count;
    uint32_t start_chrom_ix;
    uint32_t start_base;
    uint32_t end_chrom_ix;
    uint32_t end_base;
    uint64_t end_file_offset;
    uint32_t items_per_slot = 512;
    uint32_t reserved;

    header() {}
    header(std::streambuf* s) { unpack(*this, s); }

    // void pack(std::ostream& os) const;

    friend void print(header const& h, std::ostream& os) {
        os << std::setw(25) << std::left << "magic" << h.magic << '\n';
        os << std::setw(25) << std::left << "block_size" << h.block_size << '\n';
        os << std::setw(25) << std::left << "item_count" << h.item_count << '\n';
        os << std::setw(25) << std::left << "start_chrom_ix" << h.start_chrom_ix << '\n';
        os << std::setw(25) << std::left << "start_base" << h.start_base << '\n';
        os << std::setw(25) << std::left << "end_chrom_ix" << h.end_chrom_ix << '\n';
        os << std::setw(25) << std::left << "end_base" << h.end_base << '\n';
        os << std::setw(25) << std::left << "end_file_offset" << h.end_file_offset << '\n';
        os << std::setw(25) << std::left << "items_per_slot" << h.items_per_slot << '\n';
        os << std::setw(25) << std::left << "reserved" << h.reserved << '\n';
    }

    friend void unpack(header& h, std::streambuf* s) {
        s->sgetn((char*)&h.magic, sizeof h.magic);
        s->sgetn((char*)&h.block_size, sizeof h.block_size);
        s->sgetn((char*)&h.item_count, sizeof h.item_count);
        s->sgetn((char*)&h.start_chrom_ix, sizeof h.start_chrom_ix);
        s->sgetn((char*)&h.start_base, sizeof h.start_base);
        s->sgetn((char*)&h.end_chrom_ix, sizeof h.end_chrom_ix);
        s->sgetn((char*)&h.end_base, sizeof h.end_base);
        s->sgetn((char*)&h.end_file_offset, sizeof h.end_file_offset);
        s->sgetn((char*)&h.items_per_slot, sizeof h.items_per_slot);
        s->sgetn((char*)&h.reserved, sizeof h.reserved);
    }
};

struct node_header {
    static const uint32_t byte_size = 4;
    uint8_t is_leaf;
    uint8_t reserved;
    uint16_t count;

    node_header() {}
    node_header(std::streambuf* s) { unpack(*this, s); }
    // void pack(std::ostream& os) const;
    friend void print(node_header const& h, std::ostream& os) {
        using std::setw;
        using std::left;
        os << setw(25) << left << "is_leaf" << std::boolalpha << (h.is_leaf == 1) << '\n';
        os << setw(25) << left << "reserved" << (int)h.reserved << '\n';
        os << setw(25) << left << "count" << h.count << '\n';
    }

    friend void unpack(node_header& h, std::streambuf* s) {
        s->sgetn((char*)&h.is_leaf, sizeof h.is_leaf);
        s->sgetn((char*)&h.reserved, sizeof h.reserved);
        s->sgetn((char*)&h.count, sizeof h.count);
    }
};

struct internal_node {
    static const uint32_t byte_size = 24;

    uint32_t start_chrom_ix;
    uint32_t start_base;
    uint32_t end_chrom_ix;
    uint32_t end_base;
    uint64_t data_offset;

    internal_node() {}
    internal_node(std::streambuf* s) { unpack(*this, s); }
    // void pack(std::ostream& os) const;

    friend void print(internal_node const& n, std::ostream& os) {
        using std::setw;
        using std::left;
        os << setw(25) << left << "start_chrom_ix" << n.start_chrom_ix << '\n';
        os << setw(25) << left << "start_base" << n.start_base << '\n';
        os << setw(25) << left << "end_chrom_ix" << n.end_chrom_ix << '\n';
        os << setw(25) << left << "end_base" << n.end_base << '\n';
        os << setw(25) << left << "data_offset" << n.data_offset << '\n';
    }

    friend void unpack(internal_node& n, std::streambuf* s) {
        s->sgetn((char*)&n.start_chrom_ix, sizeof n.start_chrom_ix);
        s->sgetn((char*)&n.start_base, sizeof n.start_base);
        s->sgetn((char*)&n.end_chrom_ix, sizeof n.end_chrom_ix);
        s->sgetn((char*)&n.end_base, sizeof n.end_base);
        s->sgetn((char*)&n.data_offset, sizeof n.data_offset);
    }
};

struct leaf_node : internal_node {
    static const uint32_t byte_size = 32;
    uint64_t data_size;

    leaf_node() {}
    leaf_node(std::streambuf* s) { unpack(*this, s); }
    // void pack(std::ostream& os) const;

    friend void print(leaf_node const& n, std::ostream& os) {
        print(static_cast<internal_node const&>(n), os);
        os << std::setw(25) << std::left << "data_size" << n.data_size << '\n';
    }

    friend void unpack(leaf_node& n, std::streambuf* s) {
        unpack(static_cast<internal_node&>(n), s);
        s->sgetn((char*)&n.data_size, sizeof n.data_size);
    }
};
}

#endif /* DPJ_R_TREE_HH_ */
