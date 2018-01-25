#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdint>

#include <wayland-client.h>
#include <wayland-egl.h>

using u32 = std::uint32_t;

wl_compositor* compositor;
wl_shell* shell;

void global_add(void* data, wl_registry* registry, u32 name, const char* interface, u32 version) {
    if (std::strcmp(interface, wl_compositor_interface.name) == 0) {
        compositor = (wl_compositor*) wl_registry_bind(registry, name, &wl_compositor_interface, version);
    }
    else if (std::strcmp(interface, wl_shell_interface.name) == 0) {
        shell = (wl_shell*) wl_registry_bind(registry, name, &wl_shell_interface, version);
    }
}

void global_remove(void* data, wl_registry* registry, u32 name) {
    std::cout << "Removed global " << name << std::endl;
}

wl_registry_listener registry_listener = { global_add, global_remove };

int main() {
    wl_display* display = wl_display_connect(nullptr);
    if (display == nullptr) {
        std::cerr << "Can't connect to Wayland display: " << std::strerror(errno) << std::endl;
        return 1;
    }

    wl_registry* registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, nullptr);

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

    wl_shell_surface* shell_surface = wl_shell_get_shell_surface(shell, surface);

    if (shell_surface == nullptr) {
        std::cerr << "Cannot create Wayland shell surface" << std::endl;
        return 1;
    }

    wl_shell_surface_set_toplevel(shell_surface);

    std::cout << "All set" << std::endl;

    wl_display_disconnect(display);

    return 0;
}
