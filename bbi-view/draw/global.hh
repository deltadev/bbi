#ifndef DPJ_GLOBAL_HH_
#define DPJ_GLOBAL_HH_

#include <unordered_map>

#include "program.hh"

namespace dpj {
  namespace gl {
    
    struct global
    {
      std::unordered_map<std::string, dpj::gl::program_t> programs;
      ~global() { for (auto & pair : programs) destroy(pair.second); }
      
      friend
      dpj::gl::program_t program(global& g, std::string name)
      {
        dpj::gl::program_t p{glCreateProgram()};
        g.programs.emplace(name, p);
        return p;
      }
    } global;
    
    
  } 
  
  
}



#endif /* DPJ_GLOBAL_HH_ */
