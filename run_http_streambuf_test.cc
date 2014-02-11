#include <iostream>
#include <fstream>
#include <string>

#include "http_streambuf.h"



bool test_streambuf(std::streambuf* sb1, std::streambuf* sb2, bool verbose);
bool test_istream(std::istream& is1, std::istream& is2, bool verbose);


///////////////////////////////////////////////////////////////////////////
//
// Runs tests for http_streambuf.
//
//
int main(int argc, const char * argv[])
{
  std::string resource{"dict_of_vulgar_tongue_1811.txt"};
  
  try
  {
    std::string host = "localhost";
    std::string port = "80";
    std::string url = "/~dj/"+resource;
    
    unsigned buf_size = 512;
    
    {
      // Our streambuf, to be tested.
      //
      dpj::http::streambuf sbuf(host, port, url, /* debug session */ false, buf_size);

      // Reference streambuf from ifstream.
      //
      std::ifstream ifs(resource);
      if (!ifs.good())
        throw std::runtime_error("local file not good() in test http_streambuf.");
      
      bool all_pass = true;
      
      all_pass =  all_pass && test_streambuf(&sbuf, ifs.rdbuf(), false);
      std::cout << "streambuf all pass: " << std::boolalpha << all_pass << "\n\n";

    }
    
    
    
    {
      // Our streambuf, wrapped in std::istream, to be tested.
      //
      dpj::http::streambuf sbuf(host, port, url, /* debug session */ false, buf_size);
      std::istream ibs(&sbuf);
      
      // Reference istream.
      //
      std::ifstream ifs(resource);
      if (!ifs.good())
        throw std::runtime_error("local file not good() in test http_streambuf.");
      
      // tests via istream interface.
      //
      bool all_pass = true;
      all_pass = all_pass && test_istream(ibs, ifs, false);
      
      std::cout << "istream all pass: " << std::boolalpha << all_pass << "\n\n";
    }
    

  
  
  } catch (std::exception& e) {
    
    std::cout << "Exception: " << e.what() << std::endl;
    
  }
  
  
  return 0;
  
}
