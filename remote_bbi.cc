#include <iostream>
#include <fstream>
#include <string>

#include "http_streambuf.h"

#include "bbi_bed_file.h"
#include "bbi_wig_file.h"
#include "zoom_record.h"
#include "wig_record.h"
#include "bed_record.h"

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
    
    std::unique_ptr<std::streambuf> sb;
    if (opts.resource.substr(0, 7) == "http://")
    {
      opts.resource = opts.resource.substr(6);
      sb.reset(new dpj::http::streambuf(opts.host, opts.port, opts.resource, opts.debug_session));
    }
    else
    {
      auto fb = new std::filebuf();
      fb->open(opts.resource, std::ios_base::in);
      if (!fb->is_open())
        throw std::runtime_error("remote_bbi: could not open file: " + opts.resource);
      sb.reset(fb);
    }
    
    std::istream is(sb.get());
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
    
    auto index = bbi.index(opts.zoom_level);
    auto leaves = index.search({chrom_id, opts.m, opts.M});
    
    if (opts.zoom_level > 0)
    {
      // If we are on a zoom level then everything works the same for either file type.
      //
      for (auto const& ln : leaves)
      {
        std::istream is{bbi.fill_stream(ln)};
        bbi::zoom::record zr;
        while(is >> zr)
          std::cout << zr;
      }
    }
    else
    {
      // Otherwise we must specialize for wig and bed types.
      //
      if (bbi.type == bbi::file_type::wig)
      {
        using namespace bbi::wig;
        using record_type = bbi::wig_file::record_type;
        bbi::wig_file f(is);
        
        for (auto const& ln : leaves)
        {
          std::istream record_stream{f.fill_stream(ln)};
          
          bbi::wig::header wh;
          record_stream >> wh;
          unsigned count = 0;
          auto type = static_cast<record_type>(wh.type);
          
          if (false)
          {
          if (type == record_type::bed_graph)
            for (bed_graph_record bgr; record_stream >> bgr && count < wh.item_count; ++count)
              std::cout << bgr << '\n';
          
          else if (type == record_type::var_step)
            for (var_step_record vsr; record_stream >> vsr && count < wh.item_count; ++count)
              std::cout << vsr.chrom_start << ' ' << vsr.val << '\n';
          
          else if (type == record_type::fixed_step)
            for (fixed_step_record fsr; record_stream >> fsr && count < wh.item_count; ++count)
              std::cout << fsr.val << '\n';
          
          else
            throw std::runtime_error("remote_bbi: bad wig record type.");
          }
          else
            if (type == record_type::bed_graph)
            {
              auto rs = extract<bed_graph_record>(record_stream, wh.item_count);
              std::cerr << "extracted " << rs.size() << " bed_graph_record types.\n";
            }
            else if (type == record_type::var_step)
            {
              auto rs = extract<var_step_record>(record_stream, wh.item_count);
              std::cerr << "extracted " << rs.size() << " var step_record types.\n";
            }
            else if (type == record_type::fixed_step)
            {
              auto rs = extract<fixed_step_record>(record_stream, wh.item_count);
              std::cerr << "extracted " << rs.size() << " fixed_step_record types.\n";
            }
        }
      }
      else if (bbi.type == bbi::file_type::bed)
      {
        bbi::bed_file f(is);
        for (auto const& ln : leaves)
        {
          std::istream is{f.fill_stream(ln)};
          bbi::bed::record bdr;
          while (is >> bdr)
            std::cout << bdr << '\n';
        }
      }
      else
        throw std::runtime_error("remote_bbi: bad file type.");
    }
  }
  catch (std::exception& e)
  { std::cerr << "Exception: " << e.what() << std::endl; }
  
  return 0;
  
}



