#ifndef DPJ_PROGRAM_HH_
#define DPJ_PROGRAM_HH_

#include <OpenGL/gl3.h>

#include <vector>
#include <string>

#include "shader.hh"

namespace dpj
{
  namespace gl
  {
    
    class program_t
    {
      GLuint id;

    public:
      program_t() : id{glCreateProgram()} { }
      program_t(program_t&& p) noexcept = default;
      program_t(program_t const& p) : id{p.id} { }
      ~program_t()
      {
        if (id == 0)
          throw std::runtime_error("~program_t: id == 0");
        else
        {
          GLsizei shaderCount;
          
          glGetProgramiv(id, GL_ATTACHED_SHADERS, &shaderCount);
          
          std::vector<GLuint> shaders(shaderCount);
          glGetAttachedShaders(id, shaderCount, &shaderCount, shaders.data());
          
          for (GLuint shader : shaders)
            glDeleteShader(shader);
          
          glDeleteProgram(id);
          glUseProgram(0);
        }
      }
      
      friend void check_prog(program_t const & p, GLenum check_type)
      {
        GLint llength, status;
        glGetProgramiv(p.id, GL_INFO_LOG_LENGTH, &llength);
        if (llength > 0)
        {
          std::vector<GLchar> log(llength);
          glGetProgramInfoLog(p.id, llength, &llength, log.data());
          log.resize(llength);
          std::cerr << std::string{begin(log), end(log)} << '\n';
        }
        
        glGetProgramiv(p.id, check_type, &status);
        if (status == 0)
          throw std::runtime_error{"check_prog(program_t): status == 0"};
      }
      
      friend void attach(program_t const& p,
                         shader_t const& s) { glAttachShader(p.id, s.id); }
      
      friend void link(program_t const& p)
      {
        glLinkProgram(p.id);
        check_prog(p, GL_LINK_STATUS);
      }
      friend void validate(program_t const& p)
      {
        glValidateProgram(p.id);
        check_prog(p, GL_VALIDATE_STATUS);
      }
    private:
      
      
    };
    
  }
}
#endif /* DPJ_PROGRAM_HH_ */
