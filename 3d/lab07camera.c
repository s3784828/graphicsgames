/* Draws a circle using either cartesian or polar coordinates */

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool debug = true;
typedef struct
{
  float x;
  float y;
  float speed;
} MouseRotations;

typedef struct
{
  float deltaX;
  float deltaY;
  float lastX;
  float lastY;
  float x;
  float y;

} Camera;

bool rotateY = true;
MouseRotations rot = {0.0, 0.0, 1.0};
Camera cam = {0.0, 0.0, 0.0, 0.0};

// Utility function for drawing axes
void drawAxes(float len)
{
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(len, 0.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, len, 0.0);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, len);
  glEnd();
}

// Display callback
void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  drawAxes(0.90);
  glPushMatrix();
    // if (rotateY)
    //   glRotatef(rot.x, 0.0, 1.0, 0.0);
    // else
    //   glRotatef(rot.y, 1.0, 0.0, 0.0);
    // glRotatef(cam.deltaX, 0.0, 1.0, 0.0);
    // glRotatef(cam.deltaY, 1.0, 0.0, 0.0);
    glRotatef(cam.x, 0.0, 1.0, 0.0);
    glRotatef(cam.y, 1.0, 0.0, 0.0);
    glutWireTeapot(0.20);
  glPopMatrix();

  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}



void mouseMotion(int button, int state, int x, int y)
{
  //printf("button: %d, state: %d\n", button, state);

  if ()
  {
    float _x = (float) x;
    float _y = (float) y;

  

    cam.deltaX = _x - cam.lastX;
    cam.deltaY = _y - cam.lastY;

    cam.lastX = _x;
    cam.lastY = _y;

    cam.x += cam.deltaX;
    cam.y += cam.deltaY;

    printf("x: %f, y: %f, dx: %f, dy: %f\n", _x, _y, cam.deltaX, cam.deltaY);
  
    glutPostRedisplay();
  }    
  
  // switch(state) 
  // {
  //   case GLUT_DOWN:
  //     if (button == GLUT_LEFT_BUTTON)
  //     {
  //       float _x = (float) x;
  //       float _y = (float) y;

  

  //       cam.deltaX = _x - cam.lastX;
  //       cam.deltaY = _y - cam.lastY;

  //       cam.lastX = _x;
  //       cam.lastY = _y;

  //       cam.x += cam.deltaX;
  //       cam.y += cam.deltaY;

  //       printf("x: %f, y: %f, dx: %f, dy: %f\n", _x, _y, cam.deltaX, cam.deltaY);
  
  //       glutPostRedisplay();
  //     }
  // }


}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 27:
  case 'x':
    if (rotateY)
      rotateY = false;
    else
      rotateY = true;
    break;
  default:
    break;
  }
}


int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutCreateWindow("TeaPotTransformations");
  glutDisplayFunc(display);
  glutMotionFunc(mouseMotion);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}
