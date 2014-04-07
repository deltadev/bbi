#ifndef DPJ_TRIANGLE_HH_
#define DPJ_TRIANGLE_HH_

#include <vector>

#include "program.hh"
#include "buffer.hh"

using namespace dpj;
using std::vector;
struct triangle
{
  GLuint vao;
  GLint u_loc;
  gl::program_t p;
  float x = 0;
  
  triangle(gl::program_t p) : p{p}, u_loc{unif_loc(p, "u_transform")}
  {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    const float c = ::cos(8 * ::atan(1) / 3);
    const float s = ::sin(8 * ::atan(1) / 3);
    load(buffer(gl::global, "a_pos"), GL_ARRAY_BUFFER, vector<float>
         {
           1, 0, 0,
           c, s, 0,
           c, -s, 0
         });
    attr_ptr(p, "a_pos", 3);
    
    
    load(buffer(gl::global, "a_col"), GL_ARRAY_BUFFER, vector<float>
         {
           1, 0, 0,
           0, 1, 0,
           0, 0, 1
         });
    attr_ptr(p, "a_col", 3);
    
    
    load(buffer(gl::global, "index"), GL_ELEMENT_ARRAY_BUFFER, vector<GLuint>
         {
           0, 1, 2
         });
    
    validate(p);
    glBindVertexArray(0);
  }
  friend void update(triangle& t)
  {
    t.x += 0.001;
    t.x  = t.x > 1 ? t.x - 5 : t.x;
  }
  
  friend
  void draw(triangle const& tri, gl::renderer const& r)
  {
    use(tri.p);
    
    gl::transform t;
    t(1, 1) += tri.x;
    t(0, 0) += tri.x;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(tri.vao);
    
    glUniformMatrix4fv(tri.u_loc, 1, GL_FALSE, t.data());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 20);
  }
};


#endif /* DPJ_TRIANGLE_HH_ */
