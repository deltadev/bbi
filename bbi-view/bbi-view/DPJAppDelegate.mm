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

#import "bbi_bed_file.h"
#import "bbi_wig_file.h"
#import "bed_record.h"
#import "wig_record.h"
#import "zoom_record.h"

#import "BBIView.h"


#include "bbi_view_ctrl.hh"

namespace
{
  std::unique_ptr<std::istream>  stream_ptr;
  std::unique_ptr<bbi_view_ctrl> view_cntrl;
  bbi::record qi;

}

@interface DPJAppDelegate ()
@property (weak) IBOutlet NSTableView *tableView;
@property (strong) NSMutableArray* tableViewContent;

@property (weak) IBOutlet NSTextField *contig;

@property (weak) IBOutlet NSTextField *start;
@property (weak) IBOutlet NSTextField *end;


@property (unsafe_unretained) IBOutlet NSTextField *zoomLevelTextField;

@property (weak) IBOutlet NSView *glView;
@end

@implementation DPJAppDelegate

- (void)awakeFromNib
{
  qi = {0, 0, 1000};
  _contig.intValue = qi.chrom_id;
  _start.intValue = qi.chrom_start;
  _end.intValue = qi.chrom_end;
  
  std::string file_name = "test.bw";
  stream_ptr.reset(new std::ifstream{file_name});
  view_cntrl.reset(new bbi_view_ctrl{*stream_ptr});
  
  if (!stream_ptr->good())
  {
    std::cout << file_name << " not good.\n";
    exit(EXIT_FAILURE);
  }
  
  BBIView* v = [[BBIView alloc] initWithFrame:_glView.bounds];
  v.delegate = self;
  
  [_glView addSubview:v];
}

- (void)viewDidInitGL { NSLog(@"view did init gl"); }

- (void)delegateKeyDown:(NSEvent*)theEvent { }

- (IBAction)zoomLevelChanged:(id)sender
{
  if (sender == _start)
  {
    qi.chrom_start = _start.intValue;
  }
  else if (sender == _end)
  {
    qi.chrom_end = _end.intValue;
  }
  else if (sender == _zoomLevelTextField)
  {
    
  }
  [self refreshData];
}
- (void)refreshData
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

#pragma mark - <NSTableViewDataSource>

- (void)setupTableView
{
  _tableView.delegate = self;
  _tableView.dataSource = self;
  _tableViewContent = [[NSMutableArray alloc] init];
  for (auto& ad : view_cntrl->array())
  {
    
    NSDictionary* dict = [[NSDictionary alloc] initWithObjectsAndKeys:
                          key, @"key",
                          contig_id, @"contig id",
                          contig_size, @"contig size", nil];
    [_tableViewContent addObject:dict];
  }
  NSLog(@"Added %lu items to table view.", [_tableViewContent count]);
  [_tableView reloadData];
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
  if ([tableColumn.identifier isEqualToString:@"tfName"])
  {
    return [dict objectForKey:@"tfName"];
  }
  else if ([tableColumn.identifier isEqualToString:@"arrayDesign"])
  {
    return [dict objectForKey:@"arrayDesign"];
  }
  else if ([tableColumn.identifier isEqualToString:@"arrayID"])
  {
    return [dict objectForKey:@"arrayID"];
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

  NSLog(@"selected contig %@ with key %u", [rowDict objectForKey:@"contig id"], arrayID);
  
  [self selectArrayWithDataBaseID:arrayID];
}
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange:(NSArray *)oldDescriptors
{
  NSLog(@"selectedRow : %ld", tableView.selectedRow);
  [_tableViewContent sortUsingDescriptors: [tableView sortDescriptors]];
  NSLog(@"selectedRow : %ld", tableView.selectedRow);
  [tableView reloadData];
}


@end
