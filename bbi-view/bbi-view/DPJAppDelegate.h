//
//  DPJAppDelegate.h
//  bbi-view
//
//  Created by Daniel James on 20/03/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "DJGLView.h"

@interface DPJAppDelegate : NSObject <NSApplicationDelegate, DJGLDelegate, NSTableViewDelegate>

@property (assign) IBOutlet NSWindow *window;

@end
