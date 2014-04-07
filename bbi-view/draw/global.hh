#ifndef DPJ_GLOBAL_HH_
#define DPJ_GLOBAL_HH_

#include <unordered_map>

#include "program.hh"
#include "buffer.hh"

namespace dpj {
  namespace gl {
    
    struct global
    {
      std::unordered_map<std::string, dpj::gl::program_t> programs;
      std::unordered_map<std::string, dpj::gl::buffer_t> buffers;
      ~global()
      {
        for (auto pair : programs)
          destroy(pair.second);
        for (auto pair : buffers)
          destroy(pair.second);
      }
      
      friend
      dpj::gl::program_t program(global& g, std::string name)
      {
        auto it = g.programs.find(name);
        if (it == end(g.programs))
        {
          dpj::gl::program_t p{glCreateProgram()};
          g.programs.emplace(name, p);
          return p;
        }
        return it->second;
      }
      
      friend
      dpj::gl::buffer_t buffer(global& g, std::string name)
      {
        auto it = g.buffers.find(name);
        if (it == end(g.buffers))
        {
          GLuint b;
          glGenBuffers(1, &b);
          g.buffers.emplace(name, b);
          return b;
        }

        return it->second;
      }

    } global;
  }
}



#endif /* DPJ_GLOBAL_HH_ */
