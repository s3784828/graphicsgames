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

const float G = -0.25;
const float BOAT_SCALE = 0.10;
const float TIME_STEP = 0.0005;
const float BOAT_SPEED = 0.005;
const float CANNON_SPEED = 0.75;
const float DAMAGE_BOAT = 10;
const float DAMAGE_ISLAND = 1;

const float BOAT_RADIUS = 0.5 * BOAT_SCALE;
const float ISLAND_RADIUS = 0.25;
const float PROJ_RADIUS = 0.125;

const float NET_RADIUS = 0.05;
const float NET_RADIUS_GROWTH = 0.0001;
const float NET_FLIGHT_TIME = 2.0;

/*
* TYPEDEF STRUCTS
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
  float r;
  float g;
  float b;
} Color;

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
  float tLast;
} TimeUtility;

typedef struct
{
  bool fire;
  bool fireNet;
  float xPos;
} InputUtility;

typedef struct
{
  bool OSD;
  int frames;
  float frameRate;
  float frameRateInterval;
  float frameLast;
} SystemDisplay;

typedef struct
{
  PolarCoordinate pc;
  Projectile p;
  Projectile net;
  InputUtility i;
  Vector2 trans;
  Vector2 r0;
  Color col;
  TimeUtility tProj;
  TimeUtility tNet;
  float radius;
  float hyp;
  float health;
  bool isIsland;
} Object;

/*
* VARIABLES
*/

float tesselation = 26;
bool allowTangents = false;
bool allowNormals = false;
bool wireFrameMode = false;
bool gameOver = false;
bool allowAnimation = true;
float globalTime = 0.0;

SystemDisplay sd = {false, 0, 0.0, 0.2, 0.0};

/*
* ENVIROMENTAL OBJECTS
*/

SinGraphUtility sinUtil = {0.25, 2 * PI / 1 , PI / 4};
TimeUtility timeUtil = {0.0, 0.0, 0.0, 0.0};

/*
* BOAT #1
*/

Object boat1 
{
  {0.5, 60},
  {{0,0}, {0,0}, {0,0}, {0,0}, PROJ_RADIUS},
  {{0,0}, {0,0}, {0,0}, {0,0}, NET_RADIUS},
  {false, false, -0.5},
  {0.5, 0.5},
  {0.0, 0.0},
  {1.0, 0.75, 0.0},
  {0.0, 0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0, 0.0},
  BOAT_RADIUS,
  0.5,
  100.0,
  false
};

/*
* BOAT #2
*/

Object boat2
{
  {0.5, 120},
  {{0,0}, {0,0}, {0,0}, {0,0}, PROJ_RADIUS},
  {{0,0}, {0,0}, {0,0}, {0,0}, NET_RADIUS},
  {false, false, 0.5},
  {-0.5, 0.5},
  {0.0, 0.0},
  {1.0, 0.0, 0.5},
  {0.0, 0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0, 0.0},
  BOAT_RADIUS,
  0.5,
  100.0,
  false
};

/*
* ISLAND
*/

/*
* Despite an island having different attributes than a boat,
* I thought it would still be more clearer and less computionally
* expensive to have an island same object as boat.
*/

Object island
{
  {0.35, 60.0},
  {{0,0}, {0,0}, {0,0}, {0,0}, PROJ_RADIUS},
  {{0,0}, {0,0}, {0,0}, {0,0}, 0.0},
  {false, false, 0.0},
  {0.0, 0.25},
  {0.0, 0.0},
  {0.0, 1.0, 0.75},
  {0.0, 0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0, 0.0},
  ISLAND_RADIUS,
  0.25,
  100.0,
  true
};

/*
* ARRAY OF OBJECTS
*/

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
  return sinUtil.amplitude * sinf(sinUtil.k * x + sinUtil.w * globalTime);
}

float derivativeSin(float x)
{
  return sinUtil.amplitude * sinUtil.k * cosf(sinUtil.k * x + sinUtil.w * globalTime);
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

void updateProjectileStateNumerical(Object* obj, float dt, bool updateNet) 
{
  if (!updateNet)
  {
    obj->p.r0 = obj->p.r;

    // Position
    obj->p.r.x += obj->p.v.x * dt;
    obj->p.r.y += obj->p.v.y * dt;

    // Velocity
    obj->p.v.y += G * dt;
  }
  else 
  {
    obj->net.r0 = obj->net.r;

    // Position
    obj->net.r.x += obj->net.v.x * dt;
    obj->net.r.y += obj->net.v.y * dt;

    // Velocity
    obj->net.v.y += G * dt;
  }
  
}

Vector2 calculateR0(Object* obj, float hyp, float xInc, float yInc, float s)
{
  hyp *= s;
  xInc *= s;
  yInc *= s;

  float radangle = toRad(findAngle(obj->i.xPos));
  float angle = findAngle(obj->i.xPos);

  float transX, transY, x, y;

  if (!obj->isIsland)
  {
    transX = xInc * cosf(toRad(findAngle(obj->i.xPos))) - yInc * sinf(toRad(findAngle(obj->i.xPos))); 
    transY = xInc * sinf(toRad(findAngle(obj->i.xPos))) + yInc * cosf(toRad(findAngle(obj->i.xPos)));

    x = hyp * cosf(toRad(findAngle(obj->i.xPos) + obj->pc.angle)) + transX + obj->i.xPos;
    y = hyp * sinf(toRad(findAngle(obj->i.xPos) + obj->pc.angle)) + transY + sinValue(obj->i.xPos);
  }
  else 
  {
    x = hyp * cosf(toRad(obj->pc.angle)) + xInc;
    y = hyp * sinf(toRad(obj->pc.angle)) + yInc;
  }

  return {x, y};
}

Vector2 calculateV0(Object* obj) 
{
  float angle = obj->pc.angle;
  if (!obj->isIsland)
  {
   angle += findAngle(obj->i.xPos);
  }
  return {obj->pc.mag * cosf(toRad(angle)), obj->pc.mag * sinf(toRad(angle))};
}

void updateProjectileStateInitial(Object* obj) 
{
  float scale = BOAT_SCALE;
  if (obj->isIsland)
  {
    scale = 1.0;
  }
  obj->p.r0 = calculateR0(obj, obj->hyp, obj->trans.x, obj->trans.y, scale);
  obj->p.v0 = calculateV0(obj);
  obj->net.r0 = obj->p.r0;
  obj->net.v0 = obj->p.v0;
}

void updateProjectileState(Object* obj, bool updateNet)
{
  if (!updateNet)
  {
    obj->p.r = obj->p.r0;
    obj->p.v = obj->p.v0;
  }
  else 
  {
    obj->net.r = obj->net.r0;
    obj->net.v = obj->net.v0;
  }
}

void updateInitialPosition(Object* obj)
{
  obj->r0.x = obj->i.xPos;
  obj->r0.y = sinValue(obj->i.xPos);
}

float getDistanceObject(Object current, Object target)
{
  return sqrt(pow(current.i.xPos - target.i.xPos, 2));
}

Vector2 getDistanceProjectile(Object current, Object target)
{
  float distanceX = sqrt(pow(current.p.r0.x - target.r0.x, 2) + pow(current.p.r.x - target.i.xPos, 2));
  float distanceY = sqrt(pow(current.p.r0.y - target.r0.y, 2) + pow(current.p.r.y - sinValue(target.i.xPos), 2));
  return {distanceX, distanceY};
}

Vector2 getDistanceNet(Object current, Object target)
{
  /* I could not get this to work */

  /*
  * float distanceX = sqrt(pow(current.net.r0.x - target.p.r0.x, 2) + pow(current.net.r.x - target.p.r.x, 2));
  * float distanceY = sqrt(pow(current.net.r0.y - target.p.r0.y, 2) + pow(current.net.r.y - target.p.r.y, 2));
  */

  /* So instead I did this, I know it is not proper implementation ): */

  float distanceX = sqrt(pow(current.net.r.x - target.p.r.x, 2));
  float distanceY = sqrt(pow(current.net.r.y - target.p.r.y, 2));

  return {distanceX, distanceY};
}

Vector2 getDistanceIsland(Object current) 
{
  float distanceX = sqrt(pow(current.p.r0.x, 2) + pow(current.p.r.x, 2));
  float distanceY = sqrt(pow(current.p.r0.y, 2) + pow(current.p.r.y, 2));
  return {distanceX, distanceY};
}

bool collided(Object current, Object* target)
{
  bool collided = false;
  Vector2 distance;

  /*
  * Things get weird if I dont use a seperate method
  * dont know why this is though.
  */
  if (target->isIsland)
  {
    distance = getDistanceIsland(current);
  } 
  else
  {
    distance = getDistanceProjectile(current, *target);
  }

  float sum = current.p.radius + target->radius;
  if (distance.x <= sum && distance.y <= sum)
  {
    collided = true;
    if (target->isIsland)
    {
      target->health -= DAMAGE_ISLAND;
    }
    else
    {
      target->health -= DAMAGE_BOAT;
    }
  }
  return collided;
}

void collidedNet(Object current, Object* target)
{
  bool collided = false;
  float sum = current.net.radius + target->p.radius;

  Vector2 distance = getDistanceNet(current, *target);

  if (distance.x <= sum && distance.y <= sum)
  {
    collided = true;
    target->i.fire = false;
    target->p.r = {0,0};
  }
}

bool collidedWithIsland(Object obj, float inc)
{
  float sum = obj.radius + island.radius;
  bool collidedWithIsland = false;
  obj.i.xPos += inc;

  if (getDistanceObject(obj, island) <= sum) 
  {
    collidedWithIsland = true;
  }
  return collidedWithIsland;
}

bool collidedOcean(Vector2 position)
{
  return position.y <= sinValue(position.x);
}

bool collidedOcean(Object obj)
{
  return obj.p.r.y <= sinValue(obj.p.r.x);
}

/*
* DRAWING METHODS
*/

/*
* ENVIROMENTAL
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
  glColor3f(1, 0, 0);
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
  
  glColor4f(0, 0.25, 1, 0.5);
  float x, y;
  for (int i = 0; i <= segments; i++)
  {
    x = i * stepSize + left;
    y = sinValue(x);
    glVertex3f(x, y, 0.0);
    glVertex3f(x, -1.0, 0.0);
  }
  glEnd();

  if (allowTangents)
  {
    for (int i = 0; i <= segments; i++)
    {
      x = i * stepSize + left;
      y = sinValue(x);
      drawTangentSin(x, y, 0.05);
    }
  }

  if (allowNormals)
  {
    for (int i = 0; i <= segments; i++)
    {
      x = i * stepSize + left;
      y = sinValue(x);
      drawNormalSin(x, y, 0.05);
    }
  }
}

/*
* CONTROLLED
*/

void drawHull(Color col)
{
  glBegin(GL_QUADS);
  glColor3f(col.r, col.g, col.b);
  glVertex3f(-0.5, -0.25, 0.0);
  glVertex3f(0.5, -0.25, 0.0);
  glVertex3f(1.0, 0.25, 0.0);
  glVertex3f(-1.0, 0.25, 0.0);
  glEnd();
}

void drawBridge(Color col)
{
  glBegin(GL_QUADS);
  glColor3f(col.r, col.g, col.b);
  glVertex3f(-0.25, -0.25, 0.0);
  glVertex3f(0.25, -0.25, 0.0);
  glVertex3f(0.25, 0.25, 0.0);
  glVertex3f(-0.25, 0.25, 0.0);
  glEnd();
}

void drawCannon(Color col)
{
  glBegin(GL_QUADS);
  glColor3f(col.r, col.g, col.b);
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

void drawCB(Object obj)
{
  if (obj.i.fire) 
  {
    glPushMatrix();
      glTranslatef(obj.p.r.x, obj.p.r.y, 0.0);
      glColor3f(obj.col.r, obj.col.g, obj.col.b);
      glutSolidSphere(PROJ_RADIUS * 0.10, 20, 20);
    glPopMatrix();
  }
}

void drawBoat(Object boat, bool facingRight)
{
  float cannonPosition = -0.5;
  if (facingRight) 
  {
    cannonPosition = 0.5;
  }
  glPushMatrix();
    //Hull
    glTranslatef(boat.i.xPos, sinValue(boat.i.xPos), 0.0);
    glScalef(BOAT_SCALE, BOAT_SCALE, BOAT_SCALE);
    glRotatef(findAngle(boat.i.xPos), 0.0, 0.0, 1.0);
    drawAxes(1.0);
    glTranslatef(0.0, 0.25, 0.0);
    drawHull(boat.col);
    //Bridge
    glTranslatef(0.0, 0.5, 0.0);
    drawBridge(boat.col);
    //CannonSocket
    glTranslatef(cannonPosition, -0.25, 0.0);
    glRotatef(boat.pc.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.25, 0.0, 0.0);
    drawCannon(boat.col);
  glPopMatrix();
}

void drawIslandBase(float width, float height, Color col) 
{
  glBegin(GL_QUADS);
  glColor3f(col.r, col.g, col.b);
  glVertex3f(width, -1.0, 0.0);
  glVertex3f(-width, -1.0, 0.0);
  glVertex3f(-width, height, 0.0);
  glVertex3f(width, height, 0.0);
  glEnd();
}

void drawIslandCannon(float width, float height, Color col)
{
  glBegin(GL_QUADS);
  glColor3f(col.r, col.g, col.b);
  glVertex3f(-width, -height, 0);
  glVertex3f(width, -height, 0.0);
  glVertex3f(width, height, 0.0);
  glVertex3f(-width, height, 0.0);
  glEnd();
}

void drawIsland(Object island) 
{
  glPushMatrix();
    //Tower
    drawIslandBase(0.25, 0.25, island.col);
    //CannonSocket
    glTranslatef(0.0, 0.25, 0.0);
    glRotatef(island.pc.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.125, 0.0, 0.0);
    drawIslandCannon(0.125, 0.025, island.col);
  glPopMatrix();
}

void drawHealthBar(Object obj, float yTrans) 
{
  float x = obj.health * 0.005;
  float y = 0.05;
  glPushMatrix();
    glTranslatef(-1.0, yTrans, 0.0);
    glBegin(GL_QUADS); 
    glColor3f(obj.col.r, obj.col.g, obj.col.b);
    glVertex3f(0.0, y, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(x, 0.0, 0.0);
    glVertex3f(x, y, 0.0);
    glEnd();
  glPopMatrix();
}

void drawProjectilePath(Object obj)
{
  if (obj.i.fire)
  {
    float dt; 
    Vector2 r, v;
    dt = obj.tProj.dt;
    r = obj.p.r;
    v = obj.p.v;
    glBegin(GL_LINE_STRIP);
    glColor4f(obj.col.r, obj.col.g, obj.col.b, 0.25);
    glVertex3f(r.x, r.y, 0.0);
    while(!collidedOcean(r))
    {
      dt += TIME_STEP;
      r.x += v.x * dt;
      r.y += v.y * dt;
      v.y += G * dt;

      if (r.y >= sinValue(r.x))
      {
        glVertex3f(r.x, r.y, 0.0);
      }
    }
    glEnd();
  }
}

void drawNet(Object obj)
{
  if (obj.i.fireNet)
  {
    glPushMatrix();
      glTranslatef(obj.net.r.x, obj.net.r.y, 0.0);
      glColor3f(obj.col.r, obj.col.g, obj.col.b);
      glutWireSphere(obj.net.radius, 5, 5);
    glPopMatrix();
  }
}

/*
* OPENGL UTILITIES
*/

void updateObject(Object* obj, Object* target, Object* target1)
{
  if (allowAnimation)
  {
    obj->tProj.t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0 - obj->tProj.startTime;
  }
  
  obj->tProj.dt = obj->tProj.t - obj->tProj.tLast;
  updateProjectileStateNumerical(obj, obj->tProj.dt, false);

  obj->tProj.tLast = obj->tProj.t;

  if (collided(*obj, target) || collided(*obj, target1) || collidedOcean(*obj))
  {
    obj->p.r = {0, 0};
    obj->i.fire = false;
    obj->tProj = {0.0, 0.0, 0.0, 0.0};
  }

  glutPostRedisplay();
}

void updateObjectNet(Object* obj, Object* target, Object* target1)
{
  if (allowAnimation)
  {
    obj->tNet.t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0 - obj->tNet.startTime;
  }
  
  
  obj->tNet.dt = obj->tNet.t - obj->tNet.tLast;
  if (obj->tNet.t <= NET_FLIGHT_TIME)
  {
    if (target->i.fire)
    {
      collidedNet(*obj, target);
    }
    if (target1->i.fire)
    {
      collidedNet(*obj, target1);
    }
    updateProjectileStateNumerical(obj, obj->tNet.dt, true);

    if (allowAnimation)
    {
      obj->net.radius += NET_RADIUS_GROWTH;
    }
    obj->tNet.tLast = obj->tNet.t; 
  }
  else 
  {
    obj->i.fireNet = false;
    obj->net.radius = NET_RADIUS;
    obj->tNet = {0.0, 0.0, 0.0, 0.0};
  }

  glutPostRedisplay();
}

void update()
{
  float t, dt;
  static float tLast = 0.0;

  /* used for animations */
  if (allowAnimation)
  {
    globalTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
  }

  /* used for framerate */
  t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;

  dt = t - tLast;

  tLast = t;
 
  updateProjectileStateInitial(&boat1);
  updateProjectileStateInitial(&boat2);
  updateProjectileStateInitial(&island);

  updateInitialPosition(&boat1);
  updateInitialPosition(&boat2);

  if (boat1.i.fire) 
  {
    updateObject(&boat1, &island, &boat2);
  }

  if (boat1.i.fireNet)
  {
    updateObjectNet(&boat1, &island, &boat2);
  }

  if (boat2.i.fire)
  {
    updateObject(&boat2, &island, &boat1);
  }

  if (boat2.i.fireNet)
  {
    updateObjectNet(&boat2, &island, &boat1);
  }

  if (island.i.fire)
  {
    updateObject(&island, &boat1, &boat2);
  }

  if (boat1.health == 0 || boat2.health == 0 || island.health == 0)
  {
    gameOver = true;
  }

  dt = t - sd.frameLast;
  if (dt > sd.frameRateInterval) {
    sd.frameRate = sd.frames / dt;
    sd.frameLast = t;
    sd.frames = 0;
  }

  glutPostRedisplay();
}

void displayTechnicalStuff()
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
  snprintf(buffer, sizeof buffer, "fr (f/s): %6.0f", sd.frameRate);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Time per frame */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 40);
  snprintf(buffer, sizeof buffer, "ft (ms/f): %5.0f", 1.0 / sd.frameRate * 1000.0);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  /* Tesselation */
  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i(10, 20);
  snprintf(buffer, sizeof buffer, "tesselation %5.0f", tesselation);
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

void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  drawAxes(1.0);

  //Boat #1
  drawBoat(boat1, true);
  drawCB(boat1);
  drawNet(boat1);
  drawProjectilePath(boat1);
  drawHealthBar(boat1, 0.9);

  //Boat #2
  drawBoat(boat2, false);
  drawCB(boat2);
  drawNet(boat2);
  drawProjectilePath(boat2);
  drawHealthBar(boat2, 0.7);

  //Island 
  drawIsland(island);
  drawCB(island);
  drawProjectilePath(island);
  drawHealthBar(island, 0.5);
  
  drawSin(tesselation);

  displayTechnicalStuff();

  if (gameOver)
  {
    glColor3f(1.0, 1.0, 1.0);
    unsigned char string [] = "!!!GAME  OVER!!!";
    glRasterPos2f(-0.25, 0.5);
    for (int i = 0; i < 16; i++) 
    {
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, string[i]);
    }
  }
  
  sd.frames++;

  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

void input(unsigned char key)
{
  switch(key)
  {
    case 'q':
      if (boat1.pc.angle - CANNON_SPEED >= 0)
      {
        boat1.pc.angle -= CANNON_SPEED;
        glutPostRedisplay();
      }
      break;

    case 'Q':
      if (boat1.pc.angle + CANNON_SPEED <= 180)
      {
        boat1.pc.angle += CANNON_SPEED;
        glutPostRedisplay();
      }
      break;

    case 'a':
      if (!collidedWithIsland(boat1, -BOAT_SPEED))
      {
        updateInitialPosition(&boat1);
        boat1.i.xPos -= BOAT_SPEED;
        glutPostRedisplay();
      }
      break;

    case 'd':
      if (!collidedWithIsland(boat1, BOAT_SPEED))
      {
        updateInitialPosition(&boat1);
        boat1.i.xPos += BOAT_SPEED;
        glutPostRedisplay();
      }
      break;
    
    case 'e':
      if (!boat1.i.fire) 
      {
        boat1.tProj.startTime = globalTime;
        updateProjectileState(&boat1, false);
        boat1.i.fire = true;
        glutPostRedisplay();
      }
      break;
    
    case 'r':
      if (!boat1.i.fireNet)
      {
        boat1.tNet.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState(&boat1, true);
        boat1.i.fireNet = true;
        glutPostRedisplay();
      }
      break;
    
    case 'o':
      if (boat2.pc.angle + CANNON_SPEED <= 180)
      {
        boat2.pc.angle += CANNON_SPEED;
        glutPostRedisplay();
      }
      break;

    case 'O':
      if (boat2.pc.angle - CANNON_SPEED >= 0)
      {
        boat2.pc.angle -= CANNON_SPEED;
        glutPostRedisplay();
      }
      break;

    case 'j':
      if (!collidedWithIsland(boat2, -BOAT_SPEED))
      {
        updateInitialPosition(&boat2);
        boat2.i.xPos -= BOAT_SPEED;
        glutPostRedisplay();
      }
      break;

    case 'l':
      if (!collidedWithIsland(boat2, BOAT_SPEED))
      {
        updateInitialPosition(&boat2);
        boat2.i.xPos += BOAT_SPEED;
        glutPostRedisplay();
      }
      break;
    
    case 'i':
      if (!boat2.i.fire) 
      {
        boat2.tProj.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState(&boat2, false);
        boat2.i.fire = true;
        glutPostRedisplay();
      }
      break;

    case 'u':
      if (!boat2.i.fireNet)
      {
        boat2.tNet.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState(&boat2, true);
        boat2.i.fireNet = true;
        glutPostRedisplay();
      }
      break;

    case 'f':
      if (island.pc.angle + CANNON_SPEED <= 180)
      {
        island.pc.angle += CANNON_SPEED;
        glutPostRedisplay();
      }
      break;

    case 'h':
      if (island.pc.angle - CANNON_SPEED >= 0)
      {
        island.pc.angle -= CANNON_SPEED;
        glutPostRedisplay();
      }
      break;

    case 32:
      if (!island.i.fire) 
      {
        island.tProj.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState(&island, false);
        island.i.fire = true;
      }
      break;

    default:
      break;
  }
  
}

void keyboard (unsigned char key, int x, int y)
{
  if (!gameOver)
  {
    input(key);
  }

  switch (key)
  {
    case 'n':
      if (allowNormals)
      {
        allowNormals = false;
        glutPostRedisplay();
      }
      else
      {
        allowNormals = true;
        glutPostRedisplay();
      }
      break;
    
    case 't':
      if (allowTangents)
      {
        allowTangents = false;
        glutPostRedisplay();
      }
      else
      {
        allowTangents = true;
        glutPostRedisplay();
      }
      break;

    case 'w':
      if (wireFrameMode)
      {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        wireFrameMode = false;
        glutPostRedisplay();
      }
      else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        wireFrameMode = true;
        glutPostRedisplay();
      }
      break;
    
    case '+':
      tesselation += 2;
      glutPostRedisplay();
      break;
    
    case '-':
      tesselation -= 2;
      glutPostRedisplay();
      break;

    case 'g':
      if (allowAnimation)
      {
        allowAnimation = false;
        glutPostRedisplay();
      }
      else 
      {
        allowAnimation = true;
        glutPostRedisplay();
      }
      break;

    case 27:
      exit (0);
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_BLEND );
  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}
