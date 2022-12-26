// TODO: Make Parametric Curve and the Control Points different colors
// TODO: Add B-Spline Curve Option
// TODO: Toggle between Bezier and B-Spline Curve
// TODO: Switch bezierPoints to Vector

#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <cmath>
#include <vector>
#include <array>

#include <GL/glew.h>
#include <GLUT/glut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// Window Dimensions
const int InitWindowWidth  = 800;
const int InitWindowHeight = 800;

//
const int numControlPoints = 6;
const int max_Bez_Point = 100;
int bezierCoef[numControlPoints];
//std::vector<glm::vec3> bezierPoints;

//
float a, b, dist;
int control_point = NULL;
bool move_point = false;

float bezierPoints[100][3] = { 0 };

float controlPoints[numControlPoints][3] = {
    
    { -0.8,  0.3, 0.0 },
    { -0.5, -0.8, 0.0 },
     { 0.0, -0.5, 0.0 },
     { 0.2,  0.5, 0.0 },
     { 0.8,  0.8, 0.0 },
     { 0.6, -0.3, 0.0 }
};

GLuint vertex_buffer;

//=================================================================================================
// UTILS
//=================================================================================================

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

// ***  Note ran into trouble running support code for shaders and buffers
//      Followed tutorial online about shaders and buffers to produce following functions:
static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader( type );
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char*)alloca(length * sizeof(char));
        
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Error in shader." << std::endl;
        std::cout << message << std::endl;
        
        glDeleteShader(id);
        
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader( GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader( GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}

//=================================================================================================
// CALLBACKS
//=================================================================================================

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------


void idle_func()
{
//    uncomment below to repeatedly draw new frames
    glutPostRedisplay();
}

void reshape_func( int width, int height )
{
    glViewport( 0, 0, width, height );
    glutPostRedisplay();
}

void keyboard_func( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 'w':
        {
            break;
        }

        case 'a':
        {
            break;
        }

        case 's':
        {
            break;
        }

        case 'd':
        {
            break;
        }

        // Exit on escape key press
        case '\x1B':
        {
            exit( EXIT_SUCCESS );
            break;
        }
    }

    glutPostRedisplay();
}

void key_released( unsigned char key, int x, int y )
{
}

void key_special_pressed( int key, int x, int y )
{
}

void key_special_released( int key, int x, int y )
{
}

void mouse_func( int button, int state, int x, int y )
{
    // convert x and y coordinates
    a  = - 1 + ((x/float(InitWindowWidth))*2);
    b  =  1 - ((y/float(InitWindowHeight))*2);
    
    // Button Clicked - Check if moving point?
    if ( button == GLUT_LEFT_BUTTON && state == 0 && move_point == false) {

        // Check if there is a
        for (int i = 0; i < 6; i++) {
            dist = distance(a, b, controlPoints[i][0], controlPoints[i][1]);
            if (dist <= 0.02) {
                std::cout << "point " << i <<std::endl;
                
                control_point = i;
                move_point = true;
                            
                break;
            }
        }
    }
    
    else if (button == GLUT_LEFT_BUTTON && state == 0 && move_point == true ) {
        move_point = false;
        control_point = NULL;
    }
}

void passive_motion_func( int x, int y )
{
    a  = - 1 + ((x/float(InitWindowWidth))*2);
    b  =  1 - ((y/float(InitWindowHeight))*2);
    
    if (move_point){
        
        controlPoints[control_point][0] = a;
        controlPoints[control_point][1] = b;
    }
}

void active_motion_func( int x, int y )
{

}

//=================================================================================================
//                                              CURVES
//=================================================================================================

void evaluateBezier( float t, int index) {
    
    float sum_x = 0;
    float sum_y = 0;

    int n = numControlPoints - 1;
    
    for (int i = 0; i < numControlPoints; i++) {
        
        sum_x = sum_x + ( controlPoints[i][0] * bezierCoef[i] * (pow(t, i)) * (pow((1-t), (n-i))) );
        sum_y = sum_y + ( controlPoints[i][1] * bezierCoef[i] * (pow(t, i)) * (pow((1-t), (n-i))) );
        
    }
    
    bezierPoints[index][0] = sum_x;
    bezierPoints[index][1] = sum_y;
}

void bezierCurve(int numPoints) {
    
    float t = 0;
    int i = 0;
    float delta = float(1)/float(numPoints);


    while (t <= (1.0+delta)) {
        
        evaluateBezier(t, i);
        
//        std::cout<< i << ": " << bezierPoints[i][0] << ", " << bezierPoints[i][1] <<std::endl;
        t = t + delta;
        i = i+1;
    }
}

//=================================================================================================
//                                              RENDERING
//=================================================================================================
int numBezierPoints = 20;

void bufferSetup () {

    glGenBuffers( 1, &vertex_buffer );
    glBindBuffer( GL_ARRAY_BUFFER , vertex_buffer);
    glBufferData( GL_ARRAY_BUFFER , sizeof( controlPoints ), controlPoints, GL_DYNAMIC_DRAW);
        
    
    glEnableVertexAttribArray( 0 );
    glEnable( GL_PROGRAM_POINT_SIZE );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    
    std::string vertexShader =
        "#version 120\n"
        "\n"
        "attribute vec3 position;\n"
        "\n"
        "void main() {\n"
        "   gl_Position = vec4(position, 1.0);\n"
        "   gl_PointSize = 10.0;\n"
        "}\n";
    
    std::string fragmentShader =
        "#version 120\n"
        "\n"
        "uniform vec3 uColor;\n"
        "\n"
        "void main() {\n"
        "   gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "}\n";
    
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);
}

void display_func( void ) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //  Reference Coordinate System
//    glColor3f( 0.0f, 0.0f, 1.0f );
//    glBegin( GL_LINES );
//
//        // X Axis
//        glVertex2f( -1.0f, 0.0f );
//        glVertex2f( 1.0f, 0.0f );
//
//        // Y Axis
//        glVertex2f( 0.0f, -1.0f );
//        glVertex2f( 0.0f, 1.0f );
//
//    glEnd();
    
    bufferSetup();
    glDrawArrays( GL_POINTS , 0, 6);
    glDrawArrays( GL_LINE_STRIP , 0, 6);
    
    bezierCurve(numBezierPoints);
    glBindBuffer( GL_ARRAY_BUFFER , vertex_buffer);
    glBufferData( GL_ARRAY_BUFFER , sizeof( bezierPoints ), bezierPoints, GL_DYNAMIC_DRAW);
    
    
    glDrawArrays( GL_LINE_STRIP , 0, numBezierPoints+1);


    
    glutSwapBuffers();
}

//=================================================================================================
//                                              INIT
//=================================================================================================

void init( void )
{
    // Print some info
    std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
    std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
    std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n\n";

    createPolynomial( numControlPoints );
//    bezierCurve(25);
////
//    for (int i = 0; i < 26; i++) {
//        std::cout << bezierPoints[i][0] << ", " << bezierPoints[i][1] << std::endl;
//    }
//
    // Set the background color
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    std::cout << "Finished initializing...\n\n";

}

//=================================================================================================
//                                              MAIN
//=================================================================================================

int main( int argc, char** argv )
{
    
    glutInit( &argc, argv );

    glutInitWindowPosition( 100, 100 );
    glutInitWindowSize( InitWindowWidth, InitWindowHeight );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

    glutCreateWindow( "CSE-170 Computer Graphics" );

    glutDisplayFunc( display_func );
    glutIdleFunc( idle_func );
    glutReshapeFunc( reshape_func );
    glutKeyboardFunc( keyboard_func );
    glutKeyboardUpFunc( key_released );
    glutSpecialFunc( key_special_pressed );
    glutSpecialUpFunc( key_special_released );
    glutMouseFunc( mouse_func );
    glutMotionFunc( active_motion_func );
    glutPassiveMotionFunc( passive_motion_func );
    
    // GLEW Initialization, create context
    GLenum err = glewInit();
    
    if ( GLEW_OK != err ) {
        
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
 
    }
    
    init();

    glutMainLoop();

    return EXIT_SUCCESS;
}
