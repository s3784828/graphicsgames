/*
 * robot-arm-2D.c
 *
 * This program shows how to composite modeling transformations
 * to draw translated and rotated hierarchical models.
 * Interaction:  pressing the S/s and E/e keys, alters shoulder and elbow
 * joint rotation angles, and L/l, R/r the gripper plate positions.
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef LINE_SIZE
#define LINE_SIZE 0.25
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

const float sphereSize = 0.25;

float toRad(float angle)
{
  return angle * (PI / 180);
}

typedef struct 
{
  float x; 
  float y; 
} v2;

typedef struct 
{
  v2 r; 
  v2 r0;
  v2 v; 
  float radius;
} S;

S sp = {{-1.0f,0}, {1.0, 0.0}, {0.10f,0}, sphereSize / 2};
S sp1 = {{0.0,0.0}, {0.0, 0.0}, {0.0,0.0}, sphereSize / 2};

void updateProjectileStateNumerical(float dt)
{
  //update initial position
  sp.r0 = sp.r;
  // Position
  sp.r.x += sp.v.x * dt;
  sp.r.y += sp.v.y * dt;
  
  
  // Velocity
  //sp.v.y += g * dt;
}

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

void drawSphere(float x, float y) 
{
  glPushMatrix();
    glTranslatef(x, y, 0.0);
    glColor3f(1, 1, 1);
    glutSolidSphere(sphereSize / 2, 20, 20);
  glPopMatrix();
}

float getDistance() 
{
  return sqrt((sp1.r0.x - sp.r0.x) * (sp1.r0.x - sp.r0.x) + (sp1.r.x - sp.r.x) * (sp1.r.x - sp.r.x));
}

void colliderChecker()
{
  if (getDistance() <= sp.radius + sp1.radius)
  {
    printf("Collided\n");
  }
}

void update(void)
{
  static float lastT = 0.0;
  float t, dt;

  t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  
  dt = t - lastT;
  updateProjectileStateNumerical(dt);
  //printf("Distance %f\n", getDistance());

  colliderChecker();

  lastT = t;

  glutPostRedisplay();
}

void display(void)
{
  int err;

  //glClear (GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawAxes(1);
  drawSphere(sp.r.x, sp.r.y);
  drawSphere(sp1.r.x, sp1.r.y);
  

  glutSwapBuffers();

  if ((err = glGetError()) != GL_NO_ERROR) {
    printf("display: %s\n",gluErrorString(err));
  }
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key) {

  case 'f':
  case 27:
    exit(0);
    break;
  default:
    break;
  }
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize (500, 500);
  glutInitWindowPosition (100, 100);
  glutCreateWindow ("lab05collisions");
  glutIdleFunc(update);
  glutDisplayFunc(display);
  //glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
