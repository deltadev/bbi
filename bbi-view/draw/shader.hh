#ifndef DPJ_SHADER_HH_
#define DPJ_SHADER_HH_

#include <OpenGL/gl3.h>

#include <iostream>

#include <vector>
#include <string>

#include "util.hh"

namespace dpj
{
  using std::begin; using std::end;
  namespace gl
  {
    class shader_t
    {

    public:
      GLuint id;
      
      shader_t(GLenum type, std::vector<char> source_data)
      {
        id = glCreateShader(type);
        GLint size = static_cast<GLint>(source_data.size());
        const char* ptr = source_data.data();
        glShaderSource(id, 1, &ptr, &size); // &ptr stuff is for arrays of c_str
      }
      
      friend void compile(shader_t s)
      {
        glCompileShader(s.id);
        
        std::cerr << log(s) << '\n';
        
        GLint status;
        glGetShaderiv(s.id, GL_COMPILE_STATUS, &status);
        if (status == 0)
          throw std::runtime_error("compile(shader_t): status == 0");

      }
      
      friend std::string log(shader_t s)
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
        return {begin(log), end(log)};
      }
    };
  }
}
#endif /* DPJ_SHADER_HH_ */
