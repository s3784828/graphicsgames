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
const float boatScale = 0.10;
const float pScale = 0.25;
const float timeStep = 0.00001;
const float movementSpeed = 0.005;
float numericalInc = 0.5;
float bAngle = 0.0;
bool dab = false;

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
  float radius;
} Projectile;

typedef struct
{
  float t;
  float startTime;
  float dt;
} TimeUtility;

typedef struct
{
  bool fire;
  float xPos;
} InputUtility;

typedef struct
{
  Vector2 pos;
  float radius;
} Collider;

typedef struct
{
  PolarCoordinate pc;
  Projectile p;
  Collider c;
  InputUtility i;
  float health;
} ControlledObject;

/*
* CLASS OBJECTS
*/

/*
* ENVIROMENTAL OBJECTS
*/

SinGraphUtility sinUtil = {0.25, 2 * PI / 1 , PI / 4};
TimeUtility timeUtil = {0.0, 0.0, 0.0};

/*
* BOAT #1
*/

PolarCoordinate pCoord = {0.35, 0.0};
InputUtility input = {false, -0.5};
Projectile p1 = {{0,0}, {0,0}, {0,0}, {0,0}, pScale / 2};
Collider b1 = {{0.0, 0.0}};
//BoatObject = {p1, b1};

ControlledObject boat1 
{
  {0.35, 0.0},
  {{0,0}, {0,0}, {0,0}, {0,0}, pScale / 2},
  {{0.0, 0.0}},
  {false, -0.5},
  100.0
};

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

  return angle;
}

Vector2 calculateR0Angle(float hyp, float xInc, float yInc, float s)
{
  hyp *= s;
  xInc *= s;
  yInc *= s;
  xInc += input.xPos;
  yInc += sinValue(input.xPos);

  return {hyp * cosf(toRad(findAngle(input.xPos))) + input.xPos, hyp * cosf(toRad(findAngle(input.xPos))) + sinValue(input.xPos)};
}

Vector2 calculateV0Angle() 
{
  return {pCoord.mag * cosf(toRad(findAngle(input.xPos))), pCoord.mag * sinf(toRad(findAngle(input.xPos)))};
}


Vector2 calculateR0(float hyp, float xInc, float yInc, float s)
{
  hyp *= s;
  xInc *= s;
  yInc *= s;
  xInc += input.xPos;
  yInc += sinValue(input.xPos);

  return {hyp * cosf(toRad(pCoord.angle)) + xInc, hyp * sinf(toRad(pCoord.angle)) + yInc};
}

Vector2 calculateV0() 
{
  return {pCoord.mag * cosf(toRad(pCoord.angle)), pCoord.mag * sinf(toRad(pCoord.angle))};
}


void updateProjectileStateNumerical(float dt) 
{
  // Position
  p1.r.x += p1.v.x * dt;
  p1.r.y += p1.v.y * dt;

  // Velocity
  p1.v.y += g * dt;
}

void updateProjectileStateInitial() 
{
  p1.r0 = calculateR0(0.5, 0.5, 0.5, boatScale);
  p1.v0 = calculateV0();
}

void updateProjectileState()
{
  p1.r = p1.r0;
  p1.v = p1.v0;
}

bool collided(Vector2 position)
{
  return position.y <= sinValue(position.x);
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

void drawSquare(float len)
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
      drawSquare(0.015);
    glPopMatrix();
  }
  else 
  {
    glPushMatrix();
      glTranslatef(p1.r0.x, p1.r0.y, 0.0);
      drawSquare(0.015);
    glPopMatrix();
  }
}

void drawBoat(ControlledObject boat, float scale)
{
  glPushMatrix();
    //Hull
    glTranslatef(boat.i.xPos, sinValue(boat.i.xPos), 0.0);
    glScalef(scale, scale, scale);
    //glRotatef(findAngle(input.xPos), 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.25, 0.0);
    drawHull();
    //Bridge
    glTranslatef(0.0, 0.5, 0.0);
    drawBridge();
    //CannonSocket
    glTranslatef(0.5, -0.25, 0.0);
    glRotatef(boat.pc.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.25, 0.0, 0.0);
    drawCannon();
    //CannonBallLocation
    glTranslatef(0.25, 0.0, 0.0);
    //drawCB();
  glPopMatrix();
}

void drawProjectilePath()
{
  if (!input.fire)
  {
    return;
  }

  float dt; 
  Vector2 r, v;
  dt = timeUtil.dt;
  r = p1.r;
  v = p1.v;
  glBegin(GL_LINE_STRIP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(r.x, r.y, 0.0);
  while(!collided(r))
  {
    dt += timeStep;
    r.x += v.x * dt;
    r.y += v.y * dt;
    v.y += g * dt;
    if (r.y >= sinValue(r.x))
    {
      glVertex3f(r.x, r.y, 0.0);
    }
  }
  glEnd();
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
  timeUtil.dt = dt;
  updateProjectileStateNumerical(dt);

  if (collided(p1.r))
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
  drawSin(35);
  drawBoat(boatScale);
  drawCB();
  drawProjectilePath();
  
  
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

    case 'a':
      input.xPos -= movementSpeed;
      glutPostRedisplay();
      break;

    case 'd':
      input.xPos += movementSpeed;
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
  glutCreateWindow("Assignment");
  
  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}
