#version 320 es

layout(triangles) in;
layout(line_strip, max_vertices = 4) out;

out vec3 gColor;

void main() {
    gColor = vec3(0.8, 0.8, 0.8);

    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    EndPrimitive();
}
