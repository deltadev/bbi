#import "DPJAppDelegate.h"
#import "DPJGLView.h"

#include <fstream>
#include <sstream>

#include "drawable.hh"
#include "program.hh"
#include "renderer.hh"
#include "shader.hh"
#include "buffer.hh"

#include "triangle.hh"

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
  
  using scene_t = vector<gl::drawable_t>;
  scene_t scene;

  void draw(scene_t& scene, gl::renderer const& r)
  {
    for (auto& d : scene)
    {
      update(d);
      draw(d, r);
    }
  }
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

- (void)draw { draw(scene, *_glv->renderer); }

- (void)viewDidInitGL
{
  std::istringstream vis
  {
    "#version 330\n"
    "in vec3 a_pos;\n"
    "in vec3 a_col;\n"
    "smooth out vec3 v_col;\n"
    "uniform mat4 u_transform;\n"
    "void main() {\n"
    "  v_col = a_col;\n"
    "  float x = gl_InstanceID;\n"
    "  vec4 pos = u_transform * vec4(a_pos, 1);\n"
    "  //vec4 pos = vec4(a_pos, 1);\n"
    "  pos.x = pos.x - gl_InstanceID * 0.1;\n"
    "  gl_Position = pos;\n"
    "}\n"
  };
  std::istringstream fis
  {
    "#version 330\n"
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
