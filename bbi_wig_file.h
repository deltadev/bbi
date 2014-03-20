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
      is.seekg(0);
      if (main_header.magic == static_cast<unsigned>(file_type::wig))
      {
        type = file_type::wig;
        // TODO check for terminal magic number too.
      }
      else
        throw std::runtime_error("wig_file: magic number incorrect.");
    }
  };
}
#endif /* DPJ_BBI_WIG_FILE_H_ */
