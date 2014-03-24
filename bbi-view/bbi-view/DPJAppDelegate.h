//
//  DPJAppDelegate.h
//  bbi-view
//
//  Created by Daniel James on 20/03/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "BBIView.h"

@interface DPJAppDelegate : NSObject <NSApplicationDelegate, DJGLDelegate,
                                      NSTableViewDelegate, NSTableViewDataSource>

@property (assign) IBOutlet NSWindow *window;
- (void)glLayoutChanged:(BBIView*)view;

@end
