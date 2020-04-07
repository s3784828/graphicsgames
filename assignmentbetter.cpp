/* ASSIGNMENT 1 TRISTAN */

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

/*
* CONSTANTS
*/

const float g = -0.25;
const float boatScale = 0.5;
float numericalInc = 0.5;
float bAngle = 0.0;

/*
* CLASS DECLARATIONS
*/

typedef struct
{
  float amplitude;
  float k;
  float w;
} SinGraphUtility;

typedef struct
{
  float x;
  float y;
} Vector2;

typedef struct
{
  float mag;
  float angle;
} PolarCoordinate;

typedef struct 
{
  Vector2 r0;
  Vector2 v0;
  Vector2 r;
  Vector2 v;
} Projectile;

typedef struct
{
  float t;
  float startTime;
} TimeUtility;

typedef struct
{
  bool fire;
  float xPos;
} InputUtility;

/*
* CLASS OBJECTS
*/

SinGraphUtility sinUtil = {0.25, 2 * PI / 1 , PI / 4};
TimeUtility timeUtil = {0.0, 0.0};
PolarCoordinate pCoord = {0.35, 0.0};
InputUtility input = {false, 0.0};
Projectile p1 = {{0,0}, {0,0}, {0,0}, {0,0}};

/*
* HELPER METHODS
*/

float toDeg(float angle)
{
  return angle * (180 / PI);
}

float toRad(float angle)
{
  return angle * (PI / 180);
}

float sinValue(float x)
{
  return sinUtil.amplitude * sinf(sinUtil.k * x + sinUtil.w * timeUtil.t);
}

float derivativeSin(float x)
{
  return sinUtil.amplitude * sinUtil.k * cosf(sinUtil.k * x + sinUtil.w * timeUtil.t);
}

/*
* CALCULATION METHODS
*/

float findAngle(float x)
{
  float y, xt, yt, len;
  xt = 1;
  yt = derivativeSin(x);
  len = sqrt(xt * xt + yt * yt);
  xt /= len;
  yt /= len;

  float x1, y1;
  x1 = x + xt;
  y1 = y + yt;

  float angle = toDeg(atan2((y1 - y),(x1 - x)));

  return angle / 2;
}

float findAngle1(float x)
{
  float y, xt, yt, len;
  xt = 1;
  yt = derivativeSin(x);
  len = sqrt(xt * xt + yt * yt);
  xt /= len;
  yt /= len;

  float x1, y1;
  x1 = x + xt;
  y1 = y + yt;

  float angle = toDeg(atan2((y1 - y),(x1 - x)));

  return angle;
}

Vector2 calculateR0(float hyp, float xInc, float yInc, float s)
{
  hyp *= s;
  // float x2 = xInc;
  // float y2 = yInc;
  xInc *= s;
  yInc *= s;
  xInc += input.xPos;
  float sa = atanf(xInc * yInc);
  //yInc += sinValue(input.xPos);
  //return {hyp * cosf(toRad((pCoord.angle) + findAngle1(input.xPos))) + input.xPos, hyp * sinf(toRad((pCoord.angle) + findAngle1(pCoord.angle))) + sinValue(input.xPos)};
  //return {hyp * cosf(toRad((pCoord.angle))) + xInc, hyp * sinf(toRad((pCoord.angle))) + yInc};
  //return {hyp * cosf(toRad((pCoord.angle) + bAngle)) + xInc, hyp * sinf(toRad((pCoord.angle) + bAngle)) + yInc};
  //return {hyp * cosf(toRad(pCoord.angle) + xInc * cosf(toRad(bAngle))) + xInc, hyp * sinf(toRad((pCoord.angle + yInc * sinf(toRad(bAngle)) ))) + yInc};
  return {hyp * cosf(toRad((pCoord.angle + sa + bAngle))) + xInc, hyp * sinf(toRad((pCoord.angle + sa + bAngle))) + yInc};
}

Vector2 calculateV0() 
{
  return {pCoord.mag * cosf(toRad(pCoord.angle + findAngle1(input.xPos))), pCoord.mag * sinf(toRad(pCoord.angle + findAngle1(input.xPos)))};
  //return {pCoord.mag * cosf(toRad(pCoord.angle + bAngle)), pCoord.mag * sinf(toRad(pCoord.angle + bAngle))};
}


void updateProjectileStateNumerical(float dt) 
{
  // Position
  p1.r.x += p1.v.x * dt;
  p1.r.y += p1.v.y * dt;

  // Velocity
  p1.v.y += g * dt;
  printf("p1.v.x: %f p1.v.y %f p1.r.x: %f p1.r.y %f dt: %f\n", p1.v.x, p1.v.y, p1.r.x, p1.r.y, dt);
}

void updateProjectileStateInitial() 
{
  p1.r0 = calculateR0(0.5, 0.5, 0.25, boatScale);
  //p1.r0.x += 0.75;
  //p1.r0.y += 0.25; 
  p1.v0 = calculateV0();
}

void updateProjectileState()
{
  p1.r = p1.r0;
  // p1.r.x += 0.75 * 0.10;
  // p1.r.y += 0.25 * 0.10;
  p1.v = p1.v0;
}

/*
* DRAWING METHODS
*/

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

void drawSin(float segments)
{
  float left = -1.0;
  float right = 1.0;
  float range = right - left;
  float stepSize = range / segments;

  glPointSize(10);
  glBegin(GL_QUAD_STRIP);
  glColor3f(0, 0, 1);
  float x, y;
  for (int i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = sinValue(x);
    glVertex3f(x, y, 0.0);
    glVertex3f(x, -1.0, 0.0);
  }
  glEnd();
}

void drawHull()
{
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.5, -0.25, 0.0);
  glVertex3f(0.5, -0.25, 0.0);
  glVertex3f(1.0, 0.25, 0.0);
  glVertex3f(-1.0, 0.25, 0.0);
  glEnd();
}

void drawBridge()
{
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.25, -0.25, 0.0);
  glVertex3f(0.25, -0.25, 0.0);
  glVertex3f(0.25, 0.25, 0.0);
  glVertex3f(-0.25, 0.25, 0.0);
  glEnd();
}

void drawCannon()
{
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.25, -0.05, 0);
  glVertex3f(0.25, -0.05, 0.0);
  glVertex3f(0.25, 0.05, 0.0);
  glVertex3f(-0.25, 0.05, 0.0);
  glEnd();
}

void drawSqaure(float len)
{
  glBegin(GL_LINE_LOOP);
  glColor3f(1, 0, 0);
  glVertex3f(len, len, 0.0);
  glVertex3f(len, -len, 0.0);
  glVertex3f(-len, -len, 0.0);
  glVertex3f(-len, len, 0.0);
  glEnd();
}

void drawCB()
{
  if (input.fire) 
  {
    glPushMatrix();
      glTranslatef(p1.r.x, p1.r.y, 0.0);
      drawSqaure(0.015);
    glPopMatrix();
  }
  else 
  {
    glPushMatrix();
      //glTranslatef(0.75 * 0.10, 0.25 * 0.10, 0.0);
      glTranslatef(p1.r0.x, p1.r0.y, 0.0);
      drawSqaure(0.015);
    glPopMatrix();
  }
}

void drawBoat(float scale)
{
  glPushMatrix();
    //Hull
    //glTranslatef(input.xPos, sinValue(input.xPos), 0.0);
    glRotatef(bAngle, 0.0, 0.0, 1.0);
    glScalef(scale, scale, scale);
    glRotatef(pCoord.angle, 0.0, 0.0, 1.0);
    drawCannon();
    //glRotatef(findAngle1(input.xPos), 0.0, 0.0, 1.0);
    //glTranslatef(0.25, 0.0, 0.0);
    drawHull();
    //Bridge
    glTranslatef(0.0, 0.5, 0.0);
    drawBridge();
    //CannonSocket
    glTranslatef(0.5, -0.25, 0.0);
    //glRotatef(pCoord.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.25, 0.0, 0.0);
    drawCannon();
    //CannonBallLocation
    glTranslatef(0.25, 0.0, 0.0);
    //drawCB();
  glPopMatrix();
}

/*
* OPENGL UTILITIES
*/

void update() 
{
  float t, dt;
  static float tLast = 0.0;
  //tLast = 0.0;
  timeUtil.t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
  

  if (!input.fire) 
  {
    updateProjectileStateInitial();
    glutPostRedisplay();
    return;
  } 

  t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0 - timeUtil.startTime;

  dt = t - tLast;
  updateProjectileStateNumerical(dt);

  if (p1.r.y < sinValue(p1.r.x))
  {
    input.fire = false;
  } 

  tLast = t;

  glutPostRedisplay();
}

void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  //drawSin(35);
  drawCB();
  //drawBoat(boatScale);

  glPushMatrix();
    glScalef(boatScale, boatScale, boatScale);
    glRotatef(bAngle, 0.0, 0.0, 1.0);
    drawAxes(1.0);
    glTranslatef(0.5, 0.25, 0.0);
    drawSqaure(0.10);
    glRotatef(pCoord.angle, 0.0, 0.0, 1.0);
    glTranslatef(0.25, 0.0, 0.0);
    drawCannon();
  glPopMatrix();
  
  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key)
  {
    case 's':
      pCoord.angle -= 0.75;
      glutPostRedisplay();
      break;
    case 'w':
      pCoord.angle += 0.75;
      glutPostRedisplay();
      break;
    case 'q':
      bAngle += 0.75;
      glutPostRedisplay();
      break;
    case 'a':
      bAngle -= 0.75;
      glutPostRedisplay();
      break;
    case 'f':
      if (!input.fire) 
      {
        timeUtil.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState();
        input.fire = true;
      }
      else 
      {
        input.fire = false;
      }
    default:
      break;
  }
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutCreateWindow("Assignment");
  
  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}
