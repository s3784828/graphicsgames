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

enum { cartesian, parametric } coordSystem = { parametric };
enum { points, lines } renderMode = { lines };

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

// Cartesian coordinates
void drawCircleCartesian(float r, int n)
{
  float x, y;

  if (debug)
    printf("drawCircleCartesian: %d\n", n);

  glColor3f(1.0, 1.0, 1.0);
  if (renderMode == points) {
    glPointSize(2.0);
    glBegin(GL_POINTS);
  } else {
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
  }

  n /= 2;

  for (int i = 0; i < n; i++) {
    x = ((i / (float)n - 0.5) * 2.0) * r;
    y = sqrt(r * r - x * x);
    if (debug)
      printf("x y: %f %f\n", x, y);
    glVertex2f(x, y);
    if (renderMode == points) {
      y = -y;
      if (debug)
	printf("x y: %f %f\n", x, y);
      glVertex2f(x, y);
    }
  }

  if (renderMode == lines)
    for (int i = n; i > 0; i--) {
      x = (i / (float)n - 0.5) * 2.0 * r;
      y = -sqrt(r * r - x * x);
      if (debug)
	printf("x y: %f %f\n", x, y);
      glVertex2f(x, y);
    }

  glEnd();

#if 0
  // Ignore
#endif
}

// Parametric coordinates
void drawCircleParametric(float r, int n)
{
  float x, y;
  float theta;

  if (debug)
    printf("drawCircleParametric: %d\n", n);

  glColor3f(1.0, 1.0, 1.0);
  if (renderMode == points) {
    glPointSize(2.0);
    glBegin(GL_POINTS);
  } else {
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
  }
  for (int i = 0; i < n; i++) {
    theta = i / (float)n * 2.0 * M_PI;
    x = r * cosf(theta);
    y = r * sinf(theta);
    if (debug)
      printf("x y: %f %f", x, y);
    glVertex2f(x, y);
  }
  glEnd();
}

void drawTangentQaud(float x, float y, float s)
{
  float xt, yt;
  xt = 1;
  yt = 2 * x;
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

void drawNormalQaud(float x, float y, float s)
{
  float xn, yn;
  xn = -(2 * x);
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

void drawVector(float x, float y, float a, float b, float s, bool normalize)
{
  glPointSize(2.0);
  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  //printf("x1 = %f, y1 = %f\n", x1, y1);
  //glVertex2f(x1, y1);
  float len = sqrt(pow(a, 2) + pow(b, 2));

  //printf("length = %f\n", len);
  a /= len; // normalise
  b /= len;

  float x1 = x + a; // calc end (offset from start)
  float y1 = y + a;
  //printf("length = %f, a = %f, x = %f, x1 = %f, \n", len, a, x, x1);

  glVertex2f(x, y); // start
  glVertex2f(x1, y1); // end

  glEnd();
}

void drawQaudratic(float segments)
{
    float left = -1.0, right = 1.0;
    float range = right - left;
    float stepSize = range / segments;

    glPointSize(4.0);
    glBegin(GL_POINTS);
    glColor3f(1, 1, 1);
    float x, y;

    for (float i = 0; i <= segments; i++)
    {
        x = i * stepSize + left;
        y = pow(x, 2);
        glVertex2f(x, y);
    }
    glEnd();

    for (float i = 0; i <= segments; i++)
    {

        //printf("segment: %f\n", i);
        x = i * stepSize + left;
        y = pow(x,2);
        drawTangentQaud(x, y, 0.05);
    }

      for (float i = 0; i <= segments; i++)
    {

        //printf("segment: %f\n", i);
        x = i * stepSize + left;
        y = pow(x,2);
        drawNormalQaud(x, y, 0.05);
    }
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

  for (float i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = a * sinf(k * x);
    glVertex2f(x, y);

    //drawVector(x, y, 0.15, 0.15, 0, true);
  }
  glEnd();
  for (float i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = a * sinf(k * x);
    drawTangentSin(x, y, a, k, 0.05);
  }
  for (float i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = a * sinf(k * x);
    drawNormalSin(x, y, a, k, 0.05);
  }
}

// Display callback
void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawAxes(1.0);
  drawSin(1, n);
  drawQaudratic(n);

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
  glutMainLoop();
}
