#include <GLES3/gl32.h>

#include "clock.h"
#include "window.h"
#include "shader.h"

int main() {
    Window window(720, 540, "sphere", "Sphere Window");

    if (!window.initWayland()) {
        return 1;
    }
    
    if (!window.initEgl()) {
        window.releaseWayland();
        return 1;
    }

    Shader vs(GL_VERTEX_SHADER, "shaders/vertex.glsl");
    Shader fs(GL_FRAGMENT_SHADER, "shaders/fragment.glsl");
    Shader gs(GL_GEOMETRY_SHADER, "shaders/wireframe.glsl");

    ShaderProgram program;
    program << vs << gs << fs;
    program.link();

    float vertices[] = {
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    unsigned int vertexArray;
    unsigned int vertexBuffer;
    unsigned int indexBuffer;

    // Create arrays
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);
    
    // Bind the vertex array that we will use when drawing
    glBindVertexArray(vertexArray);

    // Fill the buffers with vertices and indices
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the vertex shader attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Release the vertex buffer and the vertex array (for now)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (window.isRunning() && window.dispatchPendingEvents() != -1) {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();
        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        window.swapBuffers();
    }

    std::cout << "Cleaning up..." << std::endl;

    window.releaseEgl();
    window.releaseWayland();

    return 0;
}
