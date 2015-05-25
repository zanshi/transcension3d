//
// Created by Benjamin Wiberg on 15-05-25.
//


#pragma once

#include "common/Shader.h"

class VFShaderProgram : public ShaderProgram {
public:
    VFShaderProgram(std::string vfileName, std::string ffileName)
            : vfileName_(vfileName), ffileName_(ffileName) {
        auto v_source = readFromFile(vfileName);
        auto f_source = readFromFile(ffileName);

        vertex_shader_ = AttachShader(GL_VERTEX_SHADER, v_source);
        fragment_shader_ = AttachShader(GL_FRAGMENT_SHADER, f_source);

        configureShaderProgram();
    }

    ~VFShaderProgram() {
        glDeleteShader(vertex_shader_);
        glDeleteShader(fragment_shader_);
    }

private:
    std::string vfileName_, ffileName_;
    GLuint vertex_shader_, fragment_shader_;
};