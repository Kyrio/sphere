#include <GLES3/gl3.h>

#include "clock.h"
#include "window.h"

int main() {
    Window window(960, 540, "sphere");

    if (!window.initWayland()) {
        return 1;
    }
    
    if (!window.initEgl()) {
        window.releaseWayland();
        return 1;
    }

    while (window.isRunning() && window.dispatchPendingEvents() != -1) {
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        window.swapBuffers();
    }

    std::cout << "Cleaning up..." << std::endl;

    window.releaseEgl();
    window.releaseWayland();

    return 0;
}
