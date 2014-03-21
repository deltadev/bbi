#ifndef DPJ_BBI_STREAM_HH_
#define DPJ_BBI_STREAM_HH_

#include <type_traits>

#include <vector>
#include <fstream>

#include <zlib.h>

#include "main_header.h"
#include "zoom_header.h"
#include "total_summary_header.h"

#include "r_tree.h"
#include "chromosome_tree.h"
#include "block_decompressor.h"
#include "bbi_index.h"


namespace bbi
{
  
  class stream : public std::streambuf
  {
  public:
    enum class type : unsigned int
    { wig = 0x888FFC26, bed = 0x8789F2EB };

    type type;
    
    std::vector<zoom_header> zoom_headers;
    chromosome_tree          chrom_tree;
    
    file_base(std::istream& is);
    
    // For info on headers including zoom headers when available.
    //
    void print_headers(std::ostream& os);
        
    index index(unsigned level);
    
    std::streambuf* fill_stream(r_tree::leaf_node ln);
    
  protected:
    main_header           main_header;
    total_summary_header  ts_header;
    std::size_t           num_records;
    
    std::istream& is_;
        
    std::vector<uint8_t> buf;
    block_decompressor decompressor;
    
    // Called by constructor.
    //
    void init_chrom_tree();
    void init_zoom_headers();
    void init_total_summary_header();
    void init_num_records();

    friend
    contig_index_type contig_index(stream s)
      {
      }



  };


}


#endif /* DPJ_BBI_STREAM_HH_ */
