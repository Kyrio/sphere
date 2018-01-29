#include "clock.h"
#include "window.h"

int main() {
    Window window(960, 540, "sphere");

    if (!window.initWayland()) {
        return 1;
    }
    
    if (!window.initEgl()) {
        window.releaseWayland();
        std::cout << "Wayland: Resources released" << std::endl;
        return 1;
    }

    while (window.isRunning() && window.dispatchPendingEvents() != -1) {
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        window.swapBuffers();
    }

    window.releaseEgl();
    std::cout << "EGL: Resources released" << std::endl;

    window.releaseWayland();
    std::cout << "Wayland: Resources released" << std::endl;

    return 0;
}
