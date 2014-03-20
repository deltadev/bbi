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

}

@end

@implementation BBIView

-(instancetype)initWithFrame:(NSRect)frameRect
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

  }
  return self;
}

@end


struct some_data : GLDrawable
{
  some_data(std::string shader = "default") : GLDrawable(shader) { }
  
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
    Eigen::MatrixXf enorms(Eigen::MatrixXf::Ones(3, data.size()));
    Eigen::Vector3f color(0.2, 0.3, 0.4);
    Eigen::MatrixXf ecolors(color.rowwise().replicate(data.size()));

    vao->attributedBuffer(edata, "aVertex");
    vao->attributedBuffer(enorms, "aNormal");
    vao->attributedBuffer(ecolors, "aColor");
    vao->primitiveType = GL_LINES;
    vaos.push_back(vao);

  }
  void draw()
  {
    GLDrawable::draw();
  }
};


@interface DPJAppDelegate ()
{
  std::unique_ptr<bbi::bed_file> bed_file;
  std::unique_ptr<std::istream>  stream_ptr;
}

@property (weak) IBOutlet NSView *glView;
@end

@implementation DPJAppDelegate

- (void)awakeFromNib
{
  std::string file_name = "many_contig_data.bb";
  stream_ptr.reset(new std::ifstream{file_name});
                                      
  if (!stream_ptr->good())
  {
    std::cout << file_name << " not good.\n";
    exit(EXIT_FAILURE);
  }
  
  bed_file.reset(new bbi::bed_file{*stream_ptr});
  
  NSLog(@"%@", [_glView class]);
  
  BBIView* v = [[BBIView alloc] initWithFrame:_glView.bounds];
  v.delegate = self;

  [_glView addSubview:v];
  
}
- (void)viewDidInitGL
{
  NSLog(@"view did init gl");

  BBIView* v = _glView.subviews.firstObject;
  
  auto d = std::make_shared<some_data>();
  
  v->renderer_->drawables.push_back(d);
  
  auto index = bed_file->index(1);
  auto leaves = index.search({0, 0, 1000});
  
  std::istream is{bed_file->fill_stream(leaves.front())};
  
  bbi::zoom::record zr;
  std::vector<bbi::zoom::record> zrs;
  while(is >> zr)
    zrs.push_back(zr);
  
  d->set_data(zrs);
    
    
}
- (void)delegateKeyDown:(NSEvent*)theEvent
{

  
}
@end
