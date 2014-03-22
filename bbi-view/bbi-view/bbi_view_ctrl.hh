#ifndef DPJ_BBI_VIEW_CTRL_HH_
#define DPJ_BBI_VIEW_CTRL_HH_

#include "GLRenderer.hh"
#include "GLProgram.hh"
#include "GLDrawable.hh"
#include "GLVertexArrayObject.hh"

#include "drawables.hh"
#include "bbi_file.h"

class bbi_view_ctrl
{
  bbi::stream stream;
  
public:
  bbi_view_ctrl(std::string resource)
  {
    // Set up the bbi_stream.
  }

  template<typename T> std::vector<T>
  filter_stream(bbi::record bbi)
  {
    std::vector<T> rs;

    return rs;
  }
  
  std::vector<std::tuple<std::string, int, int>> table_data()
  {
    std::vector<std::tuple<std::string, int, int>> tuples;
    
    auto ctigs = contigs(stream);
    
    for (auto const& p : ctigs)
      tuples.emplace_back(p.first, p.second.chrom_id, p.second.chrom_size);
    std::sort(begin(tuples), end(tuples));
    return tuples;
  }

};

#endif /* DPJ_BBI_VIEW_CTRL_HH_ */
