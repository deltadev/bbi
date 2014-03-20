#import "DJGLView.h"

#import <OpenGL/gl3.h>

#include "GLRenderer.hh"


@interface NSObject (DJGLSupport)
- (void)delegateKeyDown:(NSEvent*)theEvent;

@end

@implementation DJGLView
@synthesize displayLink = _displayLink;

@synthesize delegate = _delegate;

- (void)reshapeFrustumFromNear:(float)near far:(float)far fov:(float)fov
{
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    renderer_->near = near;
    renderer_->far = far;
    renderer_->fov = fov;

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    
}

- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
	[self drawView];
	return kCVReturnSuccess;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result;
    @autoreleasepool {
        result = [(__bridge DJGLView*)displayLinkContext getFrameForTime:outputTime];
    }
    return result;
}
- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (id)initWithFrame:(NSRect)frameRect
{
    renderer_ = new GLRenderer();
    
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
	{
		NSLog(@"No OpenGL pixel format");
	}
	
	self = [super initWithFrame:frameRect pixelFormat:pf];
	
	return self;
}
- (void) dealloc
{
	// Release the display link
	CVDisplayLinkRelease(_displayLink);
    delete renderer_;
}


- (void)prepareOpenGL
{
	[super prepareOpenGL];
    
    [self initGL];
    
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
	
    @autoreleasepool 
    {
        // Set the renderer output callback function
        CVDisplayLinkSetOutputCallback(_displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
    }	
    
	// Set the display link for the current renderer
	CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, cglContext, cglPixelFormat);
	
	// Activate the display link
	CVDisplayLinkStart(_displayLink);

    if (!_delegate)
    {
        NSLog(@" delegate not set. Needed for view didInitGL");
        assert(_delegate);
    }
    else
    {
        CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

        [_delegate viewDidInitGL];
        
        CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    }
}

- (void) initGL
{
	// Make this openGL context current to the thread
	// (i.e. all openGL on this thread calls will go to this context)
	[[self openGLContext] makeCurrentContext];
	
	// Synchronize buffer swaps with vertical refresh rate
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    renderer_->initGL();
}

- (void)reshape
{	
	[super reshape];
	
	// We draw on a secondary thread through the display link
	// When resizing the view, -reshape is called automatically on the main thread
	// Add a mutex around to avoid the threads accessing the context simultaneously when resizing
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	NSRect rect = [self bounds];


    renderer_->resizeViewport(rect.size.width, rect.size.height);

	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void) drawView
{	 
	[[self openGLContext] makeCurrentContext];
    
	// We draw on a secondary thread through the display link
	// When resizing the view, -reshape is called automatically on the main thread
	// Add a mutex around to avoid the threads accessing the context simultaneously	when resizing
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);


    renderer_->render();

	CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)mouseDragged:(NSEvent*)event
{
    if ([event modifierFlags] & NSControlKeyMask ) {
        renderer_->offX += 0.1*[event deltaX];
        renderer_->offY -= 0.1*[event deltaY];
    }
    else
    {
        renderer_->angleY += 0.5*[event deltaX]; // left-right rotates asround y-axis
        renderer_->angleX += 0.5*[event deltaY]; // up-down around x-axis.
    }
    
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    if ([theEvent modifierFlags] & NSControlKeyMask)
    {
        renderer_->offZ += 0.1*[theEvent scrollingDeltaY];
    }
    else
    {
        float tmp = renderer_->fov + 0.1*[theEvent scrollingDeltaY];
        if (tmp <= 0) {
            tmp = 0;
        }
        else if (tmp >= 180) {
            tmp = 180;
        }
        renderer_->fov = tmp;
    }
}

- (void)keyDown:(NSEvent *)theEvent
{
    id delegate = [NSApp delegate];
    [delegate delegateKeyDown:theEvent];
}
@end
