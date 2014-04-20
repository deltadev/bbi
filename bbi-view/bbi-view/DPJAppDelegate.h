//
//  DPJAppDelegate.h
//  bbi-view
//
//  Created by Daniel James on 20/03/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "DPJGLView.h"

@interface DPJAppDelegate : NSObject <NSApplicationDelegate, DPJGLDelegate,
                                      NSTableViewDelegate, NSTableViewDataSource>

@property (assign) IBOutlet NSWindow *window;

- (void)glLayoutChanged:(DPJGLView*)view;

@end
