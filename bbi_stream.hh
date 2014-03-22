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
#include "contig_index.hh"
#include "block_decompressor.h"

#include "bbi_index.hh"

#include "http_streambuf.h"
#include "contig_index.hh"

namespace bbi
{
  
  class stream : public std::streambuf
  {
  public:
    enum type { wig = 0x888FFC26, bed = 0x8789F2EB } type;
    
    stream(std::string resource)
    {
      if (resource.substr(0, 7) == "http://")
      {
        resource = resource.substr(7);
        auto pos = resource.find_first_of('/');
        auto host = resource.substr(0, pos);
        resource = resource.substr(pos);
        input_stream.reset(new dpj::http::streambuf(host, "http", resource, false /* debug */));
      }
      else
      {
        auto fb = new std::filebuf{};
        fb->open(resource, std::ios_base::in);
        if (!fb->is_open())
          throw std::runtime_error("remote_bbi: could not open file: " + resource);
        input_stream.reset(fb);
      }
      
      seekpos(0);

      std::istream is{input_stream.get()}; // temp sol
      main_header.unpack(is);
      
      is.seekg(main_header.total_summary_offset);
      total_summary_header.unpack(is);

      bool is_bed = main_header.magic == static_cast<unsigned>(type::bed);
      type = is_bed ? type::bed : type::wig;
      
      is.seekg(main_header.full_data_offset);
      num_records = 0;
      is.read((char*)&num_records, 8);
      
      is.seekg(main_header.byte_size);
      for (int i = 0; i < main_header.zoom_levels; ++i)
      {
        zoom_header z_h{0};
        z_h.unpack(is);
        zoom_headers.push_back(z_h);
      }
    }
    
    friend stream& seek(stream& s, r_tree::leaf_node ln)
    {
      s.buf.resize(ln.data_size);
      
      std::fill(begin(s.buf), end(s.buf), 0);
     
      std::istream is{s.input_stream.get()}; // temp solution
      
      is.seekg(ln.data_offset);
      is.read((char*)s.buf.data(), s.buf.size());
      
      if (is.gcount() != ln.data_size)
        throw std::runtime_error("file::inflate_records failed to read comp_sz bytes");
      
      if (s.main_header.uncompress_buf_size == 0)
      {
        s.setg((char*)s.buf.data(), (char*)s.buf.data(), (char*)s.buf.data() + s.buf.size());
      }
      else
      {
        auto p = s.decompressor.decompress(s.buf.data(), s.buf.data() + s.buf.size());
        s.setg((char*)p.first, (char*)p.first, (char*)p.second);
      }
      
      return s;
    }
    
    
    friend index index(stream& s, int level)
    {
      if (level > s.zoom_headers.size())
      {
        level = static_cast<int>(s.zoom_headers.size());
        std::cerr << "Warning: zoom level requested was greater than available.\n";
      }
      
      std::istream is{s.input_stream.get()}; // temp solution

      if (level == 0)
        is.seekg(s.main_header.full_index_offset);
      else
        is.seekg(s.zoom_headers[level - 1].index_offset);
      
      return bbi::index{s.input_stream.get()};
    }
    
    friend contig_index contigs(stream& s)
    {
      std::istream is{s.input_stream.get()}; // temp solution
      
      is.seekg(s.main_header.bp_tree_offset);
      contig_index ctigs{is};
            
      return ctigs;;
    }
    
 
    friend void print_header(stream& s, std::ostream& os)
    {
      os << "\n**** main_header ****\n\n";
      s.main_header.print(os);
    }
    friend void print_summary(stream& s, std::ostream& os)
    {
      os << "\n**** total_summary_header ****\n\n";
      s.total_summary_header.print(os);
    }
    friend void print_num_records(stream& s, std::ostream& os)
    {
      os << "num records: " << s.num_records << '\n';
    }
    

    std::vector<zoom_header> zoom_headers;

  private:
    std::unique_ptr<std::streambuf> input_stream;

    main_header           main_header;
    total_summary_header  total_summary_header;
    std::size_t           num_records;
    

    
    std::vector<uint8_t> buf;
    block_decompressor decompressor;

  };
  
  
}


#endif /* DPJ_BBI_STREAM_HH_ */
