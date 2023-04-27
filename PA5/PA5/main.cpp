#include <iostream>
#include <math.h>
#include <cmath>
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

#define WIDTH 800
#define HEIGHT 600

ShaderProgram simpleProgram;

GLuint VAO;
GLuint VBO[1];

float a, b, dist;
int selectedPoint = NULL;
bool movePoint = false;
bool drawBspline = true;

const int numControlPoints = 6;
int bezierCoef[numControlPoints];
GLfloat controlPoints[numControlPoints][2] = {
    
    { -0.8,  0.3 },
    { -0.5, -0.8 },
     { 0.0, -0.5 },
     { 0.2,  0.5 },
     { 0.8,  0.8 },
     { 0.6, -0.3 }
};

int resolution = 20;

// B-SPLINE CONSTANTS
int n = numControlPoints-1;
const int k = 3;

GLfloat size = 10;

int u_min = k;
int u_max = n+2;
int num_knots = k+n+1;

void bezierCurve(int resolution);
std::vector<glm::vec2> bezierPoints;

void BsplineCurve(int resolution);
std::vector<glm::vec2> bsplinePoints;

// UTIL FUNCTIONS
float distance( float x1, float y1, float x2, float y2 ) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

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

// CALLBACK FUNCTIONS
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && movePoint == false ) {
        
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        
        a  = - 1 + ((xPos/float( WIDTH ))*2);
        b  =  1 - ((yPos/float( HEIGHT ))*2);
        
        
        for (int i = 0; i < 6; i++) {
            dist = distance(a, b, controlPoints[i][0], controlPoints[i][1]);
            if (dist <= 0.02) {
                std::cout << "point " << i <<std::endl;
                
                selectedPoint = i;
                movePoint = true;
                            
                break;
            }
        }
    }
    
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && movePoint == true ) {
        selectedPoint = false;
        movePoint = NULL;
    }
}

void cursorPositionCallback( GLFWwindow *window, double xPos, double yPos ) {
    
    a  = - 1 + ((xPos/float( WIDTH ))*2);
    b  =  1 - ((yPos/float( HEIGHT ))*2);
    
    if (movePoint){
            
        controlPoints[selectedPoint][0] = a;
        controlPoints[selectedPoint][1] = b;
        
        bezierCurve( resolution );
        BsplineCurve( resolution );
    }
}

void keyboardCallback( GLFWwindow *window, int key, int scancode, int action, int mode ) {
        
    switch ( key ) {
            
        case GLFW_KEY_B: {
            if ( action == GLFW_PRESS ) {
                drawBspline = !drawBspline;
                
                if( drawBspline == true )
                    std::cout << "Drawing Bezier Curve\n";
                else
                    std::cout << "Drawing B-Spline Curve\n";
            }
            break;
        }
            
        case GLFW_KEY_UP: {
    
            if( resolution >= 25 ) {
                resolution = 25;
                break;
            }
            
            resolution += 1;
            bezierCurve( resolution );
            BsplineCurve( resolution*2 );

            break;
        }
            
        case GLFW_KEY_DOWN: {
        
            if ( resolution <= 5 ) {
                resolution = 5;
                break;
            }
            
            resolution -= 1;
            bezierCurve( resolution );
            BsplineCurve( resolution*2 );
            
            break;
        }
    }
}

void createShaders( void ) {

    simpleProgram.Create("/Users/shreyashriram/Documents/PA5/PA5/resources/shaders/simple.vert", "/Users/shreyashriram/Documents/PA5/PA5/resources/shaders/simple.frag");
}

void createAxisBuffers( void ) {

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    glGenBuffers( 1, &VBO[0] );

    glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( controlPoints ), controlPoints, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( GLfloat ), (GLvoid*)0);
    glEnableVertexAttribArray( 0 );

    glBindVertexArray(0);

}

// PARAMETRIC CURVE FUNCTIONS --------------------------------------------------------------------------------

float evaluateBspline( float u, int i, int k) {
    
    float a, b;
    
    if ( k == 1 ) {
        if ((i <= u) and u < (i+1)) {
            return 1;
        }
        
        else {
            return 0;
        }
    }
    
    else {
        a = (u - i) / (k-1) * evaluateBspline(u, i, k-1);
        b = (i+k - u) / (k-1) * evaluateBspline(u, i+1, k-1);
        
        return a + b;
    }
}

void BsplineCurve( int resolution ) {
    
    bsplinePoints.clear();
    
    float sum_x = 0;
    float sum_y = 0;
    
    float a, b, c;
    
    float u = k; // parametric variable
    float delta = ( u_max - u )/resolution;
    int i = 0; // N (i, k)
    
    while ( u <= u_max ) {
        
        i = int((floor(u) - k) + 1);
        

        a = evaluateBspline(u, i, k);
        b = evaluateBspline(u, i+1, k);
        c = evaluateBspline(u, i+2, k);
        
        sum_x = controlPoints[i-1][0]*a + controlPoints[i][0]*b + controlPoints[i+1][0]*c;
        sum_y = controlPoints[i-1][1]*a + controlPoints[i][1]*b + controlPoints[i+1][1]*c;

        bsplinePoints.push_back( glm::vec2{sum_x, sum_y} );

        u = u + delta;
    }
}

//  --------------------------------------------------------------------------------

void evaluateBezier( float t, int index) {
    
    float sum_x = 0;
    float sum_y = 0;

    int n = numControlPoints - 1;
    
    for (int i = 0; i < numControlPoints; i++) {
        
        sum_x = sum_x + ( controlPoints[i][0] * bezierCoef[i] * (pow(t, i)) * (pow((1-t), (n-i))) );
        sum_y = sum_y + ( controlPoints[i][1] * bezierCoef[i] * (pow(t, i)) * (pow((1-t), (n-i))) );
        
    }
    
    bezierPoints.push_back( glm::vec2{sum_x, sum_y} );
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
    
    bezierPoints.push_back( glm::vec2{controlPoints[numControlPoints-1][0], controlPoints[numControlPoints-1][1]} );
}

void init( void ) {

    // Print some info
    std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
    std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
    std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n";
    std::cout << "GLSL Version:   " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n\n";

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    createPolynomial( numControlPoints );
    bezierCurve( resolution );

    BsplineCurve( resolution );

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

    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "PA5 - Parametric Curves", nullptr, nullptr );
    
    if ( nullptr == window ) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }
    
    int screenWidth, screenHeight; // MAC Compatibility
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );

    // Make the window's context current
    glfwMakeContextCurrent( window );
        
    glViewport( 0.0f, 0.0f, screenWidth, screenHeight );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho( -1.0f, 1.0f, -1.0f, 1.0f, 0, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    
    glewExperimental = GL_TRUE; //
    if ( GLEW_OK != glewInit() ) {
        std::cout << "Failed to initialize GLEW" << std::endl;

        return EXIT_FAILURE;
    }
    
    glfwSetMouseButtonCallback( window, mouseButtonCallback );
    glfwSetKeyCallback( window, keyboardCallback );
    glfwSetCursorPosCallback( window, cursorPositionCallback );

    init();

    // GAME LOOP
    while( !glfwWindowShouldClose( window )) {

        glfwPollEvents();

        // clear Color Buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        simpleProgram.Use();
        GLint line_colorLoc = glGetUniformLocation( simpleProgram.GetID(), "line_color" );

        glBindVertexArray( VAO );
        glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );

        // DRAW CONTROL POINTS & SEGMENTS
        glBufferData( GL_ARRAY_BUFFER , sizeof( controlPoints ), controlPoints, GL_DYNAMIC_DRAW);
        glUniform3f( line_colorLoc, 0.0f, 1.0f, 0.0f );
        glDrawArrays( GL_LINE_STRIP , 0, 6);
        
        glPointSize( 10 );
        glDrawArrays( GL_POINTS, 0, 6 );

        
        // DRAW RESPECTIVE CURVE
        if ( drawBspline )  {
            glUniform3f( line_colorLoc, 1.0f, 0.0f, 0.0f );
            glBufferData( GL_ARRAY_BUFFER, bezierPoints.size() * sizeof(glm::vec2), &bezierPoints[0], GL_STATIC_DRAW );
            glDrawArrays( GL_LINE_STRIP, 0, bezierPoints.size() );
        }

        else {
            glUniform3f( line_colorLoc, 0.0f, 0.0f, 1.0f );
            glBufferData( GL_ARRAY_BUFFER, bsplinePoints.size() * sizeof(glm::vec2), &bsplinePoints[0], GL_STATIC_DRAW );
            glDrawArrays( GL_LINE_STRIP, 0, bsplinePoints.size() );
        }

        glBindVertexArray( 0 );
        glfwSwapBuffers( window );
    }

    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO[0] );

    glfwTerminate();

    return EXIT_SUCCESS;
}
