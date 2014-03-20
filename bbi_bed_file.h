#ifndef DPJ_BBI_BED_FILE_H_
#define DPJ_BBI_BED_FILE_H_

#include "bbi_file.h"

namespace bbi
{
  class bed_file : public file_base
  {
  public:
    bed_file(std::istream& is) : file_base(is)
    {
      is.seekg(0);
      if (main_header.magic == static_cast<unsigned>(bbi::file_type::bed))
      {
        file_type = file_type::bed;
        // TODO check for terminal magic number too.
      }
      else
        throw std::runtime_error("bed_file: magic number incorrect.");
    }    
  };
}

#endif /* DPJ_BBI_BED_FILE_H_ */
