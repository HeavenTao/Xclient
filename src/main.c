#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

int main(int argc, char **argv) {

    int screenNum;
    /*xcb_connection_t *connect = xcb_connect(NULL, &screenNum);*/
    xcb_connection_t *connect = xcb_connect("192.168.3.3:0.0", &screenNum);

    if (xcb_connection_has_error(connect)) {
        printf("connection error");
        return 1;
    }

    xcb_screen_iterator_t it = xcb_setup_roots_iterator(xcb_get_setup(connect));
    xcb_screen_t *screen = it.data;

    xcb_gcontext_t gc = xcb_generate_id(connect);
    uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t gc_values[] = {screen->black_pixel, 0};

    xcb_create_gc(connect, gc, screen->root, gc_mask, gc_values);

    uint32_t window_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t window_values[] = {screen->white_pixel,
                                XCB_EVENT_MASK_EXPOSURE |
                                    XCB_EVENT_MASK_BUTTON_PRESS};
    xcb_window_t window = xcb_generate_id(connect);
    xcb_create_window(connect, XCB_COPY_FROM_PARENT, window, screen->root, 1000,
                      0, 800, 400, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, window_mask, window_values);

    xcb_map_window(connect, window);
    xcb_flush(connect);

    const xcb_rectangle_t rect[] = {{0, 0, 100, 100}, {100, 100, 100, 100}};

    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connect))) {
        switch (event->response_type & ~0x80) {
        case XCB_EXPOSE: {
            printf("XCB_EXPOSE");
            xcb_poly_rectangle(connect, window, gc, 2, rect);
            xcb_flush(connect);
            break;
        }
        case XCB_BUTTON_PRESS: {
            printf("XCB_BUTTON_PRESS");
            break;
        }
        default:
            break;
        }

        free(event);
    }

    xcb_disconnect(connect);

    return 0;
}
