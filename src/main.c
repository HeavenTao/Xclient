#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

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
                      800, 400, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, window_mask, window_values);
    printf("window1 id is %d\n", window);

    /*create 2window*/
    uint32_t window2_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t window2_values[] = {screen->white_pixel,
                                 XCB_EVENT_MASK_EXPOSURE |
                                     XCB_EVENT_MASK_BUTTON_PRESS};
    xcb_window_t window2 = xcb_generate_id(connect);
    xcb_create_window(connect, XCB_COPY_FROM_PARENT, window2, screen->root, 800,
                      0, 200, 200, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, window2_mask, window2_values);

    /*change window property*/
    char *title = "hello world";
    xcb_change_property(connect, XCB_PROP_MODE_REPLACE, window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title),
                        title);

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
