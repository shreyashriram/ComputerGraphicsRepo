#include <iostream>
#include <math.h>
#include <vector>

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

#include "Camera.h"


const GLint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


ShaderProgram simpleProgram;

GLuint VAO;
GLuint VBO[1]; // 0 - Vertex Positions

// Other parameters
bool drawWireframe = false;

float perspZoom = 1.0f, perspSensitivity = 0.35f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

glm::mat4 model = glm::mat4( 1.0f );
glm::mat4 view = glm::mat4( 1.0f );
glm::mat4 projection = glm::mat4( 1.0f );


const int numControlPoints = 6;
int bezierCoef[numControlPoints];
GLfloat controlPoints[numControlPoints][3] = {
    
    { -0.8,  0.3,  0.1 },
    {  2.0,  5.0, -15.0 },
     { 0.0, -0.5, -0.9 },
     { 0.2,  0.5, -0.6 },
     { 0.8,  0.8, -0.1 },
     { 0.6, -0.3, -0.1 }
    
};

int resolution = 20;

GLfloat vertices[] = {
    -0.25f, -0.25f, -0.25f,
     0.25f, -0.25f, -0.25f,
     0.25f,  0.25f, -0.25f,
     0.25f,  0.25f, -0.25f,
    -0.25f,  0.25f, -0.25f,
    -0.25f, -0.25f, -0.25f,

    -0.25f, -0.25f,  0.25f,
     0.25f, -0.25f,  0.25f,
     0.25f,  0.25f,  0.25f,
     0.25f,  0.25f,  0.25f,
    -0.25f,  0.25f,  0.25f,
    -0.25f, -0.25f,  0.25f,

    -0.25f,  0.25f,  0.25f,
    -0.25f,  0.25f, -0.25f,
    -0.25f, -0.25f, -0.25f,
    -0.25f, -0.25f, -0.25f,
    -0.25f, -0.25f,  0.25f,
    -0.25f,  0.25f,  0.25f,

     0.25f,  0.25f,  0.25f,
     0.25f,  0.25f, -0.25f,
     0.25f, -0.25f, -0.25f,
     0.25f, -0.25f, -0.25f,
     0.25f, -0.25f,  0.25f,
     0.25f,  0.25f,  0.25f,

    -0.25f, -0.25f, -0.25f,
     0.25f, -0.25f, -0.25f,
     0.25f, -0.25f,  0.25f,
     0.25f, -0.25f,  0.25f,
    -0.25f, -0.25f,  0.25f,
    -0.25f, -0.25f, -0.25f,

    -0.25f,  0.25f, -0.25f,
     0.25f,  0.25f, -0.25f,
     0.25f,  0.25f,  0.25f,
     0.25f,  0.25f,  0.25f,
    -0.25f,  0.25f,  0.25f,
    -0.25f,  0.25f, -0.25f,
    
};

glm::vec3 cubePositions[] {
    glm::vec3( 0.0f, 0.0f, 0.0f ),
    glm::vec3( 2.0f, 5.0f, -15.0f ),
    glm::vec3( -1.5f, 2.2f, -2.5f ),
    glm::vec3( -3.8f, -2.0f, -12.3f ),
    glm::vec3( 2.4f, -0.4f, -3.5f ),
    
    glm::vec3( 1.3f, -2.0f, -2.5f ),
    glm::vec3( 1.5f, 2.0f, -2.5f ),
    glm::vec3( 1.5f, 0.2f, -1.5f ),
    glm::vec3( -1.3f, 1.0f, -1.5f )
};


void bezierCurve(int resolution);
std::vector<glm::vec3> bezierPoints;

//  UTIL FUNCTIONS
int factorial( int x ) {
    int mult = 1;
    
    if (x != 0) {
        for (int i = 1; i <= x; i++) {
            mult = mult*i;
        }
    }
    
    return mult;
}

void createPolynomial(int N) {
    int n = N-1;
    
    for (int i = 0; i < numControlPoints; i++) {
        bezierCoef[i] = factorial(n)/((factorial(i))*(factorial(n-i)));
    }
}

void evaluateBezier( float t, int index) {
    
    float sum_x = 0;
    float sum_y = 0;
    float sum_z = 0;

    int n = numControlPoints - 1;
    
    for (int i = 0; i < numControlPoints; i++) {
        
        sum_x = sum_x + ( controlPoints[i][0] * bezierCoef[i] * (pow(t, i)) * (pow((1-t), (n-i))) );
        sum_y = sum_y + ( controlPoints[i][1] * bezierCoef[i] * (pow(t, i)) * (pow((1-t), (n-i))) );
        sum_y = sum_y + ( controlPoints[i][2] * bezierCoef[i] * (pow(t, i)) * (pow((1-t), (n-i))) );
    }
    
    bezierPoints.push_back( glm::vec3{sum_x, sum_y, sum_z} );
}

void bezierCurve(int resolution) {
    
    bezierPoints.clear();
    
    float t = 0; // parametric variable
    int i = 0;
    float delta = float(1)/float(resolution);


    while (t <= (1.0)) {
        
        evaluateBezier(t, i);
        
        t = t + delta;
        i = i+1;
    }
}

Camera camera( glm::vec3( 0.0f, 0.0f, 1.0f) );
GLfloat lastX = WIDTH/2.0f;
GLfloat lastY = WIDTH/2.0f;
bool keys[1024];
bool firstMouse = true;

void doMovement( ) {
    // Camera controls
    if( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.processKeyboard( FORWARD, deltaTime );
    }
    
    if( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.processKeyboard( BACKWARD, deltaTime );
    }
    
    if( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.processKeyboard( LEFT, deltaTime );
    }
    
    if( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.processKeyboard( RIGHT, deltaTime );
    }
}

void keyboardCallback( GLFWwindow *window, int key, int scancode, int action, int mode ) {
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}


void mouseCallback( GLFWwindow *window, double xPos, double yPos ) {
    if ( firstMouse ) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;
    
    lastX = xPos;
    lastY = yPos;
    
    camera.processMouseMovement(xOffset, yOffset);
}

void scrollCallback( GLFWwindow *window, double xOffset, double yOffset ) {
    camera.processMouseScroll( yOffset );
}

void updateTransformationMatrices( void ) {
    
    // Projection Matrix
//    projection = glm::perspective( 45.0f, ( GLfloat )SCREEN_WIDTH / ( GLfloat )SCREEN_HEIGHT, 0.1f, 100.0f );
    glm::mat4 projection = glm::perspective( camera.getZoom(), ( GLfloat )SCREEN_WIDTH / ( GLfloat )SCREEN_HEIGHT, 0.1f, 100.0f );
     
//    // View Matrix
//    glm::vec3 eye   ( 0.0, 0.0, 2.0 );
//    glm::vec3 center( 0.0, 0.0, 0.0 );
//    glm::vec3 up    ( 0.0, 1.0, 0.0 );
    
//    view = glm::lookAt( eye, center, up );
//    view = glm::mat4( 1.0f );
    view = camera.getViewMatrix();
    
    // Model Matrix
    model = glm::mat4( 1.0f );
//    model = glm::rotate( model, glm::radians( perspRotationX ), glm::vec3( 1.0, 0.0, 0.0 ) );
//    model = glm::rotate( model, glm::radians( perspRotationY ), glm::vec3( 0.0, 1.0, 0.0 ) );
//
//    model = glm::scale( model, glm::vec3( perspZoom ) );
//
}

void createShaders( void ) {

    simpleProgram.Create("/Users/shreyashriram/Documents/ComputerGraphicsRepo/Final/Final/resources/shaders/simple.vert", "/Users/shreyashriram/Documents/ComputerGraphicsRepo/Final/Final/resources/shaders/simple.frag");

}

void createAxisBuffers( void ) {

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    glGenBuffers( 1, &VBO[0] );

    glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( bezierPoints ), &bezierPoints[0], GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid*)0);
    glEnableVertexAttribArray( 0 );

    glBindVertexArray(0);

}

void init( void ) {

    // Print some info
    std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
    std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
    std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n";
    std::cout << "GLSL Version:   " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n\n";

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    
    createPolynomial( numControlPoints );
    bezierCurve( resolution );
    
    std::cout << bezierPoints.size() << std::endl;
    
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

    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "Final Project", nullptr, nullptr );

    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );

    glfwMakeContextCurrent( window );
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
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
    glfwSetCursorPosCallback( window, mouseCallback );
    glfwSetScrollCallback( window, scrollCallback );
    
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    init();

    // GAME LOOP
    while( !glfwWindowShouldClose( window )) {
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        glfwPollEvents();
        doMovement();
        
        // clear Color Buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // Render Triangle
        simpleProgram.Use();
        
        // Update Transformation Matrices
//        updateTransformationMatrices( );
        
        glm::mat4 projection = glm::mat4( 1.0f );
                projection = glm::perspective(camera.getZoom(), (GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
                
                // Create camera transformation
        glm::mat4 view = glm::mat4( 1.0f );
        view = camera.getViewMatrix();
        
        
        GLint modelLoc = glGetUniformLocation( simpleProgram.GetID(), "model" );
        GLint viewLoc = glGetUniformLocation( simpleProgram.GetID(), "view" );
        GLint projLoc = glGetUniformLocation( simpleProgram.GetID(), "projection" );
        GLint objColorLoc = glGetUniformLocation( simpleProgram.GetID(), "objectColor" );
        
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ));
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ));
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ));

        
        if( drawWireframe == true )
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        
        glUniform3f( objColorLoc, 1.0f, 0.0f, 1.0f );
        glBindVertexArray( VAO );
        glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
    

        // DRAW CONTROL POINTS & SEGMENTS
        glBufferData( GL_ARRAY_BUFFER , sizeof( controlPoints ), controlPoints, GL_DYNAMIC_DRAW);
        glDrawArrays( GL_LINE_STRIP , 0, 6);
        
        glUniform3f( objColorLoc, 0.5f, 0.5f, 0.0f );
        glBufferData( GL_ARRAY_BUFFER, bezierPoints.size() * sizeof(glm::vec3), &bezierPoints[0], GL_STATIC_DRAW);
        glDrawArrays( GL_LINE_STRIP, 0, bezierPoints.size() );
        
        
        glUniform3f( objColorLoc, 1.0f, 0.0f, 0.0f );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices,  GL_STATIC_DRAW);
        
        for( GLuint i = 0; i < 10; i++ )
        {
            
            glm::mat4 model = glm::mat4( 1.0f );
            model = glm::translate( model, cubePositions[i] );
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3( 1.0f, 0.3f, 0.5f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        
//        glDrawArrays( GL_TRIANGLE_STRIP, 0, 36 );
        
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );

    }

    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO[0] );

    glfwTerminate();

    return EXIT_SUCCESS;
}
