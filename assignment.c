/* Interactive 3D graphics and Animation, Assignment 01 - Tristan */

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

const float g = -0.25;

//Helper functions
float toDeg(float angle)
{
  return angle * (180 / PI);
}

float toRad(float angle)
{
  return angle * (PI / 180);
}

//Projectile
typedef struct {float x, y; } Vector2f;
typedef struct {float magnitude, angle; } PolarVector2f;
typedef struct {Vector2f r0, v0, r, v; } Projectile;

PolarVector2f pVec2f = {0.5, 60.0};
PolarVector2f pVec2fb1 = {0.5, 60.0};
Vector2f v0start = {pVec2f.magnitude * cosf(toRad(pVec2f.angle)), pVec2f.magnitude * sinf(toRad(pVec2f.angle))};
// float _v0x = pVec2f.magnitude * cosf(toRad(pVec2f.angle));
// float _v0y = pVec2f.magnitude * sinf(toRad(pVec2f.angle));

Vector2f calculateV0(PolarVector2f pVector2f) 
{
  return {pVector2f.magnitude * cosf(toRad(pVector2f.angle)), pVector2f.magnitude * sinf(toRad(pVector2f.angle))};
}

Vector2f calculateR0(float x, float y)
{
  return {x, y};
}

Projectile projectile = {{0, 0}, calculateV0(pVec2f), {0.0, 0.0}, {0.0, 0.0}};
Projectile projectileb1 = {{0, 0}, calculateV0(pVec2fb1), {0.0, 0.0}, {0.0, 0.0}};

//Other
typedef struct {float t, tLast, startTime, startTimeb1; } Global;
typedef struct {float segments; float cannonRotation; float xPosition; bool fire; bool fireb1; } AdjustableValues;
typedef struct {float amplitude, k, w; } SinGraph;

Global global = {0.0, 0.0, 0.0, 0.0};
SinGraph sinG = { 0.25, 2 * PI / 1 , PI / 4};
AdjustableValues aValues = { 20, pVec2f.angle, -0.5, false };

float sinValue(float x)
{
  return sinG.amplitude * sinf(sinG.k * x + sinG.w * global.t);
}

float derivativeSin(float x)
{
  return sinG.amplitude * sinG.k * cosf(sinG.k * x + sinG.w * global.t);
}

float findAngle(float x)
{
  float y, xn, yn, len;
  y = sinValue(x);
  xn -= derivativeSin(x);
  yn = 1;
  len = sqrt(xn * xn + yn * yn);
  xn /= len;
  yn /= len;

  float x1, y1;
  x1 = x + xn;
  y1 = y + yn;

  float angle = toDeg(atan2((y1 - y),(x1 - x)));
  float debugAngle = angle;

  if (debugAngle <= 0) {
    debugAngle = 180 + debugAngle;
  }

  printf("angle = %f\n", angle);



  // if (angle >= 0) {
  //   angle -= 90;
  // }
  // else {
  //   angle += 90;
  // }

  return angle - 90;
}

float findAngle1(float x)
{
  float y, xn, yn, len;
  y = sinValue(x);
  xn -= derivativeSin(x);
  yn = 1;
  len = sqrt(xn * xn + yn * yn);
  xn /= len;
  yn /= len;

  float x1, y1;
  x1 = x + xn;
  y1 = y + yn;

  float angle = toDeg(atanf((y1 - y) / (x1 - x)));
  float debugAngle = angle;

  if (debugAngle <= 0) {
    debugAngle = 180 + debugAngle;
  }

  return angle;
}

void updateProjectileStateAnalytical(float t, Projectile projectile)
{
  Vector2f r0, v0;

  r0 = projectile.r0;
  v0 = projectile.v0;

  projectile.r.x = v0.x * t + r0.x;
  projectile.r.y = 1.0 / 2.0 * g * t * t + v0.y * t + r0.y;
  //printf("x: %f, y: %f, t: %f\n", projectile.r.x, projectile.r.y, t);
  glutPostRedisplay();
}

void updateProjTower(float t)
{
  Vector2f r0, v0;

  r0 = projectile.r0;
  v0 = projectile.v0;

  projectile.r.x = v0.x * t + r0.x;
  projectile.r.y = 1.0 / 2.0 * g * t * t + v0.y * t + r0.y;
  //printf("x: %f, y: %f, t: %f\n", projectile.r.x, projectile.r.y, t);
  glutPostRedisplay();
}

void updateProjBoat(float t) 
{
  Vector2f r0, v0;

  r0 = projectileb1.r0;
  v0 = projectileb1.v0;

  projectileb1.r.x = v0.x * t + r0.x;
  projectileb1.r.y = 1.0 / 2.0 * g * t * t + v0.y * t + r0.y;
  //printf("x: %f, y: %f, t: %f\n", projectile.r.x, projectile.r.y, t);
  glutPostRedisplay();
}

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

void drawTower(float width, float height) 
{
  glBegin(GL_QUADS);
  glColor3f(1, 0, 0);
  glVertex3f(width, -1.0, 0.0);
  glVertex3f(-width, -1.0, 0.0);
  glVertex3f(-width, height, 0.0);
  glVertex3f(width, height, 0.0);
  glEnd();
}

void drawCannon(float width, float height)
{
  glBegin(GL_QUADS);
  glColor3f(1, 0, 0);
  glVertex3f(width, -height, 0.0);
  glVertex3f(-width, -height, 0.0);
  glVertex3f(-width, height, 0.0);
  glVertex3f(width, height, 0.0);
  glEnd();
}

void drawTowerFull() 
{
  glPushMatrix();
    //Tower
    drawTower(0.25, 0.20);
    //CannonSocket
    glTranslatef(0.0, 0.25, 0.0);
    glRotatef(270 + pVec2f.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.0, 0.125, 0.0);
    drawCannon(0.05, 0.125);
  glPopMatrix();
}

void drawTangentSin(float x, float y, float s) {
  float xt, yt, len;
  xt = 1;
  yt = derivativeSin(x);
  len = sqrt(xt * xt + yt * yt);
  xt /= len;
  yt /= len;
  xt *= s;
  yt *= s;

  float x1, y1;
  x1 = x + xt;
  y1 = y + yt;

  glBegin(GL_LINES);
  glColor3f(0, 1, 0);
  glVertex2f(x, y);
  glVertex2f(x1, y1);
  glEnd();
}

void drawNormalSin(float x, float y, float s)
{
  float xn, yn, len;
  xn -= derivativeSin(x);
  yn = 1;
  len = sqrt(xn * xn + yn * yn);
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

  for (int i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = sinValue(x);
    drawTangentSin(x, y, 0.05);
  }

  for (int i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = sinValue(x);
    drawNormalSin(x, y, 0.05);
  }
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

void drawBoat()
{
  glPushMatrix();
    //Hull
    glTranslatef(0.0, 0.25, 0.0);
    drawHull();
    //Bridge
    glTranslatef(0.0, 0.5, 0.0);
    drawBridge();
    //CannonSocket
    glTranslatef(0.5, -0.25, 0.0);
    glRotatef(pVec2fb1.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.25, 0.0, 0.0);
    drawCannon();
    //CannonBallLocation
    glTranslatef(0.25, 0.0, 0.0);
    //drawQaudBoat();
    
  glPopMatrix();
}

void drawBoatFull()
{
    glPushMatrix();
      glTranslatef(aValues.xPosition, sinValue(aValues.xPosition), 0.0);
      //glTranslatef(aValues.xPosition, 0.0, 0.0);
      glRotatef(findAngle(aValues.xPosition), 0.0, 0.0, 1.0);
      glScalef(0.10, 0.10, 0.10);
      drawBoat();
    glPopMatrix();
}

void drawSqaure(float len)
{
  glBegin(GL_LINE_LOOP);
  glColor3f(1, 1, 1);
  glVertex3f(len, len, 0.0);
  glVertex3f(len, -len, 0.0);
  glVertex3f(-len, -len, 0.0);
  glVertex3f(-len, len, 0.0);
  glEnd();
}

void drawCannonBall(Projectile projectile) 
{
  glPushMatrix();
    glTranslatef(projectile.r.x, projectile.r.y, 0.0);
    drawSqaure(0.015);
  glPopMatrix();
}

void drawcb() 
{
  if (aValues.fireb1) {
    glPushMatrix();
      glTranslatef(projectileb1.r.x, projectileb1.r.y, 0.0);
      drawSqaure(0.015);
    glPopMatrix();
  }
}

void drawct() 
{
  if (aValues.fire) {
    glPushMatrix();
      glTranslatef(projectile.r.x, projectile.r.y, 0.0);
      drawSqaure(0.015);
    glPopMatrix();
  }
}

void drawQaudratic(Projectile projectile, PolarVector2f pVec2f)
{
  float v0Mag = sqrt(projectile.v0.x * projectile.v0.x + projectile.v0.y * projectile.v0.y);
  float segments = 35;
  float start, flightTime;
  start = projectile.r0.x;
  flightTime = 2 * v0Mag * sinf(toRad(pVec2f.angle)) / -g;
  float maxHeight = (v0Mag * v0Mag * sinf(toRad(pVec2f.angle)) * sinf(toRad(pVec2f.angle))) / (2 * -g);
  float range = flightTime - start;
  float stepSize = range / segments;
  float x, y;

  glBegin(GL_LINE_STRIP);
  glColor3f(0, 1, 0);
  float time;
  for (float i = 0; i <= segments; i++)
  {
    time = i * stepSize;
    x = (v0Mag * cosf(toRad(pVec2f.angle))) * time + start;
    y = -0.5 * -g * time * time + v0Mag * sinf(toRad(pVec2f.angle)) * time + projectile.r0.y;
    if (x > projectile.r.x && aValues.fire) {
      glVertex3f(x, y, 0.0);
    }
  }
  glEnd();
}

void drawQaudBoat() {
  float v0Mag = sqrt(projectileb1.v0.x * projectileb1.v0.x + projectileb1.v0.y * projectileb1.v0.y);
  float segments = 35;
  float start, flightTime;
  //start = projectileb1.r0.x + (0.5 + cosf(toRad(pVec2f.angle)) * 0.25) * 0.10;
  start = projectileb1.r0.x;
  flightTime = 2 * v0Mag * sinf(toRad(pVec2fb1.angle)) / -g;
  float maxHeight = (v0Mag * v0Mag * sinf(toRad(pVec2fb1.angle)) * sinf(toRad(pVec2fb1.angle))) / (2 * -g);
  float range = flightTime - start;
  float stepSize = range / segments;
  float x, y;

  glBegin(GL_LINE_STRIP);
  glColor3f(0, 1, 0);
  float time;
  for (float i = 0; i <= segments; i++)
  {
    time = i * stepSize;
    x = (v0Mag * cosf(toRad(pVec2fb1.angle))) * time + start;
    y = -0.5 * -g * time * time + v0Mag * sinf(toRad(pVec2fb1.angle)) * time + projectileb1.r0.y;
    if (x > projectileb1.r.x && aValues.fireb1) {
      glVertex3f(x, y, 0.0);
    }
  }
  glEnd();
}

void drawBoatQaudratic() 
{
  glPushMatrix();
    //glScalef(0.10, 0.10, 0.0);
    //glTranslatef((0.5 + cosf(toRad(pVec2f.angle)) * 0.25) * 0.10, (0.5 + sinf(toRad(pVec2f.angle)) * 0.25) * 0.10, 0.0);
    //glTranslatef((0.5 + cosf(toRad(pVec2f.angle)) * 0.25) * 0.10, 0.0, 0.0);
    glRotatef(findAngle(aValues.xPosition), 0.0, 0.0, 1.0);
    //drawQaudratic(projectile, pVec2f);
    drawQaudBoat();
  glPopMatrix();  
}

void update()
{
  float t, dt;
  static float tLast = -1.0;
  //float tLast = -1.0;
  
  t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0 - global.startTime;
  global.t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
  
  

  if (tLast < 0.0) {
    tLast = t;
  }
  
  // if (aValues.fire) {
  //   printf("Fire = true\n");
  // } 
  // else {
  //   printf("Fire = false\n");
  // }
  
  //printf("island = %f, Boat1 = %f\n", global.t - global.startTime, global.t - global.startTimeb1);
  dt = t - tLast;
  //printf("System time: %f, dt: %f\n", t, dt);
  if (aValues.fire) 
  {
    //updateProjectileStateAnalytical(global.t - global.startTime, projectile);
    updateProjTower(global.t - global.startTime);
  }
  if (aValues.fireb1) 
  {
    //updateProjectileStateAnalytical(global.t - global.startTimeb1, projectileb1);
    updateProjBoat(global.t - global.startTimeb1);
  }
  tLast = t;
  global.tLast = t;
  glutPostRedisplay();
}

// Display callback
void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  drawAxes(1.0);
  drawTowerFull();
  drawSin(aValues.segments);
  
  drawBoatFull();
  //drawQaudratic(global.t);
  if (!aValues.fire) 
  {
    projectile.r0 = {cosf(toRad(pVec2f.angle)) * 0.25f, sinf(toRad(pVec2f.angle)) * 0.25f + 0.25f};
  }
  if (!aValues.fireb1) 
  {
    //projectileb1.r0 = {aValues.xPosition, sinValue(aValues.xPosition)};
    projectileb1.r0 = {aValues.xPosition + (0.5f + cosf(toRad(pVec2f.angle)) * 0.25f) * 0.10f, sinValue(aValues.xPosition) +  (0.5f + sinf(toRad(pVec2f.angle)) * 0.25f) * 0.10f};
  }
  drawQaudratic(projectile, pVec2f);
  //drawBoatQaudratic();
  drawQaudBoat();
  //drawCannonBall(projectile);
  //drawCannonBall(projectileb1);
  drawcb();
  drawct();
  //printf("r.x: %f, r.y: %f \n", projectile.r.y, projectile.r.x);
  if (projectile.r.y <= sinValue(projectile.r.x)) 
  {
    aValues.fire = false;
  }
  if (projectileb1.r.y <= sinValue(projectileb1.r.x)) 
  {
    aValues.fireb1 = false;
  }
  
  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key)
  {
    case '=':
      aValues.segments += 2;
      glutPostRedisplay();
      break;
    case '-':
      aValues.segments -= 2;
      glutPostRedisplay();
      break;
    case 'w':
      if (!aValues.fire)
        pVec2f.angle += 0.75;
      printf("cannon rotation = %f\n", pVec2f.angle);
      projectile.v0 = calculateV0(pVec2f);
      glutPostRedisplay();
      break;
    case 's':
      if (!aValues.fire)
        pVec2f.angle -= 0.75;
      printf("cannon rotation = %f\n", pVec2f.angle);
      projectile.v0 = calculateV0(pVec2f);
      glutPostRedisplay();
      break;
    case '1':
      if (!aValues.fireb1)
        pVec2fb1.angle += 0.75;
      printf("cannon rotation = %f\n", pVec2f.angle);
      projectileb1.v0 = calculateV0(pVec2fb1);
      glutPostRedisplay();
      break;
    case 'q':
      if (!aValues.fireb1)
        pVec2fb1.angle -= 0.75;
      printf("cannon rotation = %f\n", pVec2f.angle);
      projectileb1.v0 = calculateV0(pVec2fb1);
      glutPostRedisplay();
      break;
    case 'd':
      aValues.xPosition += 0.01;
      glutPostRedisplay();
      break;
    case 'a':
      aValues.xPosition -= 0.01;
      glutPostRedisplay();
      break;
    case 'f':
      if (!aValues.fire) 
      {
        printf("fire\n");
        global.startTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        aValues.fire = true;
        projectile.v.x = projectile.v0.x;
        projectile.v.y = projectile.v0.y;
        projectile.r.y = projectile.r0.y;
        projectile.r.x = projectile.r0.x;
      }
      break;
    case 'g':
      if (!aValues.fireb1) 
      {
        printf("fire\n");
        global.startTimeb1 = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        aValues.fireb1 = true;
        projectileb1.v.x = projectileb1.v0.x;
        projectileb1.v.y = projectileb1.v0.y;
        projectileb1.r.y = projectileb1.r0.y;
        projectileb1.r.x = projectileb1.r0.x;
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
