#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <stdio.h>

int main(int argc, char **argv) {
    Display *dpy = XOpenDisplay("192.168.3.3:0.0");
    if (dpy == NULL) {
        printf("Can't connect to display\n");
        return 1;
    }

    Screen *screen = XDefaultScreenOfDisplay(dpy);

    Visual *visual = XDefaultVisualOfScreen(screen);
    int depth = XDefaultDepthOfScreen(screen);

    Window root = XRootWindowOfScreen(screen);

    int x = 0, y = 0;
    Window window = XCreateWindow(dpy, root, x, y, 400, 400, 1, CopyFromParent,
                                  InputOutput, CopyFromParent, 0, NULL);

    XStoreName(dpy, window, "hello world");

    XSetWindowBackground(dpy, window, BlackPixel(dpy, 0));

    XSelectInput(dpy, window, ExposureMask | KeyPressMask);

    XMapWindow(dpy, window);

    XEvent event;
    while (1) {
        XNextEvent(dpy, &event);
        if (event.type == Expose) {
        } else if (event.type == KeyPress) {
            x = x + 10;
            y = y + 10;
            XMoveWindow(dpy, window, x, y);
        }
    }

    XDestroyWindow(dpy, window);

    XCloseDisplay(dpy);

    return 0;
}
