#include <stdio.h>
#include <stdlib.h>
#include <xcb/composite.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_util.h>
#include <xcb/xcbext.h>
#include <xcb/xproto.h>

int checkOtherWm(xcb_connection_t *connect, xcb_screen_t *screen) {
    uint32_t mask = {XCB_CW_EVENT_MASK};
    uint32_t values[] = {XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};

    xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(
        connect, screen->root, mask, values);
    xcb_generic_error_t *err = xcb_request_check(connect, cookie);
    if (err) {
        free(err);
        printf("has wm runing");
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char **argv) {
    int screenNum;
    xcb_connection_t *connect = xcb_connect("192.168.3.3:0.0", &screenNum);
    if (xcb_connection_has_error(connect)) {
        printf("connection error");
        return 1;
    }

    xcb_screen_t *screen = xcb_aux_get_screen(connect, screenNum);

    int result = checkOtherWm(connect, screen);

    printf("result is %d", result);

    xcb_disconnect(connect);
    return 0;
}
