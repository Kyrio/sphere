#include <linux/input.h>

#include "window.h"

Window::Window(int width, int height, std::string windowClass) :
    width(width),
    height(height),
    running(false),
    windowClass(windowClass)
{
}

Window::~Window() {
}

bool Window::initWayland() {
    display = wl_display_connect(nullptr);

    if (display == nullptr) {
        std::cerr << "Can't connect to Wayland display: " << std::strerror(errno) << std::endl;
        return false;
    }

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registryListener, this);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    if (compositor == nullptr) {
        std::cerr << "Wayland compositor not found" << std::endl;
        return false;
    }

    if (shell == nullptr) {
        std::cerr << "Wayland shell not found" << std::endl;
        return false;
    }

    if (seat == nullptr) {
        std::cerr << "Wayland seat not found" << std::endl;
        return false;
    }

    surface = wl_compositor_create_surface(compositor);

    if (surface == nullptr) {
        std::cerr << "Cannot create Wayland surface" << std::endl;
        return false;
    }

    shellSurface = wl_shell_get_shell_surface(shell, surface);

    if (shellSurface == nullptr) {
        std::cerr << "Cannot create Wayland shell surface" << std::endl;
        return false;
    }

    wl_shell_surface_add_listener(shellSurface, &shellSurfaceListener, this);
    wl_shell_surface_set_class(shellSurface, windowClass.c_str());
    wl_shell_surface_set_toplevel(shellSurface);

    window = wl_egl_window_create(surface, width, height);

    return true;
}

bool Window::initEgl() {
    eglBindAPI(EGL_OPENGL_ES_API);

    eglDisplay = eglGetDisplay(display);
    EGLBoolean success = eglInitialize(eglDisplay, nullptr, nullptr);

    if (success != EGL_TRUE) {
        std::cerr << "Cannot initialize EGL display" << std::endl;
        return false;
    }

    EGLint num_configs;
    success = eglChooseConfig(eglDisplay, eglAttributes, &eglConfig, 1, &num_configs);

    if (success != EGL_TRUE) {
        std::cerr << "Cannot configure EGL display" << std::endl;
        return false;
    }

    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, eglContextAttributes);

    if (eglContext == EGL_NO_CONTEXT) {
        std::cerr << "Cannot create EGL context" << std::endl;
        return false;
    }

    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, window, nullptr);

    if (eglSurface == EGL_NO_SURFACE) {
        std::cerr << "Cannot create EGL window surface" << std::endl;
        return false;
    }

    success = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    if (success != EGL_TRUE) {
        std::cerr << "Cannot bind EGL context" << std::endl;
        return false;
    }

    running = true;

    return true;
}

void Window::releaseEgl() {
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(eglDisplay, eglSurface);
    eglDestroyContext(display, eglContext);
    eglTerminate(eglDisplay);
}

void Window::releaseWayland() {
    wl_egl_window_destroy(window);
    wl_shell_surface_destroy(shellSurface);
    wl_surface_destroy(surface);

    releaseKeyboard();
    wl_seat_release(seat);
    wl_shell_destroy(shell);
    wl_compositor_destroy(compositor);

    wl_registry_destroy(registry);
    wl_display_disconnect(display);
}

void Window::swapBuffers() {
    eglSwapBuffers(eglDisplay, eglSurface);
}

int Window::dispatchPendingEvents() {
    return wl_display_dispatch_pending(display);
}

void Window::setCompositor(wl_compositor* compositor) {
    this->compositor = compositor;
}

void Window::setShell(wl_shell* shell) {
    this->shell = shell;
}

void Window::setSeat(wl_seat* seat) {
    this->seat = seat;
    wl_seat_add_listener(seat, &seatListener, this);
}

void Window::setKeyboard(wl_keyboard* keyboard) {
    this->keyboard = keyboard;
    wl_keyboard_add_listener(keyboard, &keyboardListener, this);
}

void Window::releaseKeyboard() {
    if (keyboard != nullptr) {
        wl_keyboard_release(keyboard);
        keyboard = nullptr;
    }
}

bool Window::isRunning() {
    return running;
}

void Window::setRunning(bool running) {
    this->running = running;
}

void Window::resize(int width, int height) {
    // TODO Set viewport
    this->width = width;
    this->height = height;
    wl_egl_window_resize(window, width, height, 0, 0);
}

void Window::registryGlobal(void* data, wl_registry* registry, u32 name, const char* interface, u32 version) {
    if (data == nullptr)
        return;
    
    Window* window = (Window*) data;

    if (std::strcmp(interface, wl_compositor_interface.name) == 0) {
        wl_compositor* compositor = (wl_compositor*) wl_registry_bind(registry, name, &wl_compositor_interface, version);
        window->setCompositor(compositor);
    }
    else if (std::strcmp(interface, wl_shell_interface.name) == 0) {
        wl_shell* shell = (wl_shell*) wl_registry_bind(registry, name, &wl_shell_interface, version);
        window->setShell(shell);
    }
    else if (std::strcmp(interface, wl_seat_interface.name) == 0) {
        wl_seat* seat = (wl_seat*) wl_registry_bind(registry, name, &wl_seat_interface, version);
        window->setSeat(seat);
    }
}

void Window::registryRemove(void* data, wl_registry* registry, u32 name) {
}

void Window::surfacePing(void* data, wl_shell_surface* shellSurface, u32 serial) {
    wl_shell_surface_pong(shellSurface, serial);
}

void Window::surfaceConfigure(void* data, wl_shell_surface* shellSurface, u32 edges, i32 width, i32 height) {
    if (data == nullptr)
        return;
    
    Window* window = (Window*) data;
    window->resize(width, height);
}

void Window::seatCapabilities(void* data, wl_seat* seat, u32 capabilities) {
    if (data == nullptr)
        return;
    
    Window* window = (Window*) data;

    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        wl_keyboard* keyboard = wl_seat_get_keyboard(seat);
        window->setKeyboard(keyboard);
    }
    else {
        window->releaseKeyboard();
    }
}

void Window::keyboardKey(void* data, wl_keyboard* keyboard, u32 serial, u32 time, u32 key, u32 state) {
    if (data == nullptr)
        return;
    
    Window* window = (Window*) data;
    
    if (key == KEY_ESC && state == WL_KEYBOARD_KEY_STATE_RELEASED)
        window->setRunning(false);
}

wl_registry_listener Window::registryListener = {
    Window::registryGlobal,
    Window::registryRemove
};

wl_shell_surface_listener Window::shellSurfaceListener = {
    Window::surfacePing,
    Window::surfaceConfigure,
    Window::surfacePopupDone
};

wl_seat_listener Window::seatListener = {
    Window::seatCapabilities,
    Window::seatName
};

wl_keyboard_listener Window::keyboardListener = {
    Window::keyboardKeymap,
    Window::keyboardEnter,
    Window::keyboardLeave,
    Window::keyboardKey,
    Window::keyboardModifiers,
    Window::keyboardRepeatInfo
};

EGLint Window::eglAttributes[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
    EGL_NONE
};

EGLint Window::eglContextAttributes[] = {
    EGL_CONTEXT_CLIENT_VERSION, 3,
    EGL_NONE
};
