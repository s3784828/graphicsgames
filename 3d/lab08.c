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

typedef struct
{
  float x, y, z;
} Vec3;

bool rotateY = true;
MouseRotations rot = {0.0, 0.0, 1.0};
Camera cam = {0.0, 0.0, 0.0, 0.0};
float n = 8;
bool t = false;

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

void drawNormal(Vec3 v0, Vec3 v1, Vec3 v2)
{
  
  Vec3 A = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
  Vec3 B = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

  Vec3 N = {(A.y * B.z) - (A.z * B.y), (A.z * B.x) - (A.x * B.z), (A.x * B.y) - (A.y * B.x)};
  printf("\nAx : %f, Ay : %f, Az : %f\n", A.x, A.y, A.z);
  printf("Bx : %f, By : %f, Bz : %f\n", B.x, A.y, B.z);
  printf("Nx : %f, Ny : %f, Nz : %f\n", N.x, A.y, N.z);
  glBegin(GL_POINTS);
  glColor3f(0, 1, 0);
  glVertex3f(N.x, N.y, N.z);
  glEnd();
}

void drawNormalVector(Vec3 v, float s)
{
  float xn, yn, zn;
  xn = 1;
  yn = 1;
  zn = 1;
  // float len = sqrt((xn * xn) + (yn * yn) + (zn * zn));
  // xn /= len;
  // yn /= len;
  xn *= s;
  yn *= s;
  zn *= s;
  float x1, y1, z1;
  x1 = v.x + xn;
  y1 = v.y + yn;
  z1 = v.z + zn;
  glBegin(GL_LINES);
  glColor3f(0, 1, 0);
  glVertex3f(v.x, v.y, v.z);
  glVertex3f(v.x, y1, v.z);
  glEnd();
}

void drawGrid(float n)
{
  float xStep, x;


  float zStep, z;

  xStep = 2.0 / n;                
  zStep = 2.0 / n; // xStep and zStep are the same, but could be different
  for (int j = 0; j < n; j++) 
  {
    z = -1.0 + j * zStep;
    glPointSize(5);
    if (t)
      glBegin(GL_TRIANGLE_STRIP);
    else
      glBegin(GL_POINTS);
    glColor3f(1.0, 1.0, 1.0);
    z = -1.0 + j * zStep;
    
    for (int i = 0; i <= n; i++) 
    {
    /* What are these calculations doing? */
    x = -1.0 + i * xStep;
    
    glVertex3f(x, 0.0, z);
    }
    for (int i = n; i >= 0; i--)
    {
     x = -1.0 + i * xStep; 
     glVertex3f(x, 0.0, z + zStep);
    }
    
  }
  glEnd();
}

void drawSin(float a, float n)
{
  float xStep, x;
  float zStep, z;
  float y, y1;

  float k = 2 * M_PI / a;

  xStep = 2.0 / n;                
  zStep = 2.0 / n; // xStep and zStep are the same, but could be different
  for (int j = 0; j < n; j++) 
  {
    //z = -1.0 + j * zStep;
    glPointSize(5);
    if (t)
      glBegin(GL_QUAD_STRIP);
    else
      glBegin(GL_POINTS);
    
    glColor3f(1.0, 1.0, 1.0);
    z = -1.0 + j * zStep;
    
    for (int i = 0; i <= n; i++) 
    {
    /* What are these calculations doing? */
    x = -1.0 + i * xStep;
    y = a * sinf(k * x);
    
    glVertex3f(x, y, z);
    
    printf("x: %f, y: %f, z: %f\n", x, y, z);
    }
  
  }
  glEnd();
}


void drawNormals()
{
  float x, z, zStep, xStep;
  xStep = 2.0 / n;                
  zStep = 2.0 / n; 
  for (int j = 0; j <= n; j++) 
  {
  z = -1.0 + j * zStep;
  
    for (int i = 0; i <= n; i++) 
    {
    /* What are these calculations doing? */
    x = -1.0 + i * xStep;
    Vec3 v0 = {x, 0.0, z};
    
    drawNormalVector(v0, 0.05);
    }
  }
}

// Display callback
void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glLoadIdentity();
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glPushMatrix();
    glRotatef(cam.x, 0.0, 1.0, 0.0);
    glRotatef(cam.y, 1.0, 0.0, 0.0);
    drawAxes(0.75);
    drawSin(0.5, n);
    //drawGrid(n);
    //drawNormals();
  glPopMatrix();

  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

void mouseMotion(int x, int y)
{
  float _x = (float) x;
  float _y = (float) y;

  cam.deltaX = _x - cam.lastX;
  cam.deltaY = _y - cam.lastY;

  cam.lastX = _x;
  cam.lastY = _y;

  cam.x += cam.deltaX;
  cam.y += cam.deltaY;

  glutPostRedisplay();
}    

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case '=':
    n += 1;
    glutPostRedisplay();
    break;
  case '-':
    n -= 1;
    glutPostRedisplay();
    break;
  case 't':
    if (t)
      t = false;
    else
      t = true;
    glutPostRedisplay();
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
