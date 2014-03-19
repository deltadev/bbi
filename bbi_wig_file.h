#ifndef DPJ_BBI_WIG_FILE_H_
#define DPJ_BBI_WIG_FILE_H_

#include "bbi_file.h"

namespace bbi
{
  class wig_file : public file_base
  {
    
  public:
    
    enum class record_type
    { bed_graph = 1, var_step = 2, fixed_step = 3};
    
    wig_file(std::istream& is) : file_base(is)
    {
      if (main_header.magic == static_cast<unsigned>(file_type::wig))
      {
        type = file_type::wig;
        // TODO check for terminal magic number too.
      }
      else
        throw std::runtime_error("wig_file: magic number incorrect.");
    }

  
    bbi::wig::header prepare_records(r_tree::leaf_node ln)
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
      return iss;
    }            
  
    template<typename T>
    std::vector<T> extract(unsigned count)
    {
      std::vector<T> data(count);
      std::generate(begin(data), end(data), [&]() -> T { return is_; });
      return data;
    }
  };
}
#endif /* DPJ_BBI_WIG_FILE_H_ */