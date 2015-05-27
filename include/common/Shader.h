#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <fstream>

//Adapted from http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
class ShaderProgram {
public:
    ShaderProgram(std::string vfilename, std::string ffilename, std::string gfilename = "") {
        auto v_source = readFromFile(vfilename);
        auto f_source = readFromFile(ffilename);

        AttachShader(GL_VERTEX_SHADER, v_source);
        AttachShader(GL_FRAGMENT_SHADER, f_source);

        if (gfilename != "") {
            auto g_source = readFromFile(gfilename);
            AttachShader(GL_GEOMETRY_SHADER, g_source);

            std::cout << "Created vertex, geometry and fragment ShaderProgram.\n";
        } else {
            std::cout << "Created vertex and fragment ShaderProgram.\n";
        }

        ConfigureShaderProgram();
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
        shader_programs_.push_back(sh);

        std::cout << "Attached shader of type: '" << getShaderType(shaderType) << "'\n";

        return sh;
    }

    void ConfigureShaderProgram() {
        prog = glCreateProgram();

        for (GLuint shader_program : shader_programs_) {
            glAttachShader(prog, shader_program);
        }

        glLinkProgram(prog);

        GLint isLinked = GL_FALSE;
        glGetProgramiv(prog, GL_LINK_STATUS, (int *)&isLinked);

        if (isLinked == GL_FALSE) {
            GLint length;
            glGetShaderiv(prog, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(prog, length, &length, &log[0]);
            std::cerr << "Failed to link shaderprogram : " << std::endl
            << log << std::endl;
            exit(EXIT_FAILURE);
        } else {
            std::cout << "Shader linking complete!\n";
        }
    }

private:
    std::vector<GLuint> shader_programs_;
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
        if (compiled == GL_FALSE) {
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

    ShaderProgram() {}
};
