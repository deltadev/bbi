#ifndef DPJ_BUFFER_HH_
#define DPJ_BUFFER_HH_

#include <OpenGL/gl3.h>

namespace dpj
{
  namespace gl
  {
    struct buffer_t
    {
      GLuint id;
      buffer_t(GLuint id) : id{id} { }
      
      template<typename Container> friend
      buffer_t load(buffer_t b, int type, Container const& c)
      {
        glBindBuffer(type, b.id);
        glBufferData(type,
                     c.size() * sizeof (typename Container::value_type),
                     c.data(),
                     GL_STATIC_DRAW);
        return b;
      }

      friend
      void destroy(buffer_t b) { glDeleteBuffers(1, &b.id); }
    };
  }
}


#endif /* DPJ_BUFFER_HH_ */
