#ifndef DPJ_PROGRAM_HH_
#define DPJ_PROGRAM_HH_

#include <OpenGL/gl3.h>

#include <vector>
#include <string>

#include "util.hh"
#include "shader.hh"

namespace dpj
{
  namespace gl
  {
    
    class program_t
    {

    public:
      GLuint id;
      program_t() { }
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
      
      // Functions
      //
      friend
      program_t log(program_t p, GLenum check_type)
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
        
        return p;
      }
      
      friend
      program_t attach(program_t p, shader_t s)
      {
        glAttachShader(p.id, s.id);
        return p;
      }
      
      friend
      program_t link(program_t p)
      {
        glLinkProgram(p.id);
        return log(p, GL_LINK_STATUS);
      }
      
      friend
      program_t validate(program_t p)
      {
        glValidateProgram(p.id);
        return log(p, GL_VALIDATE_STATUS);
      }
      
    private:
      
    };
    
    program_t create_program()
    {
      program_t p;
      p.id = glCreateProgram();
      return p;
    }
  }
}
#endif /* DPJ_PROGRAM_HH_ */
