#ifndef DPJ_RENDERER_HH_
#define DPJ_RENDERER_HH_

#include <cmath>
#include <OpenGL/gl3.h>

namespace dpj
{
  namespace gl
  {
    const double PI = std::atan(1) * 4;
    
    class renderer_t
    {
      
    public:
      struct angle { float x, y, z; } angle;
      struct view { float near, far, fov; } view;
      struct offset { float x, y, z; } offset;
      
      renderer_t() : angle{0, 0, 0}, view{0.01, 1, 45}, offset{0, 0, 0}
      {
        GLint sz = 0;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &sz);
        
        sz = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sz);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
        
        // Depth test will always be enabled
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDepthRange(0.0f, 1.0f);
        
        // We will always cull back faces for better performance
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // default
        glClearColor(1.0,1.0,1.0,0);
        
        glClearDepth(1.0f);
        
      }
    };
    
  }
}
#endif /* DPJ_RENDERER_HH_ */
