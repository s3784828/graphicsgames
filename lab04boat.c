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

float cannonRotation = 45.0;

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

void drawHull() {
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.5, -0.25, 0.0);
  glVertex3f(0.5, -0.25, 0.0);
  glVertex3f(1.0, 0.25, 0.0);
  glVertex3f(-1.0, 0.25, 0.0);
  glEnd();
}

void drawBridge() {
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.25, -0.25, 0.0);
  glVertex3f(0.25, -0.25, 0.0);
  glVertex3f(0.25, 0.25, 0.0);
  glVertex3f(-0.25, 0.25, 0.0);
  glEnd();
}

void drawCannon() {
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.25, -0.05, 0);
  glVertex3f(0.25, -0.05, 0.0);
  glVertex3f(0.25, 0.05, 0.0);
  glVertex3f(-0.25, 0.05, 0.0);
  glEnd();
}

// Display callback
void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  drawAxes(1.0);

  glPushMatrix();
    //Hull
    drawHull();
    //Bridge
    glTranslatef(0.0, 0.5, 0.0);
    drawBridge();
    //CannonSocket
    glTranslatef(0.5, -0.25, 0.0);
    glRotatef(cannonRotation, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.25, 0.0, 0.0);
    drawCannon();
  glPopMatrix();
  // glPushMatrix();
  //   glTranslatef(0.5f, 0.0f, 0.0f);
  //   glRotatef(60.0f, 1.0f, 0.0f, 0.0f);
  //   glScalef(0.2f, 0.2f, 0.2f);
  //   glutWireTeapot(1.0);
  // glPopMatrix();




  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  case 'a':
    cannonRotation += 0.10;
    glutPostRedisplay();
    break;
  case 'd':
    cannonRotation -= 0.10;
    glutPostRedisplay();
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
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}
