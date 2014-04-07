//
//  main.m
//  big-data-cloud
//
//  Created by Daniel James on 05/04/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import <Cocoa/Cocoa.h>


int main(int argc, const char * argv[])
{
  int r;
  @try
  {
    r = NSApplicationMain(argc, argv);
  }
  @catch (...)
  {
    NSLog(@"Exception: \n");
  }
  return r;
}
