#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct __MiniWindow MiniWindow;
  MiniWindow* mw_create_window(int width, int height, uint32_t * framebuffer, const char* title); // Returns a window handle (pointer)
    void  mw_close_window(MiniWindow* w);
    void  mw_redraw(MiniWindow* w);
    int   mw_process_events(MiniWindow *); // Returns 0 if window was closed, 1 if still open
    void mw_set_on_key_event(MiniWindow * mw, void (* on_key_event)(MiniWindow * win, int key, int state));
  const uint32_t * mw_get_framebuffer(MiniWindow * mw);
    
enum key_event_key {
        KEY_ESCAPE,
        KEY_LEFTARROW,
    KEY_RIGHTARROW,
    KEY_UPARROW,
    KEY_DOWNARROW,
        KEY_W,
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_SPACE,
KEY_ENTER, KEY_CTRL    
    };

    enum key_event_type {
        KEY_DOWN,
        KEY_UP
    };


#ifdef __cplusplus
}
#endif
