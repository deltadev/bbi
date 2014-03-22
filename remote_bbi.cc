#include <iostream>
#include <fstream>
#include <string>

#include "http_streambuf.h"

#include "zoom_record.h"
#include "wig_record.h"
#include "bed_record.h"

#include "contig_index.hh"
#include "bbi_stream.hh"


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
    
    bbi::stream stream(opts.resource);
    
    auto ctigs = contigs(stream);
    
    if (opts.print_main_headers)
    {
      print_header(stream, std::cout);
      print_summary(stream, std::cout);
      print_num_records(stream, std::cout);
      return 0;
    }
    
    if (opts.print_chrom_ids)
    {
      std::cout << ctigs << '\n';
      return 0;
    }
    
    if (opts.print_zoom_headers)
    {
      for (unsigned i = 1; i <= stream.zoom_headers.size(); ++i)
        std::cout << index(stream, i) << '\n';
      return 0;
    }
    
    auto ctig_id = contig_id(ctigs, opts.contig);
    
    auto data_idx = index(stream, opts.zoom_level);
    auto leaves = search(data_idx, {static_cast<uint32_t>(ctig_id), opts.m, opts.M});
    
    if (opts.zoom_level > 0)
    {
      // If we are on a zoom level then everything works the same for either file type.
      //
      for (auto const& ln : leaves)
      {
        std::istream is{&seek(stream, ln)}; // temp sol.
        bbi::zoom::record zr;
        while(is >> zr)
        {
          if (zr.chrom_start == zr.chrom_end)
            break;

          std::cout << zr;
        }
      }
    }
    else
    {
      ////
      // Deals with a bigwig data request.
      //
      if (stream.type == bbi::stream::type::wig)
      {
        using namespace bbi::wig;
        
        for (auto const& ln : leaves)
        {
          std::istream is{&seek(stream, ln)}; // temp sol
          
          bbi::wig::header wh;
          is >> wh;
          
          auto const t = wh.record_type();
          
          if (t == record_type::bed_graph)
          {
            auto rs = extract<bed_graph_record>(is, wh.item_count);
            std::cout << "extracted " << rs.size() << " bed_graph_record types.\n";
          }
          else if (t == record_type::var_step)
          {
            auto rs = extract<var_step_record>(is, wh.item_count);
            std::cout << "extracted " << rs.size() << " var step_record types.\n";
          }
          else if (t == record_type::fixed_step)
          {
            auto rs = extract<fixed_step_record>(is, wh.item_count);
            std::cout << "extracted " << rs.size() << " fixed_step_record types.\n";
          }
          else
            throw std::runtime_error("remote_bbi: bad wig record type.");
        }
      }
      ////
      // Deals with a bigbed data request.
      //
      else if (stream.type == bbi::stream::type::bed)
      {
        for (auto const& ln : leaves)
        {
          std::istream is{&seek(stream, ln)}; // temp sol.
          bbi::bed::record bdr;
          while (is >> bdr)
          {
            if (bdr.chrom_start == bdr.chrom_end)
              break;
            std::cout << bdr << '\n';
          }
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



