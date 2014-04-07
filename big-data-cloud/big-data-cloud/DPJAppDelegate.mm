#include <sys/stat.h> // for fifo

#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sstream>

#import "DPJAppDelegate.h"
#import "DPJGLView.h"
#include "gl.hh"

#include "point_cloud.hh"

@interface DPJAppDelegate () <NSStreamDelegate, DPJGLDelegate>
{
  point_cloud pc_;
  std::array<char, 1024> read_buf;
  long buf_first;
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
      gl_PointSize = 3;
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
  
  pc_ = point_cloud{prog};
}

- (void)glLayoutChanged:(DPJGLView *)view { }

- (void)draw
{
  draw(pc_, *_glView->renderer);
}

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
  buf_first = 0;
}



#pragma mark - input stream functions
-(void)readBytes
{
  char* first = read_buf.data() + buf_first;
  
  unsigned long len = [_inputStream read:first maxLength:buf.size() - buf_first];
  if (len > 0)
  {
    std::string s{first, first + len};
    NSString* os = [NSString stringWithUTF8String:s.c_str()];
    if ([_textView.string length] > 128)
      _textView.string = os;

    [_textView insertText:os];

    std::istringstream iss{s};
    CGLLockContext((CGLContextObj)[[_glView openGLContext] CGLContextObj]);
    add(pc_, iss);
    CGLUnlockContext((CGLContextObj)[[_glView openGLContext] CGLContextObj]);

    
    _glView.needsDisplay = YES;
    
  }
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
  NSString* s;
  
  switch (eventCode)
  {
    case NSStreamEventNone:
      s = @"NSStreamEventNone";
      break;
    case NSStreamEventOpenCompleted:
      s = @"NSStreamEventOpenCompleted";
      break;
    case NSStreamEventHasBytesAvailable:
    {
      s = @"NSStreamEventHasBytesAvailable";
      [self readBytes];
      break;
    }
    case NSStreamEventHasSpaceAvailable:
      s = @"NSStreamEventHasSpaceAvailable";
      break;
    case NSStreamEventErrorOccurred:
    {
      s = @"NSStreamEventErrorOccurred";
      [aStream close];
      [aStream removeFromRunLoop:[NSRunLoop currentRunLoop]
                         forMode:NSDefaultRunLoopMode];
      NSLog(@"istream: %@", s);
      aStream = nil;
      break;
    }
    case NSStreamEventEndEncountered:
      s = @"NSStreamEventEndEncountered";
      break;
  }
  
  NSLog(@"istream: %@", s);
}



@end
