#ifndef DPJ_SHADER_HH_
#define DPJ_SHADER_HH_

#include <OpenGL/gl3.h>

#include <iostream>

#include <vector>
#include <array>
#include <string>

#include "util.hh"

namespace dpj
{
  using std::begin; using std::end;
  namespace gl
  {

    // This is, basically, a typed integer.
    //
    class shader_t
    {
    public:
      GLuint id;
      
      friend
      shader_t compile(shader_t s)
      {
        glCompileShader(s.id);
        
        GLint status;
        glGetShaderiv(s.id, GL_COMPILE_STATUS, &status);
        if (status == 0)
          throw std::runtime_error("compile(shader_t): status == 0");
        
        return s;
      }
      
      friend
      shader_t log(shader_t s, std::ostream& os)
      {
        std::vector<GLchar> log;
        GLint length = 0;
        glGetShaderiv(s.id, GL_INFO_LOG_LENGTH, &length);
        
        if (length > 0)
        {
          log.resize(length);
          glGetShaderInfoLog(s.id, length, &length, log.data());
          log.resize(length);
        }
        os << "shader info log: " << std::string{begin(log), end(log)} << '\n';
        return s;
      }
    };
    
    shader_t create_shader(GLenum type, std::vector<char> const& source_data)
    {
      shader_t s;
      s.id = glCreateShader(type);
      if (s.id == 0)
        throw std::runtime_error("create_shader: failed to create shader");
      
      GLint size = static_cast<GLint>(source_data.size());
      const char* ptr = source_data.data();
      glShaderSource(s.id, 1, &ptr, &size); // &ptr stuff is for arrays of c_str
      
      return s;
    }
  }
}
#endif /* DPJ_SHADER_HH_ */
