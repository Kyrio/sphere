#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <string>

#include <GLES3/gl3.h>

class Shader {
    public:
        Shader(GLenum type, std::string path);
        ~Shader();

        void read();
        void compile();

        unsigned int getId();

    protected:
        unsigned int id;
        bool compiled;

        std::string path;
        std::string code;
};

class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();
        
        void add(Shader& shader);
        void link();
        void use();

        int getUniformLocation(const char* name);

    private:
        unsigned int id;
};

ShaderProgram& operator<<(ShaderProgram& program, Shader& shader);

#endif // SHADER_H
