//
//  DPJAppDelegate.m
//  bbi-view
//
//  Created by Daniel James on 20/03/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import <GLKit/GLKit.h>

#import "DPJAppDelegate.h"
#import "DJGLView.h"
#import "BBIView.h"


#import "contig_index.hh"
#import "bbi_index.hh"
#import "bbi_stream.hh"

#import "bed_record.hh"
#import "wig_record.hh"
#import "zoom_record.hh"


#include "GLRenderer.hh"
#include "drawables.hh"

namespace
{
  std::unique_ptr<bbi::stream> bbi_stream;
  bbi::record qi;
}

@interface DPJAppDelegate ()
{
  int _zoom_level;
}
@property (weak) IBOutlet NSTableView *tableView;
@property (strong) NSMutableArray* tableViewContent;

@property (nonatomic, weak) IBOutlet NSTextField *resource;

@property (weak) IBOutlet NSTextField *start;
@property (weak) IBOutlet NSTextField *end;


@property (unsafe_unretained) IBOutlet NSTextField *zoomLevelTextField;

@property (weak) IBOutlet NSView *glView;
@end

@implementation DPJAppDelegate
- (void)viewDidInitGL
{
  NSLog(@"view did init gl");
}
- (void)delegateKeyDown:(NSEvent*)theEvent { }
- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
  qi = {0, 0, 10000000};
  
  _start.intValue = qi.chrom_start;
  _end.intValue = qi.chrom_end;
  
  _zoom_level = 5;
  _zoomLevelTextField.intValue = _zoom_level;
  
  //
  BBIView* v = [[BBIView alloc] initWithFrame:_glView.bounds];
  v.delegate = self;
  [_glView addSubview:v];
}

- (void)glLayoutChanged:(BBIView*)view
{
  NSRect r = view.frame;
  NSLog(@"new frame dimensions are: %f %f %f %f.",
        r.origin.x, r.origin.y, r.size.width, r.size.height);
  
  //int num_tiers = r.size.height / 100;
  //float tier_length = r.size.width - 100; // 50 pixels either end.
  
  // Now we want to update the 'tier uniforms' for the shaders.
}

- (IBAction)textFieldsChanged:(id)sender
{
  if (sender == _start && qi.chrom_start != _start.intValue)
  {
    qi.chrom_start = _start.intValue;
    [self refreshData];
  }
  else if (sender == _end && qi.chrom_end != _end.intValue)
  {
    qi.chrom_end = _end.intValue;
    [self refreshData];
  }
  else if (sender == _zoomLevelTextField && _zoomLevelTextField.intValue != _zoom_level)
  {
    _zoom_level = _zoomLevelTextField.intValue;
    [self refreshData];
  }
  else if (sender == _resource)
  {
    NSLog(@"attempting to open resource: %@", _resource.stringValue);
    std::string resource{[_resource.stringValue UTF8String]};
    try
    {
      bbi_stream.reset(new bbi::stream{resource});
      [self setupTableView];
      [self refreshData];
    }
    catch (std::runtime_error& e)
    { std::cerr << "Exception: " << e.what(); }
  }
}

- (void)refreshData
{
  BBIView* v = _glView.subviews.firstObject;
  if (v && bbi_stream)
  {
    v->renderer_->drawables.clear();
    
    auto rindex = index(*bbi_stream, _zoom_level);
    auto blocks = search(rindex, qi);
    NSLog(@"Retrieved %d leaves for zoom level %d.", (int)blocks.size(), _zoom_level);

    if (_zoom_level > 0)
    {
      auto d = std::make_shared<zoom_data>();
      int counter = 0;
      for (auto const& ln : blocks)
      {
        seek(*bbi_stream, ln);
        d->add(extract<bbi::zoom::record>(qi, *bbi_stream), counter++);
      }
      v->renderer_->drawables.push_back(d);

      NSLog(@"Extracted %ld zoom records", (long)(blocks.size() * rindex.header.item_count));
    }
    else if (bbi_stream->type == bbi::stream::type::wig)
    {
      using namespace bbi::wig;
      for (auto const& ln : blocks)
      {
        seek(*bbi_stream, ln);
        bbi::wig::header wh{bbi_stream.get()};
        NSLog(@"extracting wig header of type %d", wh.type);
        auto d = std::make_shared<wig_data>();
        
        if (wh.type == (int)bbi::wig::record_type::var_step)
          d->add(extract<var_step_record>(*bbi_stream, wh.item_count), wh.item_span);
        else if (wh.type == (int)bbi::wig::record_type::fixed_step)
          d->add(extract<fixed_step_record>(*bbi_stream, wh.item_count), wh.item_span);
        else if (wh.type == (int)bbi::wig::record_type::bed_graph)
          d->add(extract<bed_graph_record>(*bbi_stream, wh.item_count));

        v->renderer_->drawables.push_back(d);
      }
      NSLog(@"Extracted %ld wig records", (long)(blocks.size() * rindex.header.item_count));
    }
  }
  else if (bbi_stream->type == bbi::stream::type::bed)
  { throw std::runtime_error("Exception: bed data records not supported."); }
}



#pragma mark - <NSTableViewDataSource>

- (void)setupTableView
{
  _tableView.delegate = self;
  _tableView.dataSource = self;
  _tableViewContent = [[NSMutableArray alloc] init];
  if (bbi_stream)
  {
    auto ctigs = ctig_index(*bbi_stream);
    
    for (auto& ctig : ctigs)
    {
      auto key = ctig.first;
      NSDictionary* dict = [[NSDictionary alloc] initWithObjectsAndKeys:
                            [NSString stringWithUTF8String:key.c_str()], @"key",
                            [NSNumber numberWithInt:contig_id(ctigs, key)], @"id",
                            [NSNumber numberWithInteger:contig_size(ctigs, key)], @"size", nil];
      [_tableViewContent addObject:dict];
    }
    NSLog(@"Added %lu items to table view.", [_tableViewContent count]);
    [_tableView reloadData];
  }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
  NSInteger num = [_tableViewContent count];
  return num;
}

-           (id)tableView:(NSTableView *)tableView
objectValueForTableColumn:(NSTableColumn *)tableColumn
                      row:(NSInteger)row
{
  NSDictionary* dict = [_tableViewContent objectAtIndex:row];
  if ([tableColumn.identifier isEqualToString:@"key"])
  {
    return [dict objectForKey:@"key"];
  }
  else if ([tableColumn.identifier isEqualToString:@"id"])
  {
    return [dict objectForKey:@"id"];
  }
  else if ([tableColumn.identifier isEqualToString:@"size"])
  {
    return [dict objectForKey:@"size"];
  }
  
  return nil;
}

#pragma mark - <NSTableViewDelegate>
- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
  NSTableView* tv = notification.object;
  NSLog(@"selectedRow : %ld", tv.selectedRow);
  NSInteger row = tv.selectedRow;
  
  NSDictionary* rowDict = [_tableViewContent objectAtIndex:row];
  
  int contig_id = [[rowDict objectForKey:@"key"] intValue];
  
  NSLog(@"selected contig %@ with key %u", [rowDict objectForKey:@"id"], contig_id);
  
}
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange:(NSArray *)oldDescriptors
{
  NSLog(@"selectedRow : %ld", tableView.selectedRow);
  [_tableViewContent sortUsingDescriptors: [tableView sortDescriptors]];
  NSLog(@"selectedRow : %ld", tableView.selectedRow);
  [tableView reloadData];
}


@end
