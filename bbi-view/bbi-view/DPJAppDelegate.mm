//
//  DPJAppDelegate.m
//  bbi-view
//
//  Created by Daniel James on 20/03/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import <Quartz/Quartz.h>
#import <OpenGL/gl3.h>
#import <GLKit/GLKit.h>

#import "DPJAppDelegate.h"
#import "DJGLView.h"

#include "GLRenderer.hh"
#include "GLProgram.hh"
#include "GLDrawable.hh"
#include "GLVertexArrayObject.hh"

#import "bbi_bed_file.h"
#import "bed_record.h"
#import "zoom_record.h"

@interface BBIView : DJGLView
{
  std::unique_ptr<bbi::bed_file> bed_file;
  std::unique_ptr<std::istream>  stream_ptr;
}

@end

@implementation BBIView

-(instancetype)initWithFrame:(NSRect)frameRect fileName:(NSString*)fileName
{
  
  NSOpenGLPixelFormatAttribute attrs[] =
  {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFADepthSize, 24,
    NSOpenGLPFAOpenGLProfile,
    NSOpenGLProfileVersion3_2Core,
    0
  };
  
  NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
  
  if (!pf)
    NSLog(@"No OpenGL pixel format");
  
  
  if (self = [super initWithFrame:frameRect])
  {
    stream_ptr.reset(new std::ifstream{[fileName UTF8String]});
    if (!stream_ptr->good())
    {
      NSLog(@"file %@ not good.", fileName);
    }
    bed_file.reset(new bbi::bed_file{*stream_ptr});

  }
  return self;
}

@end


struct some_data : GLDrawable
{
  some_data(std::string shader/* = "some_shader"*/) : GLDrawable(shader)
  { program = std::make_shared<GLProgram>(shader); }
  
  void set_data(std::vector<bbi::zoom::record> const& data)
  {
    program->useProgram();
    auto vao = std::make_shared<GLVertexArrayObject>(program->programID());

    Eigen::MatrixXf edata(3, data.size());
    int idx = 0;
    for (auto const& d : data)
    {
      edata.col(idx++) = Eigen::Vector3f{d.chrom_start, d.chrom_end, 0};
    }
    vao->attributedBuffer(edata, "aVertex");
    vao->primitiveType = GL_LINES;
    vaos.push_back(vao);

  }
  void draw()
  {
    GLDrawable::draw();
  }
};


@interface DPJAppDelegate ()

@property (weak) IBOutlet NSView *glView;
@end

@implementation DPJAppDelegate

- (void)awakeFromNib
{
  
  NSLog(@"%@", [_glView class]);
  NSRect f = _glView.frame;
  
  BBIView* v = [[BBIView alloc] initWithFrame:f fileName:@"many_contig_data.bb"];
  v.delegate = self;

  [_glView addSubview:v];
    
  auto d = std::make_shared<some_data>();
  
  v->renderer_->drawables.push_back(d);
  
}
- (void)viewDidInitGL
{
  NSLog(@"view did init gl");
}
- (void)delegateKeyDown:(NSEvent*)theEvent
{
  BBIView* v = _glView.subviews.firstObject;

  auto d = std::make_shared<some_data>();

  v->renderer_->drawables.push_back(d);
  
}
@end
