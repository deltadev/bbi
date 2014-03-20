#ifndef DPJ_BBI_FILE_H_
#define DPJ_BBI_FILE_H_

#include <type_traits>

#include <vector>
#include <fstream>

#include <zlib.h>

#include "main_header.h"
#include "zoom_header.h"

#include "r_tree.h"
#include "chromosome_tree.h"
#include "block_decompressor.h"
#include "bbi_index.h"


namespace bbi
{
  enum class file_type : unsigned int
  { wig = 0x888FFC26, bed = 0x8789F2EB };
  
  class file_base : public std::streambuf
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
    
    std::streambuf* fill_stream(r_tree::leaf_node ln)
    {
      buf.resize(ln.data_size);
      
      is_.seekg(ln.data_offset);
      is_.read((char*)buf.data(), buf.size());
      
      if (is_.gcount() != ln.data_size)
        throw std::runtime_error("file::inflate_records failed to read comp_sz bytes");
      
      if (main_header.uncompress_buf_size == 0)
      {
        setg((char*)buf.data(), (char*)buf.data(), (char*)buf.data() + buf.size());
      }
      else
      {
        auto p = decompressor.decompress(buf.data(), buf.data() + buf.size());
        setg((char*)p.first, (char*)p.first, (char*)p.second);
      }
      return this;
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

  };    
}




#endif /* DPJ_BBI_FILE_H_ */
