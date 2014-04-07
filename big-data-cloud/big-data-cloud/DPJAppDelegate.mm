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
  std::array<char, 1024> buf_;
  char* bufp_;
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
  bufp_ = buf_.data();
}

using std::begin; using std::end;

#pragma mark - input stream functions
-(void)readBytes
{
  long avail_out = static_cast<long>(buf_.size() -(bufp_ - buf_.data()));
  long len = [_inputStream read:(uint8_t*)bufp_ maxLength:avail_out];
  
  std::array<char, 3> ws{'\n', '\t', ' '};
  if (len > 0)
  {
    bufp_ += len;

    auto it = std::find_first_of(begin(buf_), bufp_, begin(ws), end(ws));

    if (it != end(buf_))
    {
      std::string s{begin(buf_), it};
      
    
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
}
point_cloud::point_data_t parsePoints(std::istream& is)
{
  point_cloud::point_data_t points;
  point_cloud::point p;
  while (is >> p)
  {
    if (p.x < pc.mins.x)
      pc.mins.x = p.x;
    else if (pc.maxs.x < p.x)
      pc.maxs.x = p.x;
    pc.sums.x += p.x;
    
    if (p.y < pc.mins.y)
      pc.mins.y = p.y;
    else if (pc.maxs.y < p.y)
      pc.maxs.y = p.y;
    pc.sums.y += p.y;
    
    if (p.z < pc.mins.z)
      pc.mins.z = p.z;
    else if (pc.maxs.z < p.z)
      pc.maxs.z = p.z;
    pc.sums.z += p.z;
    points.emplace_back(p);
  }
  return points;
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
