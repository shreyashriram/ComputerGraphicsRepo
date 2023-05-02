//
//  shader.cpp
//  GLFW_OpenGL_Sonar
//
//  Created by Shreya Shriram on 3/13/23.
//

#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// CONSTRUCTORS
Shader::Shader() {
    ID = 0;
    Type = GL_INVALID_ENUM;
    Path = "";
}

Shader::Shader( std::string shaderPath, GLenum shaderType ) {
    Create( shaderPath, shaderType );
}

// DESTRUCTOR
Shader::~Shader()
{
    Delete();
}

void Shader::Delete( void )
{
    glDeleteShader( ID );

    ID = 0;
    Type = GL_INVALID_ENUM;
    Path = "";
}

// Create
void Shader::Create( std::string shaderPath, GLenum shaderType )
{
    ID = glCreateShader( shaderType );

    Type = shaderType;
    Path = shaderPath;

    Load();
}

void Shader::Load() {
    
    if ( ID == 0 ) {
        return ;
    }
    
    std::string shaderSrc;
    std::ifstream srcFile;
    srcFile.exceptions ( std::ifstream::badbit );
    
    try {
        srcFile.open( Path );
        
        std::stringstream shaderStream;
        // Read file's buffer contents into streams
        shaderStream << srcFile.rdbuf( );
        
        srcFile.close();
        
        shaderSrc = shaderStream.str();
    }
    catch ( std::ifstream::failure e )
    {
        std::cout << "Shader File Reading Unsucessful." << std::endl;
    }
    
    const GLchar *src = shaderSrc.c_str();
    
    glShaderSource( ID, 1, &src, NULL );
    glCompileShader( ID );
    
    GLint success;
    GLchar infoLog[512];
    
    glGetShaderiv( ID, GL_COMPILE_STATUS, &success );
    if ( !success ) {
        glGetShaderInfoLog( ID, 512, NULL, infoLog );
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << Path << "\n" << infoLog << std::endl;
    }
    
}
