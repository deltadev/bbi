#include <sys/stat.h> // for fifo

#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>

#import "DPJAppDelegate.h"

#include "gl.hh"

@interface DPJAppDelegate () <NSStreamDelegate>
{
  CFSocketRef _sock;
  CFSocketNativeHandle _sock_nat;
  
}
@property NSInputStream* inputStream;
@property NSTextView* textView;
@end

@implementation DPJAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  NSView* v = _window.contentView;
  _textView = [[NSTextView alloc] initWithFrame:v.bounds];
  _window.contentView = _textView;


  // FIFO for testing.
  //
  NSString* fifo = [@"~/fifos/gl" stringByStandardizingPath];
  if (mkfifo([fifo UTF8String], 0644) == -1)
    NSLog(@"error creating fifo: %s", strerror(errno));
  
  _inputStream = [self inputStreamForFile:fifo];


}



#pragma mark - input stream functions
-(void)readBytes
{
  std::array<uint8_t, 1024> buf{0};
  unsigned long len = [_inputStream read:buf.data() maxLength:buf.size()-1];
  if (len > 0)
  {
    NSString* os = [NSString stringWithUTF8String:(char*)buf.data()];
    if ([_textView.string length] > 2048)
      _textView.string = @"";
    [_textView insertText:os];
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
