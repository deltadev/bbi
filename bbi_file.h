#ifndef DPJ_BBI_FILE_H_
#define DPJ_BBI_FILE_H_

#include <type_traits>

#include <vector>
#include <fstream>

#include <zlib.h>
#include <sstream>

#include "main_header.h"
#include "zoom_header.h"

#include "r_tree.h"
#include "bp_tree.h"
#include "chromosome_tree.h"

#include "bed_record.h"
#include "wig_record.h"
#include "zoom_record.h"

#include "block_decompressor.h"

#include "bbi_index.h"

// kent/src/inc/sig.h
//
// #define bigWigSig 0x888FFC26
// /* Signature for a big wig file. */

// #define bigBedSig 0x8789F2EB
// /* Signature for a big bed file. */

namespace bbi
{
  enum class file_type : unsigned int
  {
    wig = 0x888FFC26, bed = 0x8789F2EB
  };
  
  
  
  // Xcode is to blame for this indentation. Not me.
  
  class file_base
  {
  public:
    
    file_type type;
    
    std::vector<zoom_header> zoom_headers;
    chromosome_tree          chrom_tree;
    
    file_base(std::istream& is);
    
    // For info on headers including zoom headers when available.
    //
    void print_headers(std::ostream& os);
    
    void print_index_header(unsigned index, std::ostream& os);
    
    index index(unsigned level);
    
    template<typename T> std::vector<T> extract(r_tree::leaf_node ln)
    {
      fill_buf(ln);
      
      std::istringstream iss;
      
      if (main_header.uncompress_buf_size == 0)
      {
        iss.str({begin(buf), end(buf)});
      }
      else
      {
        auto pair = decompressor.decompress(buf.data(), buf.data() + buf.size());
        iss.str({pair.first, pair.second});
      }
      
      std::vector<T> data;
      while (iss)
      {
        T t{iss};
        if (t.chrom_start == t.chrom_end)
          break;
        data.push_back(t);
      }
      
      return data;
    }
    
  protected:
    main_header main_header;
    std::istream& is_;
        
    std::vector<uint8_t> buf;
    block_decompressor decompressor;
    
    // Called by constructor.
    //
    void init_chrom_tree();
    void init_zoom_headers();
    
    
    void fill_buf(r_tree::leaf_node ln)
    {
      buf.resize(ln.data_size);
      
      is_.seekg(ln.data_offset);
      is_.read((char*)buf.data(), buf.size());
      
      if (is_.gcount() != ln.data_size)
        throw std::runtime_error("file::inflate_records failed to read comp_sz bytes");
    }
  };
}




#endif /* DPJ_BBI_FILE_H_ */
