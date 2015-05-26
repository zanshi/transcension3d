//
// Created by Benjamin Wiberg on 15-05-25.
//


#pragma once

#include "common/Shader.h"

class VGFShaderProgram : public ShaderProgram {
public:
    VGFShaderProgram(std::string vfileName, std::string gfileName, std::string ffileName)
            : vfileName_(vfileName), gfileName_(gfileName), ffileName_(ffileName) {
    auto v_source = readFromFile(vfileName);
    auto g_source = readFromFile(gfileName);
    auto f_source = readFromFile(ffileName);

    vertex_shader_ = AttachShader(GL_VERTEX_SHADER, v_source);
    geometry_shader_ = AttachShader(GL_GEOMETRY_SHADER, g_source);
    fragment_shader_ = AttachShader(GL_FRAGMENT_SHADER, f_source);

    configureShaderProgram();
}

~VGFShaderProgram() {
    glDeleteShader(vertex_shader_);
    glDeleteProgram(geometry_shader_);
    glDeleteShader(fragment_shader_);
}

private:
    std::string vfileName_, gfileName_, ffileName_;
    GLuint vertex_shader_, geometry_shader_, fragment_shader_;
};