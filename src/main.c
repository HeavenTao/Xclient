#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

void set_window_type(xcb_connection_t *connect, xcb_window_t window) {
    const char *name = "_NET_WM_WINDOW_TYPE";
    xcb_intern_atom_cookie_t cookie =
        xcb_intern_atom(connect, 1, strlen(name), name);

    xcb_intern_atom_reply_t *reply =
        xcb_intern_atom_reply(connect, cookie, NULL);
    if (!reply)
        return;

    xcb_atom_t net_wm_window_type = reply->atom;
    free(reply);
    xcb_intern_atom_cookie_t normal_cookie =
        xcb_intern_atom(connect, 0, 22, "_NET_WM_WINDOW_TYPE_NORMAL");
    xcb_intern_atom_reply_t *normal_reply =
        xcb_intern_atom_reply(connect, normal_cookie, NULL);
    if (!normal_reply)
        return;

    xcb_atom_t net_wm_window_type_normal = normal_reply->atom;
    free(normal_reply);

    xcb_change_property(connect, XCB_PROP_MODE_REPLACE, window,
                        net_wm_window_type, XCB_ATOM_ATOM, 32, 1,
                        &net_wm_window_type_normal);
}

int main(int argc, char **argv) {

    int screenNum;
    /*xcb_connection_t *connect = xcb_connect(NULL, &screenNum);*/
    xcb_connection_t *connect = xcb_connect("192.168.3.3:0.0", &screenNum);

    if (xcb_connection_has_error(connect)) {
        printf("connection error\n");
        return 1;
    }

    /*get Screen*/
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(xcb_get_setup(connect));
    xcb_screen_t *screen = it.data;

    /*create gc*/
    xcb_gcontext_t gc = xcb_generate_id(connect);
    uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t gc_values[] = {screen->black_pixel, 0};
    xcb_create_gc(connect, gc, screen->root, gc_mask, gc_values);

    /*create window*/
    uint32_t window_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t window_values[] = {screen->white_pixel,
                                XCB_EVENT_MASK_EXPOSURE |
                                    XCB_EVENT_MASK_BUTTON_PRESS};
    xcb_window_t window = xcb_generate_id(connect);
    xcb_create_window(connect, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0,
                      800, 400, 5, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, window_mask, window_values);
    printf("window1 id is %d\n", window);

    /*create 2window*/
    uint32_t window2_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t window2_values[] = {screen->white_pixel,
                                 XCB_EVENT_MASK_EXPOSURE |
                                     XCB_EVENT_MASK_BUTTON_PRESS};
    xcb_window_t window2 = xcb_generate_id(connect);
    xcb_create_window(connect, XCB_COPY_FROM_PARENT, window2, screen->root, 800,
                      0, 200, 200, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, window2_mask, window2_values);

    /*change window property*/
    char *title = "hello world";
    xcb_change_property(connect, XCB_PROP_MODE_REPLACE, window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title),
                        title);
    set_window_type(connect, window);
    /*map window*/
    xcb_map_window(connect, window);
    xcb_map_window(connect, window2);
    xcb_flush(connect);

    const xcb_rectangle_t rect[] = {{0, 0, 100, 100}, {100, 100, 100, 100}};

    int x = 0;
    int y = 0;
    /*event loop*/
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connect))) {
        switch (event->response_type & ~0x80) {
        case XCB_EXPOSE: {
            xcb_expose_event_t *expose = (xcb_expose_event_t *)event;
            xcb_poly_rectangle(connect, window, gc, 2, rect);
            xcb_flush(connect);
            break;
        }
        case XCB_BUTTON_PRESS: {
            xcb_button_press_event_t *press = (xcb_button_press_event_t *)event;
            printf("PRESS");
            if (press->event == window2) {
                /*x = x + 10;*/
                /*y = y + 10;*/
                /*uint32_t config_mask =*/
                /*XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;*/
                /*uint32_t config_values[] = {x, y};*/
                /*xcb_configure_window(connect, window, config_mask,*/
                /*config_values);*/
                xcb_unmap_window(connect, window2);
            } else {
                xcb_map_window(connect, window2);
            }
            xcb_flush(connect);
            break;
        }
        case XCB_BUTTON_RELEASE: {
            printf("XCB_BUTTON_RELEASE\n");
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
