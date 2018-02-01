#include <fstream>
#include <sstream>

#include "shader.h"

Shader::Shader(GLenum type, std::string path) :
    compiled(false),
    path(path)
{
    id = glCreateShader(type);
    read();
}

Shader::~Shader() {
    glDeleteShader(id);
}

void Shader::read() {
    std::ifstream file;
    file.open(path);

    std::stringstream stream;
    stream << file.rdbuf();

    file.close();
    code = stream.str();
}

void Shader::compile() {
    if (compiled)
        return;

    const char* source = code.c_str();

    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        char message[1024];
        glGetShaderInfoLog(id, 1024, nullptr, message);

        std::cout << path << ":\n\t";
        std::cout << message << std::endl;
        return;
    }

    compiled = true;
}

unsigned int Shader::getId() {
    return id;
}

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::add(Shader shader) {
    shader.compile();
    glAttachShader(id, shader.getId());
}

void ShaderProgram::link() {
    glLinkProgram(id);

    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (success != GL_TRUE) {
        char message[1024];
        glGetProgramInfoLog(id, 1024, nullptr, message);
        
        std::cout << message << std::endl;
        return;
    }
}

void ShaderProgram::use() {
    glUseProgram(id);
}
