//
//  shader.h
//  GLFW_OpenGL_Sonar
//
//  Created by Shreya Shriram on 3/13/23.
//

#ifndef shader_h
#define shader_h

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Shader {
public:
    Shader();
    Shader( std::string shaderPath, GLenum shaderType );
    ~Shader();
    
public:
    void Create( std::string shaderPath, GLenum shaderType );
    void Delete();
    void Load();
    
public:
//    std::string GetInfoLog() const;
//    std::string GetSource() const;
//
    GLuint      GetID()   const { return ID;   }
//    GLenum      GetType() const { return Type; }
//    std::string GetPath() const { return Path; }
    
private:
    GLuint ID;
    GLenum Type;
    std::string Path;
};

#endif /* shader_h */
