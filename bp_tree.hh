#ifndef DPJ_BP_TREE_HH_
#define DPJ_BP_TREE_HH_

#include <cstdint>
#include <ostream>
#include <vector>

namespace bp_tree 
{

  struct header 
  {
    uint32_t magic = 2026540177;
    uint32_t block_size = 256;
    uint32_t key_size = 0;
    uint32_t val_size = 0;
    uint64_t item_count = 0;
    uint64_t reserved = 0;

    header() { }
    header(std::streambuf* s) { unpack(*this, s); }

    //void pack(std::ostream& os) const;
    friend void print(header const& h, std::ostream& os)
    {
    os << std::setw(25) << std::left << "magic" << h.magic << '\n';
    os << std::setw(25) << std::left << "block_size" << h.block_size << '\n';
    os << std::setw(25) << std::left << "key_size" << h.key_size << '\n';
    os << std::setw(25) << std::left << "val_size" << h.val_size << '\n';
    os << std::setw(25) << std::left << "item_count" << h.item_count << '\n';
    os << std::setw(25) << std::left << "reserved" << h.reserved << '\n';
  }



    friend void unpack(header& h, std::streambuf* s)
    {
      s->sgetn((char*)&h.magic, sizeof h.magic);
      s->sgetn((char*)&h.block_size, sizeof h.block_size);
      s->sgetn((char*)&h.key_size, sizeof h.key_size);
      s->sgetn((char*)&h.val_size, sizeof h.val_size);
      s->sgetn((char*)&h.item_count, sizeof h.item_count);
      s->sgetn((char*)&h.reserved, sizeof h.reserved);      
    }
  };


  struct header_node 
  {
    uint8_t is_leaf;
    uint8_t reserved;
    uint16_t count;

    header_node() { }
    header_node(std::streambuf* s) { unpack(*this, s); }

    //void pack(std::ostream& os) const;
    friend void print(header_node const& h, std::ostream& os)
    {
    os << std::setw(25) << std::left << "is_leaf" << std::boolalpha << (h.is_leaf == 1) << '\n';
    os << std::setw(25) << std::left << "reserved" << (int)h.reserved << '\n';
    os << std::setw(25) << std::left << "count" << h.count << '\n';
  }


    friend void unpack(header_node& h, std::streambuf* s)
    {
      s->sgetn((char*)&h.is_leaf, sizeof h.is_leaf);
      s->sgetn((char*)&h.reserved, sizeof h.reserved);
      s->sgetn((char*)&h.count, sizeof h.count);
    }  
  };


  struct base_node
  {
    std::string key;

    base_node() { }
    base_node(std::streambuf* s) { unpack(*this, s); }

    friend void print(base_node const& n, std::ostream& os)
      { os << std::setw(25) << std::left << "key" << n.key << '\n'; }


    //void pack(std::ostream& os) const;

    friend void unpack(base_node& n, int key_size, std::streambuf* s)
    {
      std::vector<char> buff(key_size);
      s->sgetn(buff.data(), key_size);
      if (buff.back() != '\0')
        n.key.assign(buff.begin(), buff.end()); // non-null terminated
      else
        n.key = buff.data();                    // null terminated
    }
  };

  struct internal_node : base_node 
  {
    uint64_t child_offset;

    internal_node() { }
    internal_node(std::streambuf* s) { unpack(*this, s); }

    friend void print(internal_node const& n, std::ostream& os)
    {
      print(static_cast<base_node const&>(n), os);
      os << std::setw(25) << std::left << "child_offset" << n.child_offset << '\n';
    }


    //void pack(std::ostream& os) const;

    friend void unpack(internal_node& n, int key_size, std::streambuf* s)
    {
      unpack(static_cast<base_node&>(n), key_size, s);
      s->sgetn((char*)&n.child_offset, sizeof n.child_offset);
    }
  };

  struct leaf_node : base_node 
  {
    uint32_t chrom_id;
    uint32_t chrom_size;

    leaf_node() { }
    leaf_node(std::streambuf* s) { unpack(*this, s); }


    friend void print(leaf_node const& n, std::ostream& os)
    {
      print(static_cast<base_node const&>(n), os);
      os << std::setw(25) << std::left << "chrom_id" << n.chrom_id << '\n';
      os << std::setw(25) << std::left << "chrom_size" << n.chrom_size << '\n';
    }

    //void pack(std::ostream& os) const;

    friend void unpack(leaf_node& n, int key_size, std::streambuf* s)
    {
      unpack(static_cast<base_node&>(n), key_size, s);
      s->sgetn((char*)&n.chrom_id, sizeof n.chrom_id);
      s->sgetn((char*)&n.chrom_size, sizeof n.chrom_size);
    }

  };

}


#endif /* DPJ_BP_TREE_HH_ */
