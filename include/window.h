#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdint>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>

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

        void resize(int width, int height);

        static void registryAdd(void* data, wl_registry* registry, u32 name, const char* interface, u32 version);
        static void registryRemove(void* data, wl_registry* registry, u32 name);

        static void surfacePing(void* data, wl_shell_surface* shellSurface, u32 serial);
        static void surfaceConfigure(void* data, wl_shell_surface* shellSurface, u32 edges, i32 width, i32 height);
        static void surfacePopupDone(void* data, wl_shell_surface* shellSurface) {}

        static wl_registry_listener registryListener;
        static wl_shell_surface_listener shellSurfaceListener;

    private:
        int width;
        int height;
        std::string windowClass;

        wl_compositor* compositor;
        wl_shell* shell;
        wl_seat* seat;

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
};

#endif // WINDOW_H
