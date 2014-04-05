//
//  DPJAppDelegate.m
//  new-gl-test
//
//  Created by Daniel James on 03/04/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import "DPJAppDelegate.h"
#import "DPJGLView.h"


#include <fstream>
#include <sstream>

#include "drawable.hh"
#include "program.hh"
#include "renderer.hh"
#include "shader.hh"
#include "buffer.hh"

#include <unordered_map>

using namespace dpj;
using std::vector;
namespace
{
  
  struct global
  {
    std::unordered_map<std::string, gl::program_t> programs;
    ~global() { for (auto & pair : programs) destroy(pair.second); }
  } global;
  
  vector<gl::drawable_t> scene;
}

@interface DPJAppDelegate ()

@property DPJGLView* glv;
@end

@implementation DPJAppDelegate

// Sets up the gl view as a subview of the window.
//
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  NSView* v = self.window.contentView;
  _glv = [[DPJGLView alloc] initWithFrame:v.bounds];
  _glv.delegate = self;
  [v addSubview:_glv];
}

// Not needed since we have our glview set to window's content view.
//
- (void)glLayoutChanged:(DPJGLView *)view { }

- (void)draw
{
  for (auto& d : scene)
    draw(d, *_glv->renderer);
}

struct triangle
{
  GLuint vao;
  gl::program_t p;
  
  triangle(gl::program_t p) : p{p}
  {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    
    const float c = ::cos(8 * ::atan(1) / 3);
    const float s = ::sin(8 * ::atan(1) / 3);
    data(gl::gen_buffer(), GL_ARRAY_BUFFER, vector<float>
         {
           1, 0, 0,
           c, s, 0,
           c, -s, 0
         });
    attr_ptr(p, "a_pos", 3);
    

    
    data(gl::gen_buffer(), GL_ARRAY_BUFFER, vector<float>
         {
           1, 0, 0,
           0, 1, 0,
           0, 0, 1
         });
    attr_ptr(p, "a_col", 3);


    
    data(gl::gen_buffer(), GL_ELEMENT_ARRAY_BUFFER, vector<GLuint>
         {
           0, 1, 2
         });
    


    validate(p);
    glBindVertexArray(0);
  }
  
  friend
  void draw(triangle const& t, gl::renderer const& r)
  {
    use(t.p);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(t.vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 10);
  }
};


- (void)viewDidInitGL
{
  std::istringstream vis
  {
    "#version 400\n"
    "uniform mat4 u_transform;\n"
    "in vec3 a_pos;\n"
    "in vec3 a_col;\n"
    "smooth out vec3 v_col;\n"
    "void main() {\n"
    "  v_col = a_col;\n"
    "  vec3 pos = a_pos;\n"
    "  pos.x = pos.x - gl_InstanceID * 0.1;\n"
    "  gl_Position = vec4(pos, 1);\n"
    "}\n"
  };
  std::istringstream fis
  {
    "#version 400\n"
    "in vec3 v_col;\n"
    "out vec4 f_col;\n"
    "void main() {\n"
    "  f_col = vec4(v_col, 1);\n"
    "\n}"
  };
  
  gl::shader_t vs{GL_VERTEX_SHADER, vis};
  gl::shader_t fs{GL_FRAGMENT_SHADER, fis};
  
  gl::program_t p{glCreateProgram()};
  p = attach(attach(p, compile(vs)), compile(fs));
  p = link(bind_attr(bind_attr(p, "a_pos"), "a_col"));
  
  global.programs.emplace("toy", p);
  
  scene.push_back(triangle{p});
}


@end
