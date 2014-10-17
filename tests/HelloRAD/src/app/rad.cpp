// rad.cpp : Initialize GLUT, initalize the RAD emulation layer, and test RAD

#include "glut.h"

void reshape(int w, int h)
{
}

void display(void)
{
    extern void TestRAD(void);
    TestRAD();

    glutSwapBuffers();
}

void glutKey( unsigned char key, int x, int y )
{
    if( key == '\033' ) {
        exit(0);
    }
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, (char **)argv);
    glutInitDisplayString("double depth rgba stencil");
    glutInitWindowSize (500, 500);
    glutCreateWindow ((char *)argv[0]);
    glutSetWindowTitle("RAD Test App");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(glutKey);

    void InitRAD();
    InitRAD();

    glutMainLoop();

	return 0;
}

