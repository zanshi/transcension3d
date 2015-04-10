#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <fstream>

//Blatantly stolen from http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
class ShaderProgram {
private:
    GLuint vertex_shader, fragment_shader, prog;
    std::string vfileName, ffileName;

    std::string getShaderType(GLuint type) {
        std::string name;
        switch (type) {
            case GL_VERTEX_SHADER:
                name = "Vertex Shader";
                break;
            case GL_FRAGMENT_SHADER:
                name = "Fragment Shader";
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

public:
    ShaderProgram(std::string vfileName, std::string ffileName) {
        this->vfileName = vfileName;
        this->ffileName = ffileName;
        auto v_source = readFromFile(vfileName);
        auto f_source = readFromFile(ffileName);
        vertex_shader = compile(GL_VERTEX_SHADER, v_source.c_str());
        fragment_shader = compile(GL_FRAGMENT_SHADER, f_source.c_str());
        prog = glCreateProgram();
        glAttachShader(prog, vertex_shader);
        glAttachShader(prog, fragment_shader);
        glLinkProgram(prog);
    }

    operator GLuint() { return prog; }

    void operator()() { glUseProgram(prog); }

    ~ShaderProgram() {
        glDeleteProgram(prog);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
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
};
