#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdint>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>

#include "clock.h"

using u32 = std::uint32_t;
using i32 = std::int32_t;

wl_compositor* compositor;
wl_shell* shell;
wl_seat* seat;

void globalAdd(void* data, wl_registry* registry, u32 name, const char* interface, u32 version) {
    // std::cout << "Added global " << name << ": " << interface << std::endl;
    if (std::strcmp(interface, wl_compositor_interface.name) == 0) {
        compositor = (wl_compositor*) wl_registry_bind(registry, name, &wl_compositor_interface, version);
    }
    else if (std::strcmp(interface, wl_shell_interface.name) == 0) {
        shell = (wl_shell*) wl_registry_bind(registry, name, &wl_shell_interface, version);
    }
    else if (std::strcmp(interface, wl_seat_interface.name) == 0) {
        seat = (wl_seat*) wl_registry_bind(registry, name, &wl_seat_interface, version);
    }
}

void globalRemove(void* data, wl_registry* registry, u32 name) {
    // std::cout << "Removed global " << name << std::endl;
}

void surfacePing(void* data, wl_shell_surface* shellSurface, u32 serial) {
    std::cout << "Ping" << std::endl;
    wl_shell_surface_pong(shellSurface, serial);
}

void surfaceConfigure(void* data, wl_shell_surface* shellSurface, u32 edges, i32 width, i32 height) {
    std::cout << "Configure" << std::endl;
}

void surfacePopupDone(void* data, wl_shell_surface* shellSurface) {
    std::cout << "Popup" << std::endl;
}

wl_registry_listener registryListener = { globalAdd, globalRemove };
wl_shell_surface_listener shellSurfaceListener = { surfacePing, surfaceConfigure, surfacePopupDone };

int main(int argc, char* argv[]) {
    wl_display* display = wl_display_connect(nullptr);

    if (display == nullptr) {
        std::cerr << "Can't connect to Wayland display: " << std::strerror(errno) << std::endl;
        return 1;
    }

    wl_registry* registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registryListener, nullptr);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    if (compositor == nullptr) {
        std::cerr << "Wayland compositor not found" << std::endl;
        return 1;
    }

    if (shell == nullptr) {
        std::cerr << "Wayland shell not found" << std::endl;
        return 1;
    }

    if (seat == nullptr) {
        std::cerr << "Wayland seat not found" << std::endl;
        return 1;
    }

    wl_surface* surface = wl_compositor_create_surface(compositor);

    if (surface == nullptr) {
        std::cerr << "Cannot create Wayland surface" << std::endl;
        return 1;
    }

    wl_shell_surface* shellSurface = wl_shell_get_shell_surface(shell, surface);

    if (shellSurface == nullptr) {
        std::cerr << "Cannot create Wayland shell surface" << std::endl;
        return 1;
    }

    wl_shell_surface_add_listener(shellSurface, &shellSurfaceListener, nullptr);
    wl_shell_surface_set_class(shellSurface, "sphere");
    wl_shell_surface_set_toplevel(shellSurface);

    std::cout << "Wayland: All set" << std::endl;

    eglBindAPI(EGL_OPENGL_API);
    EGLDisplay eglDisplay = eglGetDisplay(display);
    EGLBoolean success = eglInitialize(eglDisplay, nullptr, nullptr);

    if (success != EGL_TRUE) {
        std::cerr << "Cannot initialize EGL display" << std::endl;
        return 1;
    }

    EGLint attributes[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_configs;

    success = eglChooseConfig(eglDisplay, attributes, &config, 1, &num_configs);

    if (success != EGL_TRUE) {
        std::cerr << "Cannot configure EGL display" << std::endl;
        return 1;
    }

    EGLContext context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, nullptr);

    if (context == EGL_NO_CONTEXT) {
        std::cerr << "Cannot create EGL context" << std::endl;
        return 1;
    }

    wl_egl_window* window = wl_egl_window_create(surface, 960, 540);
    EGLSurface windowSurface = eglCreateWindowSurface(eglDisplay, config, window, nullptr);

    if (windowSurface == EGL_NO_SURFACE) {
        std::cerr << "Cannot create EGL window surface" << std::endl;
        return 1;
    }

    success = eglMakeCurrent(eglDisplay, windowSurface, windowSurface, context);

    if (success != EGL_TRUE) {
        std::cerr << "Cannot bind EGL context" << std::endl;
        return 1;
    }

    std::cout << "EGL: All set" << std::endl;

    // TODO Key events, resize, title (?)

    Clock clock;

    while (wl_display_dispatch_pending(display) != -1) {
        clock.update();

        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        eglSwapBuffers(eglDisplay, windowSurface);

        if (clock.sinceStart() > 5)
            break;
    }

    eglDestroySurface(eglDisplay, windowSurface);
    wl_egl_window_destroy(window);
    eglDestroyContext(display, context);
    eglTerminate(eglDisplay);

    std::cout << "EGL: Resources released" << std::endl;

    wl_shell_surface_destroy(shellSurface);
    wl_surface_destroy(surface);
    wl_display_disconnect(display);

    std::cout << "Wayland: Resources released" << std::endl;

    return 0;
}
