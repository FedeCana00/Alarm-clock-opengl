#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>

static GLuint createShaderProgram(const std::string vertex_filename, const std::string fragment_filename)
{
    // find current path
    std::string this_file = __FILE__;
    std::string this_path = this_file.substr(0, this_file.find_last_of("\\/") + 1);

    // open the files
    std::string compl_vertex_filename = this_path + vertex_filename;
    std::ifstream vertex_file(compl_vertex_filename.c_str());
    if (!vertex_file)
    {
        std::cout << "Could not open vertex shader file: \"" << this_path + vertex_filename << "\"" << std::endl;
    }

    std::string compl_fragment_filename = this_path + fragment_filename;
    std::ifstream fragment_file(compl_fragment_filename.c_str());
    if (!fragment_file)
    {
        std::cout << "Could not open fragment shader file: \"" << this_path + fragment_filename << "\"" << std::endl;
    }

    // files to strings
    std::string vertexShaderSourceStr = std::string(std::istreambuf_iterator<char>(vertex_file), std::istreambuf_iterator<char>());
    const char * vertexShaderSource = vertexShaderSourceStr.c_str();
    std::string fragmentShaderSourceStr = std::string(std::istreambuf_iterator<char>(fragment_file), std::istreambuf_iterator<char>());
    const char * fragmentShaderSource = fragmentShaderSourceStr.c_str();

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
