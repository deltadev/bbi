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

#include "drawable.hh"
#include "program.hh"
#include "renderer.hh"
#include "shader.hh"

@implementation DPJAppDelegate

std::vector<char> read_shader(std::string file_name)
{
  std::ifstream is{"zoom_data.vsh"};
  if (!is.good())
    throw std::runtime_error{"read_shader(std::string): !is.good()"};
  
  std::istreambuf_iterator<char> first{is};
  std::istreambuf_iterator<char> last;
  return {first, last};
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  DPJGLView* glv = [[DPJGLView alloc] initWithFrame:NSRect{}];

  dpj::gl::program_t p;
  
  auto buf = read_shader("zoom_shader.vsh");
  
  dpj::gl::shader_t s{GL_VERTEX_SHADER, buf};
  compile(s);
  attach(p, s);

  link(p);
  validate(p);

  dpj::gl::renderer_t r;
}

@end
