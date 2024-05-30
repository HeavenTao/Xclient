#include <stdio.h>
#include <xcb/composite.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xproto.h>

int main(int argc, char **argv) {
    int screenNum;
    xcb_connection_t *connect = xcb_connect(NULL, &screenNum);
    if (xcb_connection_has_error(connect)) {
        printf("connection error\n");
        return 1;
    }

    xcb_screen_t *screen = xcb_aux_get_screen(connect, screenNum);

    xcb_ewmh_init_atoms(connect, xcb_ewmh_connection_t * ewmh)

        xcb_disconnect(connect);

    return 0;
}
