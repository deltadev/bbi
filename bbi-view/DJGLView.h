#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

@class DJGLView;

@protocol DJGLDelegate

@required
- (void)viewDidInitGL;
- (void)glLayoutChanged:(DJGLView*)view;
@end

@interface DJGLView : NSOpenGLView
{
#ifdef __cplusplus
    @public
    class GLRenderer;
    GLRenderer* renderer_;
#endif
}

@property (assign, nonatomic) CVDisplayLinkRef displayLink;

- (void)reshapeFrustumFromNear:(float)near far:(float)far fov:(float)fov;

- (id)initWithFrame:(NSRect)frameRect;

@property (nonatomic, weak) id<DJGLDelegate> delegate;
@end
