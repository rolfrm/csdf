// clang -dynamiclib -framework Cocoa -o libminwindow.dylib minwindow.m

#import "minwindow.h"
#import <Cocoa/Cocoa.h>
#import <AppKit/NSEvent.h>

@interface PixelView : NSView
@property (nonatomic) int width, height;
@property (nonatomic) CGImageRef img;
@end

@implementation PixelView
- (void)drawRect:(NSRect)rect {
	 CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];

	 CGContextSaveGState(ctx); // Save current state
    CGContextTranslateCTM(ctx, 0, self.bounds.size.height);
    CGContextScaleCTM(ctx, 1.0, -1.0);

	 
    CGContextSetInterpolationQuality(ctx, kCGInterpolationNone);
    CGContextDrawImage([[NSGraphicsContext currentContext] CGContext], self.bounds, self.img);
    CGContextRestoreGState(ctx); // Restore original state
}
@end

struct __MiniWindow {
  NSWindow* window;
  PixelView* view;
  bool alive;
  uint32_t * pixels;
  
  void (* on_key_event)(MiniWindow * win, int key, int state);
};

// Exposed C API:
MiniWindow * mw_create_window(int width, int height, uint32_t * framebuffer, const char* title) {
@autoreleasepool {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    __block MiniWindow* mw = malloc(sizeof(MiniWindow));
	 NSRect r = {{100,100},{width * 2,height * 2}};
	 mw->window = [[NSWindow alloc] initWithContentRect:r
											  styleMask:(NSWindowStyleMaskTitled |
                                                      NSWindowStyleMaskClosable |
                                                      NSWindowStyleMaskMiniaturizable |
                                                      NSWindowStyleMaskResizable)
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
	 mw->window.title = [NSString stringWithUTF8String:title ? title : "Window"];
	 mw->view = [PixelView new];
	 mw->view.width = width;
	 mw->view.height = height;
	 mw->view.frame = r;
	 mw->alive = true;
	 
	 [mw->window setContentView:mw->view];
	 [mw->window makeKeyAndOrderFront:nil];
	 [mw->window orderFront:nil];

	 // setup the framebuffer based on the pixel pointer,
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
	 
	 CGDataProviderRef dp = CGDataProviderCreateWithData(0, framebuffer, width * height * 4, 0);
	 mw->view.img = CGImageCreate(width, height, 8, 32, width * 4, cs,
									  kCGBitmapByteOrder32Host | kCGImageAlphaNoneSkipLast, dp, 0, 0, 0);
	 CGColorSpaceRelease(cs);
	 
	 return mw;
 }
}

void mw_close_window(MiniWindow * mw) {
  [mw->window close];
  free(mw);
}

void mw_redraw(MiniWindow * mw) {
  [mw->view setNeedsDisplay:YES];
}

void mw_set_on_key_event(MiniWindow * mw, void (* on_key_event)(MiniWindow * win, int key, int state)){
    mw->on_key_event = on_key_event;
}

enum mac_keys{
    MAC_KEY_UP = 126,
    MAC_KEY_DOWN = 125,
    MAC_KEY_LEFT = 123,
    MAC_KEY_RIGHT = 124,
    MAC_KEY_RETURN = 36,
    MAC_KEY_SPACE = 49,
    MAC_KEY_A = 0,
    MAC_KEY_D = 2,
    MAC_KEY_S = 1,
    MAC_KEY_W = 13,
    MAC_KEY_ESC = 53
};

// Returns 1 if open, 0 if closed.
int mw_process_events(MiniWindow* mw) {
    
    @autoreleasepool {  
    
        for (;;)
        {
            bool handled = false;
            
            NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:[NSDate distantPast]
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES];
            if (event == nil)
                break;
				if ((event.type == NSEventTypeKeyDown || event.type == NSEventTypeKeyUp) && mw->on_key_event) {
                    int type = NSEventTypeKeyDown == event.type ? KEY_DOWN : KEY_UP;
                    handled = true;
                    unsigned short key = [event keyCode];
                    printf("M: KEy %i\n", key);
                    if (key) {
                        switch (key) {
                            case MAC_KEY_ESC: // Escape
                                mw->on_key_event(mw, KEY_ESCAPE, type);
                                break;
                            case MAC_KEY_RETURN: // Enter
                                mw->on_key_event(mw, KEY_ENTER, type);
                                break;    
                            case MAC_KEY_W:
                                mw->on_key_event(mw, KEY_W, type);
                                break;
                            case MAC_KEY_A:
                                mw->on_key_event(mw, KEY_A, type);
                                break;
                            case MAC_KEY_S:
                                mw->on_key_event(mw, KEY_S, type);
                                break;
                            case MAC_KEY_D:
                                mw->on_key_event(mw, KEY_D, type);
                                break;
                            case MAC_KEY_SPACE:
                                mw->on_key_event(mw, KEY_SPACE, type);
                                break;
                            case MAC_KEY_LEFT:
                                mw->on_key_event(mw, KEY_LEFTARROW, type);
                                break;
                            case  MAC_KEY_RIGHT:
                                mw->on_key_event(mw, KEY_RIGHTARROW, type);
                                break;
                            case MAC_KEY_UP:
                                mw->on_key_event(mw, KEY_UPARROW, type);
                                break;
                            case MAC_KEY_DOWN:
                                mw->on_key_event(mw, KEY_DOWNARROW, type);
                                break;
                            default:
                                break;
                        }
                    }
                    }
                    if(!handled)
                    [NSApp sendEvent:event];
        }
            [NSApp updateWindows];
    
        } // autoreleasepool
    return mw->alive;
}
