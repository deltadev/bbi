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
      GLuint attr_idx_ = 0;
    public:
      GLuint id;

      // To be constructed with an id returned from glCreateProgram().
      //
      program_t(GLuint id) : id{id}
      {
        if (id == 0)
          throw std::runtime_error("program_t: id == 0");
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
      program_t use(program_t p)
      {
        glUseProgram(p.id);
        return p;
      }
      friend
      program_t validate(program_t p)
      {
        glValidateProgram(p.id);
        return log(p, GL_VALIDATE_STATUS);
      }
      friend
      program_t bind_attr(program_t p, std::string attrib)
      {
        glBindAttribLocation(p.id, p.attr_idx_++, attrib.c_str());
        return p;
      }
      friend
      program_t attr_ptr(program_t p, std::string attr, GLuint dim)
      {
        GLint loc = glGetAttribLocation(p.id, attr.c_str());
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, dim, GL_FLOAT, GL_FALSE, 0, nullptr);
        return p;
      }
      
      friend
      void destroy(program_t p)
      {
        if (p.id == 0)
          throw std::runtime_error("destroy_program: id == 0");
        else
        {
          GLsizei shaderCount;
          
          glGetProgramiv(p.id, GL_ATTACHED_SHADERS, &shaderCount);
          
          std::vector<GLuint> shaders(shaderCount);
          glGetAttachedShaders(p.id, shaderCount, &shaderCount, shaders.data());
          
          for (GLuint shader : shaders)
            glDeleteShader(shader);
          
          glDeleteProgram(p.id);
          glUseProgram(0);
        }
      }
      
    };
  }
}
#endif /* DPJ_PROGRAM_HH_ */
