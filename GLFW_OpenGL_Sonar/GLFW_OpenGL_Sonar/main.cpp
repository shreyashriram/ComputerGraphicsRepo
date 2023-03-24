#include <iostream>
#include <math.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "shaderprogram.h"

#include "camera.h"

const GLint WIDTH = 800, HEIGHT = 600;

ShaderProgram simpleProgram;

GLuint VAO;
GLuint VBO[2];

// Other parameters
bool drawWireframe = false;

float perspZoom = 1.0f, perspSensitivity = 0.35f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

glm::mat4 model = glm::mat4( 1.0f );
glm::mat4 view = glm::mat4( 1.0f );
glm::mat4 projection = glm::mat4( 1.0f );

GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,

    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 0.5f, 0.5f, 1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,

    0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f
    
};

void keyboardCallback( GLFWwindow *window, int key, int scancode, int action, int mode ) {
        
    switch ( key ) {
            
        case GLFW_KEY_SPACE: {
            if ( action == GLFW_PRESS ) {
                drawWireframe = !drawWireframe;
                
                if( drawWireframe == true )
                    std::cout << "Wireframes on.\n";
                else
                    std::cout << "Wireframes off.\n";
            }
            break;
        }
        
        case GLFW_KEY_D: {
            perspRotationY += 5 * perspSensitivity;
            break;
        }
            
        case GLFW_KEY_A: {
            perspRotationY -= 5 * perspSensitivity;
            break;
        }
            
        case GLFW_KEY_W: {
            perspRotationX += 5 * perspSensitivity;
            break;
        }
            
        case GLFW_KEY_S: {
            perspRotationX -= 5 * perspSensitivity;
            break;
        }
            
        case GLFW_KEY_E: {
            perspZoom += 0.03f;
            break;
        }
            
        case GLFW_KEY_Q: {
            if ( perspZoom - 0.03f > 0.0f ) {
                perspZoom -= 0.03f;
            }
            
            break;
        }
    
    }
}

void updateTransformationMatrices( void ) {
    
    // Projection Matrix
    projection = glm::perspective( 45.0f, ( GLfloat )WIDTH / ( GLfloat )HEIGHT, 0.1f, 100.0f );
    
    // View Matrix
    glm::vec3 eye   ( 0.0, 0.0, 2.0 );
    glm::vec3 center( 0.0, 0.0, 0.0 );
    glm::vec3 up    ( 0.0, 1.0, 0.0 );
    
    view = glm::lookAt( eye, center, up );
    
    // Model Matrix
    model = glm::mat4( 1.0f );
    model = glm::rotate( model, glm::radians( perspRotationX ), glm::vec3( 1.0, 0.0, 0.0 ) );
    model = glm::rotate( model, glm::radians( perspRotationY ), glm::vec3( 0.0, 1.0, 0.0 ) );
    
    model = glm::scale( model, glm::vec3( perspZoom ) );
    
}

void createShaders( void ) {

    simpleProgram.Create("resources/shaders/simple.vert", "resources/shaders/simple.frag");

}

void createAxisBuffers( void ) {

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    glGenBuffers( 2, &VBO[0] );

    glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid*)0);
    glEnableVertexAttribArray( 0 );

    glBindBuffer( GL_ARRAY_BUFFER, VBO[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( colors ), colors, GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), (GLvoid*)0);
    glEnableVertexAttribArray( 1 );

    glBindVertexArray(0);

}

void init( void ) {

    // Print some info
    std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
    std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
    std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n";
    std::cout << "GLSL Version:   " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n\n";

    glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );

    createShaders();
    createAxisBuffers();

}

int main() {

    // Create and initialize the OpenGL context
    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "LEARN OPENGL", nullptr, nullptr );

    int screenWidth, screenHeight; // MAC Compatibility
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );

    glfwMakeContextCurrent( window );
    glViewport( 0, 0, screenWidth, screenHeight );
    glEnable( GL_DEPTH_TEST );

    if ( nullptr == window ) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glewExperimental = GL_TRUE; //
    if ( GLEW_OK != glewInit() ) {
        std::cout << "Failed to initialize GLEW" << std::endl;

        return EXIT_FAILURE;
    }

    glfwSetKeyCallback( window, keyboardCallback );

    init();

    // GAME LOOP
    while( !glfwWindowShouldClose( window )) {
        
        glfwPollEvents();

        // clear Color Buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Update Transformation Matrices
        updateTransformationMatrices( );
        
        // Render Triangle
        simpleProgram.Use();

        GLint modelLoc = glGetUniformLocation( simpleProgram.GetID(), "model" );
        GLint viewLoc = glGetUniformLocation( simpleProgram.GetID(), "view" );
        GLint projLoc = glGetUniformLocation( simpleProgram.GetID(), "projection" );

        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ));
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ));
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ));

        // Drawing in wireframe?
        if( drawWireframe == true )
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        
        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 36 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );

    }

    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO[0] );

    glfwTerminate();

    return EXIT_SUCCESS;
}
