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
#include <vector>

#include "shader.h"
#include "shaderprogram.h"

const GLint WIDTH = 800, HEIGHT = 600;

ShaderProgram simpleProgram;

GLuint VAO;
GLuint VBO[1]; // 0 - Vertices, 1 - Color

// Other parameters
bool drawWireframe = true;

float perspZoom = 1.0f, perspSensitivity = 1.0f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

glm::mat4 model = glm::mat4( 1.0f );
glm::mat4 view = glm::mat4( 1.0f );
glm::mat4 projection = glm::mat4( 1.0f );

// TORUS STRUCTURES/VARIABLES
std::vector <glm::vec3> torus;
float resolution = 25;
float R = 0.5;
float r = 0.25;


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

        case GLFW_KEY_RIGHT: {
            perspRotationY += 5 * perspSensitivity;
            break;
        }

        case GLFW_KEY_LEFT: {
            perspRotationY -= 5 * perspSensitivity;
            break;
        }

        case GLFW_KEY_UP: {
            perspRotationX += 5 * perspSensitivity;
            break;
        }

        case GLFW_KEY_DOWN: {
            perspRotationX -= 5 * perspSensitivity;
            break;
        }

        case GLFW_KEY_P: {
            perspZoom += 0.03f;
            break;
        }

        case GLFW_KEY_L: {
            if ( perspZoom - 0.03f > 0.0f ) {
                perspZoom -= 0.03f;
            }

            break;
        }
            
        // CHANGE RESOLUTION
        case GLFW_KEY_Q: {
            resolution += 10;
            
            break;
        }
            
        case GLFW_KEY_A: {
            resolution -= 10;
            
            break;
        }
        
        // CHANGE tube radius - r
        case GLFW_KEY_W: {
            
            r += 0.02;
            
            if (r >= R) {
                r = R - 0.02;
            }
            
            break;
        }
            
        case GLFW_KEY_S: {
            r -= 0.02;
            
            if (r <= 0) {
                r = 0.02;
            }
            
            break;
        }
            
        // CHANGE center radius - R
        case GLFW_KEY_E: {
            R += 0.02;
            
            break;
        }
            
        case GLFW_KEY_D: {
            
            R -= 0.02;
            
            if (R <= r) {
                R = r + 0.02;
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

    glGenBuffers( 1, &VBO[0] );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
    glBufferData( GL_ARRAY_BUFFER, torus.size() * sizeof(glm::vec3), &torus[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid*)0);
    glEnableVertexAttribArray( 0 );

    glBindVertexArray(0);

}

void drawTorus( GLfloat R, GLfloat r, GLint resolution) {
    torus.clear();

    float angleStep = glm::radians( 360.0f/float(resolution) );
    
    // MAIN RADIUS
    float theta = 0.0f;
    float phi = 0.0f;
    
    GLfloat x, y, z;

    for ( int i = 0; i <= resolution; i++ ) {

        // TUBE RADIUS
        phi = 0.0f;
        
        for ( int j = 0; j <= (resolution); j++ ) {

            x = (R + (r * cos(theta))) * cos(phi);
            y = (R + (r * cos(theta))) * sin(phi);
            z = r * sin(theta);

            torus.push_back( glm::vec3( x, y, z) );

            phi += angleStep;
        }

        theta += angleStep;
    }
}

void init( void ) {

    // Print some info
    std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
    std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
    std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n";
    std::cout << "GLSL Version:   " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n\n";

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    
    drawTorus(0.5f, 0.25, resolution);
    
    createShaders();
    createAxisBuffers();
    
    
    std::cout << "         Interaction Menu   " << std::endl;
    std::cout << "      ----------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "      Q: Increase Resolution" << std::endl;
    std::cout << "      A: Decrease Resolution" << std::endl;
    std::cout << std::endl;
    std::cout << "      W: Increase Tube Radius" << std::endl;
    std::cout << "      S: Decrease Tube Radius" << std::endl;
    std::cout << std::endl;
    std::cout << "      E: Increase Outer Radius" << std::endl;
    std::cout << "      Q: Decrease Outer Radius" << std::endl;
    std::cout << std::endl;
    std::cout << "            ---------       " << std::endl;
    std::cout << "      P: Zoom In" << std::endl;
    std::cout << "      L: Zoom Out" << std::endl;
    std::cout << std::endl;
    std::cout << "      Rotate with Arrow Keys" << std::endl;
    std::cout << std::endl;
    std::cout << "      ↑      ↓      →      ←" << std::endl;
    std::cout << std::endl;
    std::cout << "      ----------------------" << std::endl;
    
    
}

int main() {

    // Create and initialize the OpenGL context
    glfwInit();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "PA2 - Torus", nullptr, nullptr );

    int screenWidth, screenHeight; // MAC Compatibility
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );

    glfwMakeContextCurrent( window );
    glViewport( 0, 0, screenWidth, screenHeight );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

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
        GLint objColorLoc = glGetUniformLocation( simpleProgram.GetID(), "objColor" );
        
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ));
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ));
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ));
        glUniform3f( objColorLoc, 0.0f, 0.0f, 1.0f );
        
        // Drawing in wireframe?
        if( drawWireframe == true )
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        drawTorus(R, r, resolution);
        
        glBindVertexArray( VAO );
        
        glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
        glBufferData( GL_ARRAY_BUFFER, torus.size() * sizeof(glm::vec3), &torus[0], GL_STATIC_DRAW );
        
        glDrawArrays( GL_TRIANGLE_STRIP, 0, torus.size() );
        
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );

    }

    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO[0] );

    glfwTerminate();

    return EXIT_SUCCESS;
}
