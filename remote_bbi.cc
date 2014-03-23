#include <iostream>
#include <fstream>
#include <string>

#include "http_streambuf.h"

#include "zoom_record.hh"
#include "wig_record.hh"
#include "bed_record.hh"

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
  try
  {
    opts opts = parse_options(argc, argv);
    
    bbi::stream stream(opts.resource);
    
    if (opts.print_main_headers)
    {
      print_header(stream, std::cout);
      print_summary(stream, std::cout);
      print_num_records(stream, std::cout);
      return 0;
    }

    auto ctigs = ctig_index(stream);
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
    
    unsigned ctig_id = contig_id(ctigs, opts.contig);
    
    auto idx = index(stream, opts.zoom_level);

    bbi::record query{ctig_id, opts.m, opts.M};
    
    if (opts.zoom_level > 0)
    {
      for (auto const& l: search(idx, query))
      {
        seek(stream, l);
        for (auto const& z : extract<bbi::zoom::record>(query, stream))
        {
          print(z, std::cout);
          endl(std::cout);
        }
      }
    }
    else
    {
      // bigwig
      //
      // TODO: at the moment this dumps the whole leaf node. Seperate logic is needed for
      //       each wig record type in order to only retrieve overlapping records.
      //
      if (stream.type == bbi::stream::type::wig)
      {
        using namespace bbi::wig;
        
        for (auto const& l : search(idx, query))
        {
          seek(stream, l);

          bbi::wig::header wh{&stream};
          
          if (wh.record_type() == record_type::bed_graph)
            for (auto const& r : extract<bed_graph_record>(stream, wh.item_count))
              println(r, std::cout);

          else if (wh.record_type() == record_type::var_step)
            for (auto const& r: extract<var_step_record>(stream, wh.item_count))
              println(r, std::cout);

          else if (wh.record_type() == record_type::fixed_step)
            for (auto const& r : extract<fixed_step_record>(stream, wh.item_count))
              println(r, std::cout);

          else
            throw std::runtime_error("remote_bbi: bad wig record type.");
        }
      }
      // bigbed
      //
      else if (stream.type == bbi::stream::type::bed)
      {
        for (auto const& ln : search(idx, query))
        {
          seek(stream, ln);
          for (auto const& b: extract<bbi::bed::record>(query, stream))
          {
            print(b, std::cout);
            endl(std::cout);
          }
        }
      }
      else
        throw std::runtime_error("remote_bbi: bad file type.");
    }
  }
  catch (std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; }
  
  return 0;
  
}



