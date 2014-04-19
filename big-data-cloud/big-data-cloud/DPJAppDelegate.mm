#include <sys/stat.h> // for fifo

#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sstream>

#import "DPJAppDelegate.h"
#import "DPJGLView.h"
#include "gl.hh"
#include "array_streambuf.hh"

#include "point_cloud.hh"

@interface DPJAppDelegate () <NSStreamDelegate, DPJGLDelegate>
{
  point_cloud point_cloud_;
  std::unique_ptr<dpj::array_streambuf> sb;
}
@property NSInputStream* inputStream;
@property NSTextView* textView;
@property DPJGLView* glView;
@end

@implementation DPJAppDelegate


- (void)viewDidInitGL
{
  auto prog = program(gl::global, "point_cloud");
  
  gl::shader_t vs{GL_VERTEX_SHADER, R"(
#version 400
    uniform mat4 u_transform; // scales all points to [-1, 1]^3.
    in vec3 a_pos;
    
    void main()
    {
      vec4 pos = u_transform * vec4(a_pos, 1);
      gl_PointSize = 4;
      gl_Position = pos;
    }
    )"
  };
  
  gl::shader_t fs{GL_FRAGMENT_SHADER, R"(
#version 400
    out vec4 f_col;
    void main()
    {
      f_col = vec4(0, 0, 0, 1); // black dots.
    }
    )"
  };
  
  prog = attach(attach(prog, compile(vs)), compile(fs));
  prog = link(bind_attr(prog, "a_pos"));
  
  point_cloud_ = point_cloud{prog};
  sb.reset(new dpj::array_streambuf{});
}

- (void)glLayoutChanged:(DPJGLView *)view { }

- (void)draw { draw(point_cloud_, *_glView->renderer); }

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  NSView* v = _window.contentView;
  NSView* lv = [v.subviews objectAtIndex:0];
  NSView* rv = [v.subviews objectAtIndex:1];
  
  _textView = [[NSTextView alloc] initWithFrame:lv.bounds];
  [lv addSubview:_textView];
  _glView = [[DPJGLView alloc] initWithFrame:rv.bounds];
  [rv addSubview:_glView];
  _glView.delegate = self;
  
  // FIFO for testing.
  //
  NSString* fifo = [@"~/fifos/gl" stringByStandardizingPath];
  if (mkfifo([fifo UTF8String], 0644) == -1)
    NSLog(@"error creating fifo: %s", strerror(errno));
  
  _inputStream = [self inputStreamForFile:fifo];
  
}

using std::begin; using std::end;

#pragma mark - input stream functions
-(void)readBytes
{
  long avail = sb->avail();
  if (avail == 0)
  {
    NSLog(@"no buffer space available for input");
    
    std::istream is{sb.get()};
    std::string bad_stuff;
    is >> bad_stuff;
    
    NSLog(@"maybe choked on: \"%s\"?", bad_stuff.c_str());
    
    return;
  }
  
  long n = [_inputStream read:(uint8_t*)sb->pptr() maxLength:sb->avail()];
  sb->bump_egptr(n);
  
  if (n > 0)
    [self parsePoints];
  else
    NSLog(@"returned zero bytes from inputStream_");

}
-(void)parsePoints
{
  point_cloud::point_data_t points;
  point_cloud::point p;
  std::istream is{sb.get()};
  sb->mark_unget();
  while (is >> p)
  {
    if (is.peek() != '\n')
      break;
    points.emplace_back(p);
    sb->mark_unget();
  }
  
  CGLLockContext((CGLContextObj)[[_glView openGLContext] CGLContextObj]);
  
  add(point_cloud_, points);
  
  CGLUnlockContext((CGLContextObj)[[_glView openGLContext] CGLContextObj]);
  
  _glView.needsDisplay = YES;
}

- (NSInputStream*)inputStreamForFile:(NSString *)path
{
  NSInputStream* s = [[NSInputStream alloc] initWithFileAtPath:path];
  s.delegate = self;
  [s scheduleInRunLoop:[NSRunLoop currentRunLoop]
               forMode:NSDefaultRunLoopMode];
  [s open];
  return s;
}
-(void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode
{
  switch (eventCode)
  {
    case NSStreamEventNone:
      break;
    case NSStreamEventOpenCompleted:
      break;
    case NSStreamEventHasBytesAvailable:
    {
      [self readBytes];
      break;
    }
    case NSStreamEventHasSpaceAvailable:
      break;
    case NSStreamEventErrorOccurred:
    {
      [aStream close];
      [aStream removeFromRunLoop:[NSRunLoop currentRunLoop]
                         forMode:NSDefaultRunLoopMode];
      NSLog(@"NSStreamEventErrorOccurred");
      aStream = nil;
      break;
    }
    case NSStreamEventEndEncountered:
      break;
  }
}



@end
