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

@interface DPJAppDelegate ()
@property DPJGLView* glv;
@end

@implementation DPJAppDelegate

std::vector<char> shader_source(std::string file_name)
{
  std::ifstream is{"zoom_data.vsh"};
  if (!is.good())
    throw std::runtime_error{"read_shader(std::string): !is.good()"};
  
  std::istreambuf_iterator<char> first{is};
  std::istreambuf_iterator<char> last;
  return {first, last};
}


- (void)viewDidInitGL
{
  
  auto p  = dpj::gl::create_program();
  auto vs = dpj::gl::create_shader(GL_VERTEX_SHADER, shader_source("zoom_shader.vsh"));

  validate(link(attach(p, compile(vs))));

}

- (void)glLayoutChanged:(DPJGLView *)view
{
  
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  NSView* v = self.window.contentView;
  _glv = [[DPJGLView alloc] initWithFrame:v.bounds];
  _glv.delegate = self;
  [v addSubview:_glv];
}

@end
