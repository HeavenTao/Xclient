#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

int main(int argc, char **argv) {

    int screenNum;
    xcb_connection_t *connect = xcb_connect("192.168.3.3:0.0", &screenNum);

    if (xcb_connection_has_error(connect)) {
        printf("connection");
        return 1;
    }

    xcb_screen_iterator_t it = xcb_setup_roots_iterator(xcb_get_setup(connect));
    xcb_screen_t *screen = it.data;

    xcb_window_t window = xcb_generate_id(connect);
    xcb_create_window(connect, XCB_COPY_FROM_PARENT, window, screen->root, 1000,
                      0, 800, 400, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, 0, NULL);

    xcb_map_window(connect, window);

    xcb_flush(connect);

    xcb_generic_event_t *event;

    while ((event = xcb_wait_for_event(connect))) {
        switch (event->response_type & 0x80) {
        case XCB_EXPOSE:
            break;
        default:
            break;
        }

        free(event);
    }

    xcb_disconnect(connect);

    return 0;
}
