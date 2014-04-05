#ifndef DPJ_BUFFER_HH_
#define DPJ_BUFFER_HH_

namespace dpj
{
  namespace gl {
    
    
    struct buffer
    {
      GLuint id;
      buffer(GLuint id) : id{id} { }
            
      template<typename Container> friend
      buffer data(buffer b, int type, Container const& c)
      {
        glBindBuffer(type, b.id);
        glBufferData(type,
                     c.size() * sizeof (typename Container::value_type),
                     c.data(),
                     GL_STATIC_DRAW);
        return b;
      }
    };
    
    buffer gen_buffer()
    {
      GLuint id;
      glGenBuffers(1, &id);
      return id;
    }
    
    
  }
}


#endif /* DPJ_BUFFER_HH_ */
