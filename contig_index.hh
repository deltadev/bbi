#ifndef DPJ_CONTIG_INDEX_HH_
#define DPJ_CONTIG_INDEX_HH_

#include <cstdint>

#include <string>
#include <iostream>

#include <map>

#include "bp_tree.hh"

class contig_index {
  public:
    using hash_map = std::map<std::string, bp_tree::leaf_node>;
    using iterator = hash_map::const_iterator;

    contig_index() {}
    contig_index(std::streambuf* s) : header{s} {
        bp_tree::header_node n_hdr{s};
        recursive_fill_map(n_hdr, s);
    }

    friend void print_header(contig_index const& index, std::ostream& os) {
        print(index.header, os);
    }

    friend int contig_id(contig_index const& index, std::string key) {
        return index.id_hash.at(key).chrom_id;
    }

    friend int contig_size(contig_index const& index, std::string key) {
        return index.id_hash.at(key).chrom_size;
    }

    friend iterator begin(contig_index const& index) { return std::begin(index.id_hash); }

    friend iterator end(contig_index const& index) { return std::end(index.id_hash); }

    friend std::ostream& operator<<(std::ostream& os, contig_index const& index) {
        for (auto const& p : index.id_hash)
            os << "(" << p.first << ", " << p.second.chrom_id << ", " << p.second.chrom_size
               << ")\n";
        return os;
    }

  private:
    bp_tree::header header;
    std::map<std::string, bp_tree::leaf_node> id_hash;

    // TODO: transform this into non-recursive.
    //
    void recursive_fill_map(bp_tree::header_node n_h, std::streambuf* s) {
        if (n_h.is_leaf == false) {
            // Drops down a level for each node header pointed to by this
            // internal_node. Probably never happens since the number of chromosomes is small.
            //
            for (int i = 0; i < n_h.count; ++i) {
                bp_tree::internal_node in_node;
                unpack(in_node, header.key_size, s);

                // This is the next internal_node position, out sibling, so save pos for next
                // time through the loop.
                //
                std::streamsize curr_off = s->pubseekoff(0, std::ios_base::cur);

                // Now go down a level.
                //
                s->pubseekpos(in_node.child_offset);
                bp_tree::header_node n_h_n;
                unpack(n_h_n, s);
                recursive_fill_map(n_h_n, s);

                // We're done below so seek back to our sibling's position.
                s->pubseekpos(curr_off);
            }
        } else {
            // We're on the leaf level so we can just fill up the hash map.
            //
            for (int i = 0; i < n_h.count; ++i) {
                bp_tree::leaf_node ln;
                unpack(ln, header.key_size, s);
                id_hash[ln.key] = ln;
            }
        }
    }
};

#endif /* DPJ_CONTIG_INDEX_HH_ */
