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

const int milleseconds = 1000;

enum { cartesian, parametric } coordSystem = { parametric };
enum { points, lines } renderMode = { lines };

typedef struct {float t, lastT, dt;} Global;
Global g = {0.0, 0.0, 0.0 };

bool debug = true;

int n = 32;

// Utility function for drawing axes
void drawAxes(float len)
{
#if 0
  glPushAttrib(GL_CURRENT_BIT);
#endif
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
#if 0
  glPopAttrib();
#endif
}

void update()
{
  g.t = glutGet(GLUT_ELAPSED_TIME);
  g.t /= 1000.0;

  g.dt = g.t - g.lastT;

  g.lastT = g.t;

  glutPostRedisplay();
}

void drawTangentSin(float x, float y, float a, float k, float s)
{
  float xt, yt;
  xt = 1;
  yt = a * k * cosf(k * x);
  float len = sqrt(pow(xt, 2) + pow(yt, 2));
  xt /= len;
  yt /= len;
  //float len_1 = sqrt(pow(xt, 2) + pow(yt, 2));
  //printf("length = %f\n", len_1);
  xt *= s;
  yt *= s;
  float x1, y1;
  x1 = x + xt;
  y1 = y + yt;

  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex2f(x, y);
  glVertex2f(x1, y1);
  glEnd();
}

void drawNormalSin(float x, float y, float a, float k, float s)
{
  float xn, yn;
  xn = -(a * k * cosf(k * x));
  yn = 1;
  float len = sqrt((xn * xn) + (yn * yn));
  xn /= len;
  yn /= len;
  xn *= s;
  yn *= s;
  float x1, y1;
  x1 = x + xn;
  y1 = y + yn;
  glBegin(GL_LINES);
  glColor3f(0, 1, 0);
  glVertex2f(x, y);
  glVertex2f(x1, y1);
  glEnd();
}

void drawSin(float a, float segments)
{
  float left = -1.0, right = 1.0;
  float range = right - left;
  float stepSize = range / segments;

  glPointSize(2.0);
  glBegin(GL_POINTS);
  glColor3f(1, 1, 1);

  float x, y;
  float k = 2 * M_PI / a;
  float interval = 4;
  float w = M_PI / interval;

  for (float i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = a * sinf(k * x + w * g.t);
    glVertex2f(x, y);

    //drawVector(x, y, 0.15, 0.15, 0, true);
  }
  glEnd();
  // for (float i = 0; i <= segments; i++)
  // {
  //   x = i * stepSize + left;
  //   y = a * sinf(k * x);
  //   drawTangentSin(x, y, a, k, 0.05);
  // }
  // for (float i = 0; i <= segments; i++)
  // {
  //   x = i * stepSize + left;
  //   y = a * sinf(k * x);
  //   drawNormalSin(x, y, a, k, 0.05);
  // }
}

// Display callback
void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawAxes(1.0);
  drawSin(1, n);
  // drawQaudratic(n);



  // if (coordSystem == cartesian)
  //   drawCircleCartesian(1.0, n);
  // else if (coordSystem == parametric)
  //   drawCircleParametric(1.0, n);

  // Always check for gl errors
  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

// Keyboard callback
void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 't':
    printf("time: %f\n", g.t);
  case 'c':
    if (coordSystem == cartesian)
      coordSystem = parametric;
    else
      coordSystem = cartesian;
    break;
  case 'm':
    if (renderMode == lines)
      renderMode = points;
    else
      renderMode = lines;
    break;
  case 'd':
    debug = !debug;
    break;
  case '+':
    n *= 2;
    break;
  case '-':
    n /= 2;
    if (n < 2)
      n = 2;
    break;
  case 27:
    // ESCAPE key
    exit (0);
    break;
  }
  glutPostRedisplay();
}


int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutCreateWindow("Circle");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(update);
  glutMainLoop();
}
