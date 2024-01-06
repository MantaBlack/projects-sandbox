#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* file_name);

class Shader
{
public:
    GLuint id;

    Shader(const char* vertex_file, const char* fragment_file);

    void activate();
    void cleanup();
};