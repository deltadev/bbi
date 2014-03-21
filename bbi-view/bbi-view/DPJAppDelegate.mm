//
//  DPJAppDelegate.m
//  bbi-view
//
//  Created by Daniel James on 20/03/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import <Quartz/Quartz.h>
#import <GLKit/GLKit.h>

#import "DPJAppDelegate.h"
#import "DJGLView.h"

#include "GLRenderer.hh"
#include "GLProgram.hh"
#include "GLDrawable.hh"
#include "GLVertexArrayObject.hh"

#import "bbi_bed_file.h"
#import "bbi_wig_file.h"
#import "bed_record.h"
#import "wig_record.h"
#import "zoom_record.h"

#import "BBIView.h"

#include "drawables.hh"

namespace
{
  std::unique_ptr<bbi::file_base> bbi_file;
  std::unique_ptr<std::istream>  stream_ptr;
  bbi::record qi;
}

@interface DPJAppDelegate ()
{

}
@property (unsafe_unretained) IBOutlet NSTextField *zoomLevelTextField;

@property (weak) IBOutlet NSView *glView;
@end

@implementation DPJAppDelegate

- (IBAction)zoomLevelChanged:(id)sender
{
  if (sender == _zoomLevelTextField)
  {
    int zoom_level = _zoomLevelTextField.intValue;
    auto index = bbi_file->index(zoom_level);

    std::cerr << "\n\nIndex retrived: \n" << index << '\n';
    
    auto r_leaves = index.search(qi);
    
    NSLog(@"Retrieved %d leaves for zoom level %d.", (int)r_leaves.size(), zoom_level);
    
    BBIView* v = _glView.subviews.firstObject;
    v->renderer_->drawables.clear();


    std::vector<bbi::zoom::record> zrs;
    std::vector<bbi::wig::var_step_record> rs;
    
    for (auto const& ln : r_leaves)
    {
      std::cerr << "obtaining record stream from r_tree leaf:\n";
      ln.print(std::cerr);
      
      std::istream is{bbi_file->fill_stream(ln)};
      
      if (zoom_level > 0)
      {
        bbi::zoom::record r;
        while (is >> r)
        {
          if (r.chrom_id == qi.chrom_id && r.chrom_start < qi.chrom_end && qi.chrom_start < r.chrom_end)
            zrs.push_back(r);
        }
      }
      else if (bbi_file->file_type == bbi::file_type::wig)
      {
        bbi::wig::var_step_record r;
        while (is >> r)
          rs.push_back(r);
      }
    }
    if (zoom_level > 0)
    {
      auto d = std::make_shared<zoom_data>();
      d->set_data(zrs);
      v->renderer_->drawables.push_back(d);
      NSLog(@"Extracted %ld zoom records", (long)zrs.size());
      std::cerr << "last record was:\n";
      zrs.back().print(std::cerr);
    }
    else
    {
      auto d = std::make_shared<wig_data>();
      d->set_data(rs);
      v->renderer_->drawables.push_back(d);
      NSLog(@"Extracted %ld wig records", (long)rs.size());
    }
  }
}

- (void)awakeFromNib
{
  qi = {0, 0, 1000};
  
  std::string file_name = "test.bw";
  stream_ptr.reset(new std::ifstream{file_name});
  
  if (!stream_ptr->good())
  {
    std::cout << file_name << " not good.\n";
    exit(EXIT_FAILURE);
  }
  bbi_file.reset(new bbi::file_base{*stream_ptr});
  
  BBIView* v = [[BBIView alloc] initWithFrame:_glView.bounds];
  v.delegate = self;
  
  [_glView addSubview:v];
}

- (void)viewDidInitGL { NSLog(@"view did init gl"); }

- (void)delegateKeyDown:(NSEvent*)theEvent { }
@end
