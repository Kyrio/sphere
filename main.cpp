#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdint>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>

using u32 = std::uint32_t;

wl_compositor* compositor;
wl_shell* shell;

void globalAdd(void* data, wl_registry* registry, u32 name, const char* interface, u32 version) {
    if (std::strcmp(interface, wl_compositor_interface.name) == 0) {
        compositor = (wl_compositor*) wl_registry_bind(registry, name, &wl_compositor_interface, version);
    }
    else if (std::strcmp(interface, wl_shell_interface.name) == 0) {
        shell = (wl_shell*) wl_registry_bind(registry, name, &wl_shell_interface, version);
    }
}

void globalRemove(void* data, wl_registry* registry, u32 name) {
    std::cout << "Removed global " << name << std::endl;
}

wl_registry_listener registryListener = { globalAdd, globalRemove };

int main() {
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

    wl_shell_surface_set_toplevel(shellSurface);

    std::cout << "Wayland: All set" << std::endl;

    EGLDisplay eglDisplay = eglGetDisplay(display);
    EGLBoolean success = eglInitialize(eglDisplay, nullptr, nullptr);

    if (success != EGL_TRUE) {
        std::cerr << "Cannot initialize EGL display" << std::endl;
        return 1;
    }

    eglTerminate(eglDisplay);
    wl_display_disconnect(display);

    return 0;
}
