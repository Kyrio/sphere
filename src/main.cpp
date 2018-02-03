#include <GLES3/gl3.h>

#include "clock.h"
#include "window.h"
#include "shader.h"

int main() {
    Window window(720, 540, "sphere");

    if (!window.initWayland()) {
        return 1;
    }
    
    if (!window.initEgl()) {
        window.releaseWayland();
        return 1;
    }

    Shader vs(GL_VERTEX_SHADER, "shaders/vertex.glsl");
    Shader fs(GL_FRAGMENT_SHADER, "shaders/fragment.glsl");

    ShaderProgram program;
    program << vs << fs;
    program.link();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };

    unsigned int vertexArray;
    unsigned int vertexBuffer;
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);
    
    glBindVertexArray(vertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while (window.isRunning() && window.dispatchPendingEvents() != -1) {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        window.swapBuffers();
    }

    std::cout << "Cleaning up..." << std::endl;

    window.releaseEgl();
    window.releaseWayland();

    return 0;
}
