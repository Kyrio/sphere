#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdint>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

using u32 = std::uint32_t;
using i32 = std::int32_t;

class Window {
    public:
        Window(int width, int height, std::string windowClass);
        ~Window();

        bool initWayland();
        bool initEgl();
        void releaseEgl();
        void releaseWayland();

        void swapBuffers();
        int dispatchPendingEvents();

        void setCompositor(wl_compositor* compositor);
        void setShell(wl_shell* shell);
        void setSeat(wl_seat* seat);

        void setKeyboard(wl_keyboard* keyboard);
        void releaseKeyboard();

        bool isRunning();
        void setRunning(bool running);

        void resize(int width, int height);

        static void registryGlobal(void* data, wl_registry* registry, u32 name, const char* interface, u32 version);
        static void registryRemove(void* data, wl_registry* registry, u32 name);

        static void surfacePing(void* data, wl_shell_surface* shellSurface, u32 serial);
        static void surfaceConfigure(void* data, wl_shell_surface* shellSurface, u32 edges, i32 width, i32 height);
        static void surfacePopupDone(void* data, wl_shell_surface* shellSurface) {}

        static void seatCapabilities(void* data, wl_seat* seat, u32 capabilities);
        static void seatName(void* data, wl_seat* seat, const char* name) {}

        static void keyboardKeymap(void* data, wl_keyboard* keyboard, u32 format, i32 fd, u32 size) {}
        static void keyboardEnter(void* data, wl_keyboard* keyboard, u32 serial, wl_surface* surface, wl_array* keys) {}
        static void keyboardLeave(void* data, wl_keyboard* keyboard, u32 serial, wl_surface* surface) {}
        static void keyboardKey(void* data, wl_keyboard* keyboard, u32 serial, u32 time, u32 key, u32 state);
        static void keyboardModifiers(void* data, wl_keyboard* keyboard, u32 serial,
                                      u32 mods_depressed, u32 mods_latched, u32 mods_locked,
                                      u32 group) {}
        static void keyboardRepeatInfo(void* data, wl_keyboard* keyboard, i32 rate, i32 delay) {}

        static wl_registry_listener registryListener;
        static wl_shell_surface_listener shellSurfaceListener;
        static wl_seat_listener seatListener;
        static wl_keyboard_listener keyboardListener;

    private:
        int width;
        int height;
        bool running;
        std::string windowClass;

        wl_compositor* compositor;
        wl_shell* shell;
        wl_seat* seat;
        wl_keyboard* keyboard;

        wl_display* display;
        wl_registry* registry;
        wl_surface* surface;
        wl_shell_surface* shellSurface;
        wl_egl_window* window;

        EGLDisplay eglDisplay;
        EGLConfig eglConfig;
        EGLContext eglContext;
        EGLSurface eglSurface;

        static EGLint eglAttributes[];
        static EGLint eglContextAttributes[];
};

#endif // WINDOW_H
