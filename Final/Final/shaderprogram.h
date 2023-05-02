//
//  shaderprogram.h
//  GLFW_OpenGL_Sonar
//
//  Created by Shreya Shriram on 3/13/23.
//

#ifndef shaderprogram_h
#define shaderprogram_h

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "shader.h"

class ShaderProgram {

public:
    // CONSTRUCTORS
    ShaderProgram();
    ShaderProgram( std::string vspath, std::string fspath );
    ~ShaderProgram();

public:
    // FUNCTIONALITY
    void Create( std::string vspath, std::string fspath );
    void Delete();
    void Link();
    void Validate();
    void Reload();
    void Use();
    
public:
    // HELPER
    std::string GetInfoLog( void ) const;
    GLuint GetID() {
        return ID;
    }

private:
    GLuint ID;
    Shader vertexShader, fragmentShader;
};

#endif /* shaderprogram_h */
