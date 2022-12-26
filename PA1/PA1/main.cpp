#define GL_SILENCE_DEPRECATION

#include <GLUT/glut.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <array>

class Vertex {
public:
    double x;
    double y;
    
    Vertex() {}
    Vertex(double x, double y) : x(x), y(y) {}

    void displayVertex() {
        std::cout << "(" << x << ", " << y << ") ";
    }
};

class Triangle {
public:
    Vertex A;
    Vertex B;
    Vertex C;
    
    Triangle() {}
    Triangle(Vertex A, Vertex B, Vertex C) : A(A), B(B), C(C) {};
    
    void displayTriangle() {
        A.displayVertex();
        B.displayVertex();
        C.displayVertex();
        std::cout << "\n";
    }
};

class Letter {
public:
    
    int order;
    int norm_scale;
    std::vector<Triangle> triangles;
    
    Letter() {}
    
    Letter(int order, std::string filepath) : order(order), norm_scale(36) {
        triangles = parseVertices(filepath);
    };
    
    
    std::vector<Triangle> parseVertices(std::string filepath) {
        /*
         This function parses a list of vertices (x, y) from a csv file and converts them into a vector of triangles.
         
         Input:
            filepath (string) - full, explicit path to csv containing coordinates defining letter. Each line of csv has form "x, y".
         
         Output:
            tri_vect (vector<Triangle>) - Appropriate vertices are grouped together to create triangles, whoch are appended to vector.
        */
        
        std::ifstream inputFile;
        inputFile.open(filepath);
        

        std::string line = "";
        
        std::vector<Triangle> tri_vect;
        std::array<Vertex, 3> vert_holder;
        double counter = 0;
        
        double x;
        double y;
        
        while(getline(inputFile, line)) {
            
            if (counter == 3) {
                Triangle temp_triangle(vert_holder[0], vert_holder[1], vert_holder[2]);
                tri_vect.push_back(temp_triangle);
                
                counter = 0;
            }
            
            std::string tempString = "";
            std::stringstream inputString(line);

            getline(inputString, tempString, ',');
            x = atof(tempString.c_str());

            tempString = "";

            getline(inputString, tempString, ',');
            y = atof(tempString.c_str());
            
            line = "";
            
            // Scale down
            // Note: Original letters were created to fit in 8x12 grid. Needs to be scaled down appropriately to fit inside GLUT window and coordinates.
            x = x/norm_scale;
            y = y/norm_scale;
            
            // Translate Left in order of name.
            x = x + (-0.5 + (0.25*order));
            
            Vertex v(x, y);
            vert_holder[counter++] = v;
        }
        
        //    Creates and pushes last triangle from array.
        Triangle temp_triangle(vert_holder[0], vert_holder[1], vert_holder[2]);
        tri_vect.push_back(temp_triangle);
        

        return tri_vect;
    };
};

//=================================================================================================
// GLOBAL VARIABLES DECLARED
//=================================================================================================

std::vector<Letter> my_name;

// translate word
double tran_x = 0;
double tran_y = 0;

// change color
bool changeColor = false;

double mouse_r = 1;
double mouse_g = 1;
double mouse_b = 1;

// scale word
double scale = 1;

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
    //uncomment below to repeatedly draw new frames
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
        case ' ':
        {
            //  PRINT CONTROL/OPTION MENU
            
            std::cout << "\n\n\n\n";
            std::cout << "                  ------------------------ \n";
            std::cout << "                  YOU HAVE OPENED THE MENU \n";
            std::cout << "                  ------------------------ \n";

            std::cout << "\n                  Movement Control Keys \n";
            std::cout << "                          W: ↑            \n";
            std::cout << "                          S: ↓            \n";
            std::cout << "                          D: →            \n";
            std::cout << "                          A: ←            \n";
        
            std::cout << "\n                  Random Color w Mouse \n";
            std::cout << "                          C: On/Off      \n";
            
            std::cout << "\n                  Scale word with Input \n";
            std::cout << "                          Q: Input (0,2) \n";
            
            std::cout << "\n                  Open Menu: Space Bar \n";
            std::cout << "                  ------------------------ \n";
            std::cout << "                  ------------------------ \n";

            break;
        }
            
        case 'w':
        {
            //  MOVE UP
            tran_y = tran_y + 0.2;
            break;
        }

        case 'a':
        {
            //  MOVE LEFT
            tran_x = tran_x - 0.2;
            break;
        }

        case 's':
        {
            //  MOVE DOWN
            tran_y = tran_y - 0.2;
            break;
        }

        case 'd':
        {
            //  MOVE RIGHT
            tran_x = tran_x + 0.2;
            break;
        }
            
        case 'c':
        {
            //  SWITCH ON/OFF COLOR CHANGE BY MOUSE (see passive_motion_func())
            changeColor = !changeColor;
            break;
        }
            
        case 'q':
        {
            //  SCALE WORD WITH USER INPUT
            double temp;
            
            std::cout << "Type a number (0, 2): ";
            std::cin >> temp;
    
            std::cout << "Scale by " << temp << "\n";
            scale = temp;
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

void passive_motion_func( int x, int y )
{
    // If "C" is pressed, changeColor is toggled. If True, then based on cursor position (R,G,B) values change. When changeColor is toggled False, word changes back to default (1,1,1)
    if (changeColor){
        
        if (x > 400) {
            mouse_r = 0;
            mouse_g = ((double) x)/800;
        }
        else if (x == 400) {
            mouse_g = 0;
            mouse_r = 0;
        }
        
        else {
            mouse_g = 0;
            mouse_r = ((double) x)/800;
        }
        
        mouse_b = double (x/600);
    }
    else {
        mouse_r = 1;
        mouse_g = 1;
        mouse_b = 1;
    }
}

//=================================================================================================
// RENDERING
//=================================================================================================

void render_triangles(Letter l) {
    
    for (Triangle t : l.triangles) {
        glBegin( GL_TRIANGLES );
            glColor3f( (float) mouse_r, (float) mouse_g, (float) mouse_b );
            glVertex2f( scale*((float) t.A.x + tran_x), scale*((float) t.A.y + tran_y));
            glVertex2f( scale*((float) t.B.x + tran_x), scale*((float) t.B.y + tran_y));
            glVertex2f( scale*((float) t.C.x + tran_x), scale*((float) t.C.y + tran_y));
        glEnd();
    }
}


void display_func( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    for (Letter L : my_name) {
        render_triangles(L);
    }

    glutSwapBuffers();
}

//=================================================================================================
// INIT
//=================================================================================================
void init( void )
{
    // Print some info
    std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
    std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
    std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n\n";

    // Set the background color
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    std::cout << "Finished initializing...\n\n";
    
    std::cout << "                  ------------------------ \n";
    std::cout << "                  HIT SPACEBAR FOR MENU \n";
    std::cout << "                  ------------------------ \n";
    
}


//=================================================================================================
// MAIN
//=================================================================================================
int main( int argc, char** argv )
{
    //    Instantiate Letters
    Letter S(0, "Letters/S.csv");
    my_name.push_back(S);
    Letter H(1, "Letters/H.csv");
    my_name.push_back(H);
    Letter R(2, "Letters/R.csv");
    my_name.push_back(R);
    Letter E(3, "Letters/E.csv");
    my_name.push_back(E);
    

    glutInit( &argc, argv );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 800, 600 );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    
    glutCreateWindow( "CSE-170 Computer Graphics" );
    
    glutDisplayFunc( display_func );
    glutIdleFunc( idle_func );
    glutReshapeFunc( reshape_func );
    glutKeyboardFunc( keyboard_func );
    glutPassiveMotionFunc( passive_motion_func );

    init();


    glutMainLoop();

    return EXIT_SUCCESS;
}
