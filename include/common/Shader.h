#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <fstream>

//Blatantly stolen from http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
class ShaderProgram {
public:
    ShaderProgram() {
        prog = glCreateProgram();
    }

    operator GLuint() { return prog; }

    void operator()() { glUseProgram(prog); }

    ~ShaderProgram() {
        glDeleteProgram(prog);
    }

    static const std::string readFromFile(std::string fileName) {
        std::ifstream ifs(fileName.c_str());
        std::string out;
        if (ifs.is_open()) {
            std::string s;
            while (std::getline(ifs, s)) {
                out += s + "\n";
            }
        }
        else {
            std::cerr << "Could not open file" << std::endl;
        }
        ifs.close();
        return out;
    }

protected:
    GLuint AttachShader(GLuint shaderType, std::string source) {
        GLuint sh = compile(shaderType, source.c_str());
        glAttachShader(prog, sh);

        return sh;
    }

    void configureShaderProgram() {
        glLinkProgram(prog);
    }

private:
    GLuint prog;

    std::string getShaderType(GLuint type) {
        std::string name;
        switch (type) {
            case GL_VERTEX_SHADER:
                name = "Vertex Shader";
                break;
            case GL_FRAGMENT_SHADER:
                name = "Fragment Shader";
                break;
            case GL_GEOMETRY_SHADER:
                name = "Geometry Shader";
                break;
            default:
                name = "Unknown Shader type";
                break;
        }
        return name;
    }

    GLuint compile(GLuint type, GLchar const *source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &log[0]);
            std::cerr << "Failed to compile shadertype: " + getShaderType(type) << std::endl
            << log << std::endl;
            exit(EXIT_FAILURE);
            return false;
        }
        return shader;
    }

};
