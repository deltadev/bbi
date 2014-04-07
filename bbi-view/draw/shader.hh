#ifndef DPJ_SHADER_HH_
#define DPJ_SHADER_HH_

#include <OpenGL/gl3.h>

#include <iostream>

#include <vector>
#include <array>
#include <string>


namespace dpj
{
  using std::begin; using std::end;
  
  namespace gl
  {
    
    /// A typed GLuint.
    //
    //  This object is constructed with a std::istream. From that moment on
    //  its resources are managed by a program object.
    //
    //  intended usage:
    //
    //  program_t p;
    //
    //  std::istream is;
    //  attach(p, compile(sander_t{GL_SHADER_TYPE, is}))
    //
    //  This is not enforced at present. <- TODO: decide upon and impl. this
    //                                            policy.
    //
    class shader_t
    {
      
    public:
      
      GLuint id;
      
      // constructors
      //
      shader_t(GLenum type) : id{glCreateShader(type)}
      {
        if (id == 0)
          throw std::runtime_error("create_shader: failed to create shader");
      }
      
      shader_t(GLenum type, std::string s) : shader_t{type}
      {
        std::cout << "loading shader:\n";
        std::cout << s << "\n\n";
        load_source(*this, begin(s), end(s));
      }
        shader_t(GLenum type, std::istream& is) : shader_t{type}
      {
        std::istreambuf_iterator<char> first{is}, last;
        load_source(*this, first, last);
      }
        
      // compile
      //
      friend
      shader_t compile(shader_t s)
      {
        glCompileShader(s.id);
        
        GLint status;
        glGetShaderiv(s.id, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
          log(s, std::cerr);
          throw std::runtime_error("compile(shader_t): status == 0");
        }
        return s;
      }
      
      // log
      //
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

      template<typename It> friend
      void load_source(shader_t s, It first, It last)
      {
        std::vector<char> data{first, last};
        
        GLint size = static_cast<GLint>(data.size());
        
        // &ptr stuff is for arrays of c_str
        const char* ptr = data.data();
        glShaderSource(s.id, 1, &ptr, &size);
        
      }
    };
  }
}
#endif /* DPJ_SHADER_HH_ */
