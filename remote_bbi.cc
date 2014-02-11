#include <getopt.h>

#include <iostream>
#include <string>

#include "http_streambuf.h"

#include "bbi_file.h"

void usage() {
  std::cerr <<
  
  "Usage:\n"
  "\t-h, --host           required\n"
  "\t-r, --resource       required\n"
  "\t-c, --chrom-name     required\n"
  "\t-m, --min-coord      required\n"
  "\t-M, --max-coord      required\n\n"
  
  "\t-p, --port           default is 80\n"
  "\t-z, --zoom           default is 0, (main data)\n"
  "\t-d, --debug-session  prints the headers for the HTTP session\n\n"
  
  "\t--print-chroms       prints each chromosome, (name, id, size)\n"
  "\t--print-main         prints the main-headers\n"
  "\t--print-zoom         prints the zoom-headers\n"
  ;
}
int main(int argc, char * argv[])
{
  int print = 0;
  option longopts[]{
    {"host", required_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {"resource", required_argument, NULL, 'r'},
    {"zoom-level", required_argument, NULL, 'z'},
    {"chrom-name", required_argument, NULL, 'c'},
    {"print-chroms", no_argument, &print, 1},
    {"print-main", no_argument, &print, 2},
    {"print-zoom", no_argument, &print, 3},
    {"min-coord", required_argument, NULL, 'm'},
    {"max-coord", required_argument, NULL, 'M'},
    {"debug-session", no_argument, NULL, 'd'},
    {NULL, 0, NULL, 0} // This is a required sentinel.
  };
  bool print_chrom_ids = false;
  bool print_main_headers = false;
  bool print_zoom_headers = false;
  try {
    std::string host, resource, chrom_name;
    std::string port{"80"};
    uint32_t m = 0, M = 0, zoom_level = 0;
    bool debug_session = false;
    char c;
    while ((c = getopt_long_only(argc, argv, "h:p:r:z:m:M:d", longopts, NULL)) != -1)
    {
      switch (c)
      {
        case 'h': host.assign(optarg);
          break;
        case 'p': port.assign(optarg);
          break;
        case 'r': resource.assign(optarg);
          break;
        case 'c': chrom_name.assign(optarg);
          break;
        case 'z': zoom_level = std::atoi(optarg);
          break;
        case 'm': m = std::atoi(optarg);
          break;
        case 'M': M = std::atoi(optarg);
          break;
        case 'd':
          debug_session = true;
          break;
        case 0:
          switch (print)
        {
          case 1: print_chrom_ids = true;
            break;
          case 2: print_main_headers = true;
            break;
          case 3: print_zoom_headers = true;
            break;
          default:
            break;
        }
          break;
        case ':':
          usage();
        case '?':
          usage();
        default:
          exit(EXIT_FAILURE);
          break;
      }
    }
    
    if (host.empty() || resource.empty() || chrom_name.empty() || !(m < M))
    {
      std::cerr << "      host: " << host << '\n';
      std::cerr << "  resource: " << resource << '\n';
      std::cerr << "chrom-name: " << chrom_name << '\n';
      std::cerr << "         m: " << m << '\n';
      std::cerr << "         M: " << M << '\n';
      usage();
      exit(EXIT_FAILURE);
    }
    
    dpj::http::streambuf sbuf(host, port, resource, debug_session, 4096);
    
    std::istream is(&sbuf);
    bbi_file bbi(is);
    
    
    if (print_main_headers) {
      bbi.print_headers(std::cout);
      return 0;
    }
    
    if (print_chrom_ids) {
      bbi.chrom_tree.print(std::cout);
      return 0;
    }

    if (print_zoom_headers) {
      for (unsigned i = 0; i < bbi.z_hdrs.size(); ++i) {
        bbi.print_index_header(i, std::cout);
        std::cout << '\n';
      }
      return 0;
    }
    
    // Obtains any r-tree leaf nodes whose intervals contain our records in
    // the main data section, (zoom level 0).
    //
    auto chrom_id = bbi.chrom_tree.chrom_id(chrom_name);
    auto leaves = bbi.search_r_tree({chrom_id, m, M}, zoom_level);
    std::cout << "found " << leaves.size() << " r-tree leaf nodes for search\n";
    
    
    // Prints out any data blocks.
    //
    for (auto ln : leaves)
    {
      using namespace std::placeholders;
      if (zoom_level == 0)
      {
        auto ds = bbi.records_for_leaf<bed_data_record>(ln);
        for (auto const& d : ds) d.print(std::cout);
      }
      else
      {
        auto zs = bbi.records_for_leaf<zoom_data_record>(ln);
        for (auto& z : zs) z.print(std::cout);
      }
    }
    
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << '\n';
  }
  
  return 0;
}

