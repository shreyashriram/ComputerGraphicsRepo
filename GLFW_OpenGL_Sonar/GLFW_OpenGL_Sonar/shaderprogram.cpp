//
//  shaderprogram.cpp
//  GLFW_OpenGL_Sonar
//
//  Created by Shreya Shriram on 3/13/23.
//

#include "shaderprogram.h"
#include <iostream>

// CONSTRUCTORS
ShaderProgram::ShaderProgram() {
    ID = 0;
}

ShaderProgram::ShaderProgram( std::string vspath, std::string fspath ) {
    Create( vspath, fspath );
}

// DESTRUCTOR
ShaderProgram::~ShaderProgram()
{
    Delete();
}

void ShaderProgram::Delete( void )
{
    if( ID != 0 )
    {
        glDetachShader( ID, vertexShader.GetID() );
        glDetachShader( ID, fragmentShader.GetID() );

        glDeleteProgram( ID );

        ID = 0;
    }
}

// CREATE
void ShaderProgram::Create( std::string vspath, std::string fspath ) {
    
    ID = glCreateProgram();
    
    if ( ID != 0 ) {
        vertexShader.Create( vspath, GL_VERTEX_SHADER );
        glAttachShader( ID, vertexShader.GetID() );
        
        fragmentShader.Create( fspath, GL_FRAGMENT_SHADER );
        glAttachShader( ID, fragmentShader.GetID() );
        
        Link();
    }
}

void ShaderProgram::Link() {

    glLinkProgram( ID );

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv( ID, GL_LINK_STATUS, &success );
    if (!success)
    {
        glGetProgramInfoLog( ID, 512, NULL, infoLog );
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

void ShaderProgram::Use( void )
{
    glUseProgram( ID );
}



