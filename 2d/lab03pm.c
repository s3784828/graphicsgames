/*
 * projectile-motion.c
 *
 * This program shows how to compute projectile motion using both
 * analytical and numerical integration of the equations of motion.
 *
 * $Id: projectile-motion.c,v 1.9 2019/03/22 02:53:51 gl Exp gl $
 *
 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

float toRad(float degree) {
  //printf("radians = %f\n", degree * (M_PI/180));
  return degree * (M_PI/180);
}

typedef struct { float x, y; } vec2f;
typedef struct {float mag, angle;} vec2fPolar;

vec2fPolar polar = {0.5, 60.0};
float v0x = polar.mag * cosf(toRad(polar.angle));
float v0y = polar.mag * sinf(toRad(polar.angle));

const float RADIUS = 0.05;
const float TIMESTEP = 0.01;

typedef struct { vec2f r0, v0, r, v; float radius; } state;
state projectile = {
  { 0.0, 0.0 },
  { v0x, v0y },
  { 0.0, 0.0 },
  { 0.0, 0.0 },
  RADIUS
};

typedef struct {float t, lastT, dt; } Global;


/* Use a low gravity value, rather than true Earth gravity of 9.8,
 * to slow down projectile acceleration, otherwise too fast for
 * nice projectile motion aesthetics.
 */
const float g = -0.25;
const float posg = 0.25;
const int milli = 1000;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef enum { analytical, numerical } integrationMode;

typedef struct {
  bool debug;
  bool go;
  bool reached;
  float startTime;
  integrationMode integrateMode;
  bool OSD;
  int frames;
  float frameRate;
  float frameRateInterval;
  float lastFrameRateT;
} global_t;

global_t global = { true, false, false, 0.0, numerical, true, 0, 0.0, 0.2, 0.0 };
float globalt = 0.0;
float globaldt = 0.0;

void drawQaudratic(float t)
{
  float v0Mag = sqrt(projectile.v0.x * projectile.v0.x + projectile.v0.y * projectile.v0.y);
  float segments = 35;
  //-12 * g * t * t + projectile.v0.y * sinf(polar.angle) * t;
  float left, right;
  left = 0;
  right = 2 * v0Mag * sinf(toRad(polar.angle)) / posg; //= 3.46
  //right /= 0.25;
  //(v0Mag * v0Mag * sinf(toRad(2 * polar.angle))) / posg;
  //((2 * v0Mag) * sinf(toRad(polar.angle))) / (posg);
  float maxHeight = (v0Mag * v0Mag * sinf(toRad(polar.angle)) * sinf(toRad(polar.angle))) / (2 * posg);
  float range = right - left;
  float stepSize = range / segments;
  float x, y;

  //glPointSize(10.0);
  glBegin(GL_LINE_STRIP);
  glColor3f(1, 1, 1);
  float time;
  for (float i = 0; i <= segments; i++)
  {
    time = i * stepSize;
    x = (v0Mag * cosf(toRad(polar.angle))) * time;
    y = -0.5 * posg * time * time + v0Mag * sinf(toRad(polar.angle)) * time + projectile.r0.y;
    glVertex3f(x, y, 0);
    //printf("y = %f, x = %f, t = %f, i = %f\n", y, x, time, i);
    // if (projectile.r.y >= 0 && global.go) {
    //   printf("y = %f, x = %f, t = %f\n", y, x, t);
    // }
  }
  glEnd();
  glutPostRedisplay();
}

void updateProjectileStateAnalytical(float t)
{
  vec2f r0, v0;

  r0 = projectile.r0;
  v0 = projectile.v0;

  projectile.r.x = v0.x * t + r0.x;
  projectile.r.y = 1.0 / 2.0 * g * t * t + v0.y * t + r0.y;

  //drawQaudratic(t);

  if (projectile.r.y < 0) {
    printf("projectile position = %f\n", projectile.r.x);
  }

  /*
   * The above statement can alternatively be written as below, saving
   * a few arithmetic operations, which may or may not be worthwhile
   * and which can obscure code and introduce hard to find bugs:
   *
   * 1) Precalculate 1.0 / 2.0 as 0.5 to save a division
   * projectileState.r.y = 0.5 * g * t * t + v0.y * t + r0.y;
   *
   * 2) Factorise to save a multiplication
   * projectileState.r.y = t * (0.5 * g * t + v0.y) + r0.y;
  */
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

void drawLine(int len)
{
  glPointSize(1);
  glBegin(GL_LINES);
  glColor3f(1, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(polar.mag * cosf(toRad(polar.angle)), polar.mag * sinf(toRad(polar.angle)), 0);
  //printf("x: %f, y: %f\n", polar.mag * cosf(polar.angle), polar.mag * sinf(polar.angle));
  glEnd();
}





void displayProjectile(void)
{
  glPointSize(5.0);
  glBegin(GL_POINTS);
  glVertex2f(projectile.r.x, projectile.r.y);
  glEnd();
}

typedef struct
{
  float amplitude;
  float k;
  float w;
} SinGraphUtility;

SinGraphUtility sinUtil = {0.15, 2 * M_PI / 1 , M_PI / 4};

float sinValue(float x)
{
  return sinUtil.amplitude * sinf(sinUtil.k * x + sinUtil.w * globalt);
}

float sinValue1(float x)
{
  return sinUtil.amplitude * sinf(sinUtil.k * x + sinUtil.w * globalt) + 1;
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

float getDistance() 
{
  return sqrt((projectile.r0.y - sinValue(projectile.r0.x)) * (projectile.r0.y - sinValue(projectile.r0.x)) + (projectile.r.y - sinValue(projectile.r.x)) * (projectile.r.y - sinValue(projectile.r.x)));
  //return sqrt((sinValue(projectile.r0.x) - projectile.r0.y) * (sinValue(projectile.r0.x) - projectile.r0.y) + (sinValue(projectile.r.x) - projectile.r.y) * (sinValue(projectile.r.x)) - projectile.r.y);
}

float getDistance(float x, float y)
{
  float y0 = y - TIMESTEP;
  float x0 = x - TIMESTEP;
  float sin0 = sinValue(x0);
  float sin = sinValue(x);
  // if (sin0 < 0) 
  // {
  //   sin0 = -sinValue(x0);
  // }
  // if (sin < 0) {
  //   sin = -sinValue(x);
  // }
  

  return sqrt((y0 - sin0) * (y0 - sin0) + (y - sin) * (y - sin));
}

void checkCollision() 
{
  float sum = sinValue(projectile.r.x);
  static bool collided = false;
  if (getDistance() <= sum || collided) 
  {
    //printf("Collided\n");

    if (!collided)
      collided = true;
  }
  else if (!collided)
  {
    //printf("distance %f, sum %f\n", getDistance(), sum);
  }
}

bool collided(float x, float y)
{
  float sinvalue = sinValue(x);
  if (sinvalue < 0) 
  {
    sinvalue = -sinValue(x);
  }

  float sum = sinvalue;
  bool collided = false;
  if (getDistance(x, y) <= sum && global.go) 
  {
    printf("Collided\n");

    if (!collided)
      collided = true;
      return collided;
  }
  else if (!collided)
  {
    printf("distance %f, sum %f\n", getDistance(), sum);
  }
  return collided;
}

void updateProjectileStateNumerical(float dt)
{
  // Euler integration
  projectile.r0 = projectile.r;
  // Position
  projectile.r.x += projectile.v.x * dt;
  projectile.r.y += projectile.v.y * dt;

  // Velocity
  projectile.v.y += g * dt;
}

void drawPath(float dt)
{
  float dtt = dt + TIMESTEP;
  float x = projectile.r.x;
  float y = projectile.r.y;
  float vy = projectile.v.y;
  float vx = projectile.v.x;
  glBegin(GL_LINES);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(x, y, 0.0);
  x += vx * dtt;
  y += vy * dtt;
  vy += g * dtt;
  
  while(!collided(x, y) && global.go) {
    
    dtt += TIMESTEP;
    x += vx * dtt;
    y += vy * dtt;
    vy += g * dtt;
    glVertex3f(x, y, 0);
  }
  glEnd();
}

void updateProjectileState(float t, float dt)
{
  if (global.debug)
    //printf("global.integrateMode: %d\n", global.integrateMode);
  if (global.integrateMode == analytical)
    updateProjectileStateAnalytical(t);
  else
    updateProjectileStateNumerical(dt);
    globaldt = dt;
    //drawPath(dt);
}

// Idle callback for animation
void update(void)
{
  static float lastT = -1.0;
  float t, dt;

  if (!global.go)
  {
    globalt = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
  }
  else 
  {
  globalt = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
  t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;
  
  if (lastT < 0.0) {
    lastT = t;
    return;
  }
  checkCollision();
  dt = t - lastT;
  if (global.debug)
    //printf("%f %f\n", t, dt);
  

  //printf("distance %f\n", getDistance());
  
  updateProjectileState(t, dt);
  lastT = t;

  if (collided(projectile.r.x, projectile.r.y))
    global.go = false;
  }
  // t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;
  
  // if (lastT < 0.0) {
  //   lastT = t;
  //   return;
  // }

  // dt = t - lastT;
  // if (global.debug)
  //   //printf("%f %f\n", t, dt);
  

  // printf("distance %f\n", getDistance());
  // updateProjectileState(t, dt);
  


  // lastT = t;

  /* Frame rate */
  dt = t - global.lastFrameRateT;
  if (dt > global.frameRateInterval) {
    global.frameRate = global.frames / dt;
    global.lastFrameRateT = t;
    global.frames = 0;
  }

  if (collided(projectile.r.x, projectile.r.y)) {
    global.go = false;
    projectile.r0 = {0, 0};
  }

  glutPostRedisplay();
}



void displayOSD()
{
  char buffer[30];
  char *bufp;
  int w, h;

  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  /* Set up orthographic coordinate system to match the
     window, i.e. (0,0)-(w,h) */
  w = glutGet(GLUT_WINDOW_WIDTH);
  h = glutGet(GLUT_WINDOW_HEIGHT);
  glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  /* Frame rate */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 60);
  snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", global.frameRate);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Time per frame */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 40);
  snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / global.frameRate * 1000.0);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Pop modelview */
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);

  /* Pop projection */
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  /* Pop attributes */
  glPopAttrib();
}

void display(void)
{
  GLenum err;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glColor3f (0.8, 0.8, 0.8);

  drawAxes(1);
  drawLine(0.5);

  drawSin(30);
  
  glPushMatrix();
    glTranslatef(projectile.r.x, projectile.r.y, 0);
    glColor3f(1, 1, 1);
    glutSolidSphere(projectile.radius,20,20);
  glPopMatrix();

  
  drawPath(globaldt);
  
  //drawPath(globaldt);

  //drawQaudratic(0);


  // Display OSD
  if (global.OSD)
    displayOSD();

  glPopMatrix();

  glutSwapBuffers();

  global.frames++;

  // Check for errors
  while ((err = glGetError()) != GL_NO_ERROR)
    printf("%s\n",gluErrorString(err));
}

void myinit (void)
{
}

void keyboardCB(unsigned char key, int x, int y)
{
  switch (key) {
  case 27:
  case 'q':
    exit(EXIT_SUCCESS);
    break;
  case 'm':
    global.debug = !global.debug;
    break;
  case 'i':
    if (global.integrateMode == analytical)
    {
      global.integrateMode = numerical;
      printf("numerical\n");
    }
    else
    {
      global.integrateMode = analytical;
      printf("analytical\n");
    }
    break;
  case 'o':
    global.OSD = !global.OSD;
    break;
  case 'a':
    polar.angle += 0.25;
    projectile.v0.y = polar.mag * sinf(toRad(polar.angle));
    projectile.v0.x = polar.mag * cosf(toRad(polar.angle));
    //printf("polar angle = %f polar magnitude = %f, initialVelocity.x = %f, initialVelocity.y = %f \n", polar.angle, polar.mag, projectile.v0.x, projectile.v0.y);
    //printf("angle: %f\n", polar.angle);
    glutPostRedisplay();
    break;
  case 'd':
    polar.angle -= 0.25;
    projectile.v0.y = polar.mag * sinf(toRad(polar.angle));
    projectile.v0.x = polar.mag * cosf(toRad(polar.angle));
    //printf("polar angle = %f polar magnitude = %f, initialVelocity.x = %f, initialVelocity.y = %f \n", polar.angle, polar.mag, projectile.v0.x, projectile.v0.y);
    //printf("angle: %f\n", polar.angle);
    glutPostRedisplay();
    break;
  case 'w':
    //projectile.r0.y += 0.10;
    polar.mag *= 1.1;
    //polar.mag += 0.10;
    //printf("polar angle = %f polar magnitude = %f, initialVelocity.x = %f, initialVelocity.y = %f \n", polar.angle, polar.mag, projectile.v0.x, projectile.v0.y);
    projectile.v0.y = polar.mag * sinf(toRad(polar.angle));
    projectile.v0.x = polar.mag * cosf(toRad(polar.angle));

    //projectile.v0.y += 0.10;
    glutPostRedisplay();
    break;
  case 's':
    polar.mag *= 0.9;
    //polar.mag -= 0.10;
    //printf("polar angle = %f polar magnitude = %f, initialVelocity.x = %f, initialVelocity.y = %f \n", polar.angle, polar.mag, projectile.v0.x, projectile.v0.y);
    projectile.v0.y = polar.mag * sinf(toRad(polar.angle));
    projectile.v0.x = polar.mag * cosf(toRad(polar.angle));
    glutPostRedisplay();
    break;
  case 32:
    if (!global.go) {
      global.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
      global.go = true;
      projectile.v.x = projectile.v0.x;
      projectile.v.y = projectile.v0.y;
      projectile.r.y = projectile.r0.y;
      projectile.r.x = projectile.r0.x;
    }
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void myReshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(500, 500);
  glutCreateWindow("Projectile Motion");
  glutKeyboardFunc(keyboardCB);
  glutReshapeFunc(myReshape);
  glutDisplayFunc(display);
  glutIdleFunc(update);

  myinit();

  glutMainLoop();
}
