#include <iostream>
#include <string>

#include "http_streambuf.h"

#include "bbi_bed_file.h"
#include "bbi_wig_file.h"

struct opts
{
  
  std::string resource;
  std::string contig;
  
  std::string host = "localhost";
  std::string port = "http";
  
  bool print_chrom_ids    = false;
  bool print_main_headers = false;
  bool print_zoom_headers = false;
  
  uint32_t m          = 0;
  uint32_t M          = 0;
  uint32_t zoom_level = 0;
  
  bool debug_session  = false;
  
};

opts parse_options(int argc, char * argv[]);


int main(int argc, char * argv[])
{
  try {
    
    
    opts opts = parse_options(argc, argv);
    dpj::http::streambuf sbuf(opts.host, opts.port, opts.resource, opts.debug_session);
    
    std::istream is(&sbuf);
    bbi::file_base bbi{is};
    
    
    if (opts.print_main_headers)
    {
      bbi.print_headers(std::cout);
      return 0;
    }
    
    if (opts.print_chrom_ids)
    {
      bbi.chrom_tree.print(std::cout);
      return 0;
    }
    
    if (opts.print_zoom_headers)
    {
      for (unsigned i = 0; i < bbi.zoom_headers.size(); ++i)
      {
        bbi.print_index_header(i, std::cout);
        std::cout << '\n';
      }
      return 0;
    }
    
    auto chrom_id = bbi.chrom_tree.chrom_id(opts.contig);
    bbi::record r{chrom_id, opts.m, opts.M};
    
    auto index = bbi.index(opts.zoom_level);
    auto leaves = index.search(r);
    
    if (opts.zoom_level > 0)
    {
      for (auto const& ln : leaves)
      {
        auto zs = bbi.extract<bbi::zoom::record>(ln);
        for (auto& z : zs) z.print(std::cout);
      }
    }
    
    if (bbi.type == bbi::file_type::wig)
    {
      is.seekg(0);
      bbi::wig_file f(is);
      for (auto const& ln : leaves)
      {
        auto wh = f.prepare_records(ln);
        switch (wh.type)
        {
          case static_cast<int>(bbi::wig_file::record_type::bed_graph):
          {
            auto ds = f.extract<bbi::wig::bed_graph>(wh.item_count);
            for (auto const& d : ds) d.print(std::cout);

          }
            break;
          case static_cast<int>(bbi::wig_file::record_type::var_step):
          {
            auto ds = f.extract<bbi::wig::var_step>(wh.item_count);
            for (auto const& d : ds) d.print(std::cout);
          }
            break;
          case static_cast<int>(bbi::wig_file::record_type::fixed_step):
          {
            auto ds = f.extract<bbi::wig::fixed_step>(wh.item_count);
            for (auto const& d : ds) d.print(std::cout);
          }
            break;
          default:
            break;
        }
        
      }
    }
    else if (bbi.type == bbi::file_type::bed)
    {
      is.seekg(0);
      bbi::bed_file f(is);
      for (auto const& ln : leaves)
      {
        auto ds = f.extract<bbi::bed::record>(ln);
        for (auto const& d : ds)
          d.print(std::cout);
      }
    }
    else
      throw std::runtime_error("remote_bbi: bad file type.");
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  
  return 0;
  
}



