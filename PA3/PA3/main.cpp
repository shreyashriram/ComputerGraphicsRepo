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

ShaderProgram persplightProgram;

GLuint VAO;
GLuint VBO[2]; // 0 - Vertices, 1 - Normals
GLuint IBO;

// Other parameters
bool drawWireframe = true;
bool drawNormals = false;
bool drawSmooth = false;

float perspZoom = 1.0f, perspSensitivity = 1.0f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

glm::mat4 model = glm::mat4( 1.0f );
glm::mat4 view = glm::mat4( 1.0f );
glm::mat4 projection = glm::mat4( 1.0f );

// TORUS STRUCTURES/VARIABLES
std::vector <glm::vec3> torus;

std::vector <GLuint> indices;
std::vector <glm::vec3> normals;
std::vector <glm::vec3> segments;


float resolution = 20;
float R = 0.5;
float r = 0.25;
float seg_size = 8.0;

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
            
        case GLFW_KEY_C: {
            if ( action == GLFW_PRESS ) {

                drawNormals = !drawNormals;

                if( drawNormals == true )
                    std::cout << "Normals on.\n";
                else
                    std::cout << "Normals off.\n";
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
            resolution += 5;
            
            if ( resolution > 50 ) {
                resolution = 50;
            }
            
            break;
        }
            
        case GLFW_KEY_A: {
            
            
            resolution -= 5;
            
            if ( resolution < 5 ) {
                resolution = 5;
            }

            
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
            
        case GLFW_KEY_Z: {
        
            if ( action == GLFW_PRESS ) {

                drawSmooth = false;
                std::cout << "FLAT SHADING" << std::endl;

            }
            
            break;
         
        }
            
        case GLFW_KEY_X: {
            
            if ( action == GLFW_PRESS ) {

                drawSmooth = true;
                std::cout << "SMOOTH SHADING" << std::endl;

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
    persplightProgram.Create("resources/shaders/persplight.vert", "resources/shaders/persplight.frag");

}

void createAxisBuffers( void ) {

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    glGenBuffers( 2, &VBO[0] );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
    glBufferData( GL_ARRAY_BUFFER, torus.size() * sizeof(glm::vec3), &torus[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid*)0);
    glEnableVertexAttribArray( 0 );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO[1] );
    glBufferData( GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid*)0);
    glEnableVertexAttribArray( 1 );
    
    glGenBuffers( 1, &IBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}


void smoothShading( int numRings, int numSegments ) {

    // 1 - CALCULATE & UPDATE NORMALS PER VERTEX
    float minorDelta = ( M_PI*2 )/ numSegments;
    float majorDelta = ( M_PI*2 )/ numRings;

    float major, minor = 0.0f;
    
    for ( int i = 0; i <= numRings; i++ ) {
        
        float sinMajor = sin( major );
        float cosMajor = cos( major );
        
        float minor = 0.0f;
        
        for ( int j = 0; j <= numSegments; j++ ) {
            
            float sinMinor = sin( minor );
            float cosMinor = cos( minor );
            
            float nx = -sinMajor * ( R + r * cosMinor );
            float ny = cosMajor * ( R + r * cosMinor );
            float nz = r * sinMinor;
            
            float length = sqrt(nx * nx + ny * ny + nz * nz);
            nx /= length;
            ny /= length;
            nz /= length;
            
            normals.push_back( glm::vec3( nx, ny, nz) );
    
            minor += minorDelta;
        }
        
        major += majorDelta;
    }
    
    // 2 - UPDATE SEGMENTS PER VERTEX
    for ( int i = 0; i < torus.size(); i++ ) {

        segments.push_back( torus[i] );
        segments.push_back( glm::vec3(torus[i] + (normals[i]/seg_size)) );
    }
}
void flatShading( ) {
    
    for ( int i = 0; i < indices.size(); i += 3) {
        // 1 - CALCULATE & UPDATE NORMALS PER FACE
        glm::vec3 v1 = torus[ indices[i] ];
        glm::vec3 v2 = torus[ indices[i+1] ];
        glm::vec3 v3 = torus[ indices[i+2] ];
    
        glm::vec3 n = glm::cross( v1 - v3, v1- v2);

        float length = sqrt( n[0]*n[0] + n[1]*n[1] + n[2]*n[2] );
        n[0] /= length;
        n[1] /= length;
        n[2] /= length;
        
        for (int j = 0; j < 3; j++) {
            normals.push_back(n);
        }
        
        // 2 - UPDATE SEGMENTS ( center of face -> normal )
        glm::vec3 center = glm::vec3 (((v1[0]+v2[0]+v3[0])/3), ((v1[1]+v2[1]+v3[1])/3), ((v1[2]+v2[2]+v3[2])/3));
        
        segments.push_back(center);
        segments.push_back(center+(n/seg_size));
        
    }
    
}

void updateIndices( int numSegments ) {
    
    int delta = numSegments + 1;
    
    GLuint v1, v2, v3;

    for ( int i = 0; i < torus.size() - delta; i += delta ) {
    
        for ( int j = 0; j < delta; j++ ) {
            
            // TRIANGLE A - Doesn't apply to repeated
            if ( j != ( delta - 1 ) ) {
                v1 = i + j;
                v2 = i + j + 1;
                v3 = i + j + delta;
                
                indices.push_back(v1);
                indices.push_back(v2);
                indices.push_back(v3);
            }
            
            // TRIANGLE B - Doesn't apply to first
            if ( j != 0 ) {
                v1 = i + j;
                v2 = i + j + delta;
                v3 = i + j + delta - 1;
                
                indices.push_back(v1);
                indices.push_back(v2);
                indices.push_back(v3);
            }
        }
    }
}


void drawTorus( GLfloat R, GLfloat r, GLint resolution ) {


    torus.clear();
    indices.clear();
    
    normals.clear();
    segments.clear();

    int numRings = resolution;
    int numSegments = resolution;

    float minorDelta = ( M_PI*2 )/ numSegments;
    float majorDelta = ( M_PI*2 )/ numRings;

    float major, minor = 0.0f;
    GLfloat x, y, z;

    for ( int i = 0; i <= numRings; i++ ) {
        
        float sinMajor = sin( major );
        float cosMajor = cos( major );
        
        float minor = 0.0f;
        
        for ( int j = 0; j <= numSegments; j++ ) {
            
            float sinMinor = sin( minor );
            float cosMinor = cos( minor );
            
            x = (R + r * cosMinor)*cosMajor;
            y = (R + r * cosMinor)*sinMajor;
            z = r * sinMinor;
            
            torus.push_back( glm::vec3( x, y, z) );
    
            minor += minorDelta;
        }
        
        major += majorDelta;
    }
    
    updateIndices( numSegments );
    
    if ( drawSmooth ) {
        smoothShading( numRings, numSegments );
    }
    
    else {
        flatShading();
    }
}


void init( void ) {

    // Print some info
    std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
    std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
    std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n";
    std::cout << "GLSL Version:   " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n\n";

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    drawTorus(R, r, resolution);

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
    std::cout << "      Z: Flat Shading" << std::endl;
    std::cout << "      X: Smooth Shading" << std::endl;
    std::cout << "      C: Toggle Normal References" << std::endl;
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
//    glEnable( GL_CULL_FACE );

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

//     GAME LOOP
    while( !glfwWindowShouldClose( window )) {

        glfwPollEvents();

        // clear Color Buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Update Transformation Matrices
        updateTransformationMatrices( );

        // Render Triangle
        persplightProgram.Use();

        GLint modelLoc = glGetUniformLocation( persplightProgram.GetID(), "modelMatrix" );
        GLint viewLoc = glGetUniformLocation( persplightProgram.GetID(), "viewMatrix" );
        GLint projLoc = glGetUniformLocation( persplightProgram.GetID(), "projectionMatrix" );
        GLint objColorLoc = glGetUniformLocation( persplightProgram.GetID(), "objColor" );

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
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
        glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        if ( drawNormals ) {
            glUniform3f( objColorLoc, 0.0f, 1.0f, 0.0f );
            glBufferData( GL_ARRAY_BUFFER, segments.size() * sizeof(glm::vec3), &segments[0], GL_STATIC_DRAW );
            glDrawArrays(GL_LINES, 0, segments.size());
        }

        glBindBuffer( GL_ARRAY_BUFFER, VBO[1] );
        glBufferData( GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW );

        glBindVertexArray( 0 );

        glfwSwapBuffers( window );

    }

    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO[0] );

    glfwTerminate();

    return EXIT_SUCCESS;
}
