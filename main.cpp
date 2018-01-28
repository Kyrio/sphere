#include "clock.h"
#include "window.h"

int main() {
    Window window(960, 540, "sphere");

    if (!window.initWayland()) {
        return 1;
    }

    std::cout << "Wayland: All set" << std::endl;
    
    if (!window.initEgl()) {
        window.releaseWayland();
        std::cout << "Wayland: Resources released" << std::endl;
        return 1;
    }

    std::cout << "EGL: All set" << std::endl;

    Clock clock;

    while (window.dispatchPendingEvents() != -1) {
        clock.update();

        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        window.swapBuffers();

        if (clock.sinceStart() > 5)
            break;
    }

    window.releaseEgl();
    std::cout << "EGL: Resources released" << std::endl;

    window.releaseWayland();
    std::cout << "Wayland: Resources released" << std::endl;

    return 0;
}
