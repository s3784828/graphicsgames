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
const float projectileDamage = 10;
const float cannonCount = 3;
const float arrayLength = 3;

const float boatRadius = 0.5 * boatScale;
const float islandRadius = 0.25;
const float projRadius = 0.125;

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
  Vector2 r0;
  Vector2 r;
} Position;

typedef struct
{
  PolarCoordinate pc;
  Projectile p;
  Collider c;
  InputUtility i;
  Vector2 trans;
  float hyp;
  float health;
  float dt;
  float startTime;
  Vector2 r0;
  bool isIsland;
} Object;

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

Object boat1 
{
  {0.5, 0.0},
  {{0,0}, {0,0}, {0,0}, {0,0}, projRadius},
  {{0.0, boatRadius}},
  {false, -0.5},
  {0.5, 0.5},
  0.5,
  100.0,
  0.0,
  0.0,
  {0.0, 0.0},
  false
};

/*
* BOAT #2
*/

Object boat2
{
  {0.5, 180.0},
  {{0,0}, {0,0}, {0,0}, {0,0}, projRadius},
  {{0.0, boatRadius}},
  {false, 0.5},
  {-0.5, 0.5},
  0.5,
  100.0,
  0.0,
  0.0,
  {0.0, 0.0},
  false
};

/*
* ISLAND
*/

Object island
{
  {0.35, 0.0},
  {{0,0}, {0,0}, {0,0}, {0,0}, projRadius},
  {{0.0, islandRadius}},
  {false, 0.0},
  {0.0, 0.25},
  0.25,
  100.0,
  0.0,
  0.0,
  {0.0, 0.0},
  true
};

/*
* ARRAY OF OBJECTS
*/

Object arrayObjects[] = {boat1, boat2, island};

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

void updateProjectileStateNumerical(Object* obj, float dt) 
{
  obj->p.r0 = obj->p.r;
  // Position
  obj->p.r.x += obj->p.v.x * dt;
  obj->p.r.y += obj->p.v.y * dt;

  // Velocity
  obj->p.v.y += g * dt;
}

Vector2 calculateR0(Object* obj, float hyp, float xInc, float yInc, float s)
{
  hyp *= s;
  xInc *= s;
  yInc *= s;

  float radangle = toRad(findAngle(obj->i.xPos));
  float angle = findAngle(obj->i.xPos);

  if (!obj->isIsland)
  {

    //float _x = cosf(toRad(findAngle(obj->i.xPos))) * xInc;
    //float _y = sinf(toRad(findAngle(obj->i.xPos))) * yInc; 

    float coolAngle = atanf(xInc * yInc);

   // xInc += obj->i.xPos;
   // yInc += sinValue(obj->i.xPos);


    float _x = cosf(toRad(findAngle(obj->i.xPos))) * xInc;
    float _y = sinf(toRad(findAngle(obj->i.xPos))) * yInc; 

    float x = hyp * cosf(toRad(findAngle(obj->i.xPos) + obj->pc.angle)) + _x + obj->i.xPos;
    float y = hyp * sinf(toRad(findAngle(obj->i.xPos) + obj->pc.angle)) + _y + sinValue(obj->i.xPos);
    return {x, y};
  }

  float x = hyp * cosf(toRad(obj->pc.angle)) + xInc;
  float y = hyp * sinf(toRad(obj->pc.angle)) + yInc;

  return {x, y};
}

Vector2 calculateV0(Object* obj) 
{
  float angle = obj->pc.angle;
  if (!obj->isIsland)
  {
   angle += findAngle(obj->pc.angle);
  }
  return {obj->pc.mag * cosf(toRad(angle)), obj->pc.mag * sinf(toRad(angle))};
}

void updateProjectileStateInitial(Object* obj) 
{
  //Object boat = *boatObject;
  float scale = boatScale;
  if (obj->isIsland)
  {
    scale = 1.0;
  }
  obj->p.r0 = calculateR0(obj, obj->hyp, obj->trans.x, obj->trans.y, scale);
  obj->p.v0 = calculateV0(obj);
  //printf("x = %f, y = %f\n", boat.p.r0.x, boat.p.r0.y);
}

void updateProjectileState(Object* obj)
{
  obj->p.r = obj->p.r0;
  obj->p.v = obj->p.v0;
}

void updateInitialPosition(Object* obj)
{
  if (obj->r0.x != obj->i.xPos)
  {
    obj->r0.x = obj->i.xPos;
  }

  if (obj->r0.y != obj->i.xPos)
  {
    obj->r0.y = sinValue(obj->r0.x);
  }
}

float getDistanceObject(Object current, Object target)
{
  float distance = sqrt(pow(current.r0.x - target.r0.x, 2) + pow(current.i.xPos - target.i.xPos, 2));
  return distance;
}

Vector2 getDistanceProjectile(Object* current, Object* target)
{
  float distanceX = sqrt(pow(current->p.r0.x - target->r0.x, 2) + pow(current->p.r.x - target->i.xPos, 2));
  float distanceY = sqrt(pow(current->p.r0.y - target->r0.y, 2) + pow(current->p.r.y - sinValue(target->i.xPos), 2));
  //printf("x = %f\n, y = %f/n", distanceX, distanceY);
  return {distanceX, distanceY};
}

Vector2 getDistanceIsland(Object* current) 
{
  float distanceX = sqrt(pow(current->p.r0.x, 2) + pow(current->p.r.x, 2));
  float distanceY = sqrt(pow(current->p.r0.y, 2) + pow(current->p.r.y, 2));
  //printf("x = %f, y = %f, sum = %f, proj = %f, island = %f\n", distanceX, distanceY, current->p.radius + island.c.radius, current->p.radius, island.c.radius);
  return {distanceX, distanceY};
}

void printDistance(Object* current, Object* target)
{
  float distanceX = sqrt(pow(current->p.r0.x - target->r0.x, 2) + pow(current->p.r.x - target->i.xPos, 2));
  float distanceY = sqrt(pow(current->p.r0.y - target->r0.y, 2) + pow(current->p.r.y - sinValue(target->i.xPos), 2));
  //printf("x = %f\n, y = %f, sum = %f/n", distanceX, distanceY, current->p.radius + target->c.radius);
}

// void printDistanceMovement(Object* current, Object* target)
// {
//   float distanceX = sqrt(pow(current->r0 - target->r0.x, 2) + pow(current->p.r.x - target->i.xPos, 2));
//   float distanceY = sqrt(pow(current->p.r0.y - target->r0.y, 2) + pow(current->p.r.y - target->i.xPos, 2));
//   printf("x = %f\n, y = %f/n", distanceX, distanceY);
// }

bool collided(Object* current, Object* target)
{
  bool collided = false;
  Vector2 distance;
  if (target->isIsland)
  {
    distance = getDistanceIsland(current);
  } 
  else 
  {
    distance = getDistanceProjectile(current, target);
  }
  //float sum = current->p.radius + target->c.radius;
  float sum = current->p.radius + target->c.radius;
  if (distance.x <= sum && distance.y <= sum)
  {
    collided = true;
    target->health -= projectileDamage;
    printf("collided, target health = %f\n", target->health);
  }
  return collided;
}

bool collidedWithIsland(Object obj, float inc)
{
  float sum = obj.c.radius + island.c.radius;
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

bool collidedObjects(Object* obj)
{
  bool collidedObject = false;
  for (int i = 0; i < arrayLength; ++i)
  {
    //Object target = arrayObjects[i];
    if (collided(obj, &arrayObjects[i]))
    {
      collidedObject = true;
    }
  }
  return collidedObject;
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

/*
* CONTROLLED
*/

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

void drawCB(Object obj)
{
  if (obj.i.fire) 
  {
    glPushMatrix();
      glTranslatef(obj.p.r.x, obj.p.r.y, 0.0);
      glColor3f(1, 1, 1);
      glutSolidSphere(projRadius * 0.10, 20, 20);
    glPopMatrix();
  }
  else 
  {
    glPushMatrix();
      glTranslatef(obj.p.r0.x, obj.p.r0.y, 0.0);
      glColor3f(1, 1, 1);
      glutSolidSphere(projRadius * 0.10, 20, 20);
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
    glScalef(boatScale, boatScale, boatScale);
    glRotatef(findAngle(boat.i.xPos), 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.25, 0.0);
    drawHull();
    //Bridge
    glTranslatef(0.0, 0.5, 0.0);
    drawBridge();
    //CannonSocket
    glTranslatef(cannonPosition, -0.25, 0.0);
    glRotatef(boat.pc.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.25, 0.0, 0.0);
    drawCannon();
    //CannonBallLocation
    glTranslatef(0.25, 0.0, 0.0);
    //glutWireSphere(20 * 0.005, 5, 5);
  glPopMatrix();
}

void drawIslandBase(float width, float height) 
{
  glBegin(GL_QUADS);
  glColor3f(1, 0, 0);
  glVertex3f(width, -1.0, 0.0);
  glVertex3f(-width, -1.0, 0.0);
  glVertex3f(-width, height, 0.0);
  glVertex3f(width, height, 0.0);
  glEnd();
}

void drawCannon1(float width, float height)
{
  glBegin(GL_QUADS);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(-width, -height, 0);
  glVertex3f(width, -height, 0.0);
  glVertex3f(width, height, 0.0);
  glVertex3f(-width, height, 0.0);
  glEnd();
}

void drawIslandCannon(float width, float height)
{
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 1.0, 1.0);
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
    drawIslandBase(0.25, 0.25);
    //CannonSocket
    glTranslatef(0.0, 0.25, 0.0);
    glRotatef(island.pc.angle, 0.0, 0.0, 1.0);
    //Cannon
    glTranslatef(0.125, 0.0, 0.0);
    //drawIslandCannon(0.025, 0.125);
    drawCannon1(0.125, 0.025);
  glPopMatrix();
}

void drawHealthBar(Object obj, float yTrans, float r, float g, float b) 
{
  float x = obj.health * 0.010;
  float y = 0.05;
  glPushMatrix();
    glTranslatef(-1.0, yTrans, 0.0);
    glBegin(GL_QUADS); 
    glColor3f(r, g, b);
    glVertex3f(0.0, y, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(x, 0.0, 0.0);
    glVertex3f(x, y, 0.0);
    glEnd();
  glPopMatrix();
}

void drawProjectilePath(Object obj)
{
  if (!obj.i.fire)
  {
    return;
  }

  float dt; 
  Vector2 r, v;
  dt = obj.dt;
  r = obj.p.r;
  v = obj.p.v;
  glBegin(GL_LINE_STRIP);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(r.x, r.y, 0.0);
  while(!collidedOcean(r))
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

void setRadius(Object* island)
{
  island->c.radius = islandRadius;
}

/*
* OPENGL UTILITIES
*/

void updateObject(Object* obj, Object* target, Object* target1)
{
  float t, dt;
  static float tLast = 0.0;

  t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0 - obj->startTime;

  dt = t - tLast;
  obj->dt = dt;
  updateProjectileStateNumerical(obj, dt);

  //getDistanceProjectile1(obj, target);

  // if (obj->isIsland)
  // {

  // }
  // else
  // {
    
  // } 

  if (collided(obj, target) || collided(obj, target1) || collidedOcean(*obj))
  {
    glutPostRedisplay();
    obj->i.fire = false;
  }

  // if (collided(obj, target) || collided(obj, &island))
  // {
  //   glutPostRedisplay();
  //   obj->i.fire = false;
  // } 

  tLast = t;

  glutPostRedisplay();
}

void update()
{
  // float t, dt;
  // static float tLast = 0.0;

  timeUtil.t = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;

  //Object* boat = &boat1;
 
  updateProjectileStateInitial(&boat1);
  updateProjectileStateInitial(&boat2);
  updateProjectileStateInitial(&island);

  if (boat1.i.fire) 
  {
    updateObject(&boat1, &island, &boat2);
  }

  if (boat2.i.fire)
  {
    updateObject(&boat2, &island, &boat1);
  }

  if (island.i.fire)
  {
    updateObject(&island, &boat1, &boat2);
  }

  glutPostRedisplay();
}

void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  drawAxes(1.0);

  //Boat #1
  if (boat1.health != 0)
  {
    updateInitialPosition(&boat1);
    drawBoat(boat1, true);
    drawCB(boat1);
    drawProjectilePath(boat1);
    drawHealthBar(boat1, 0.9, 1.0, 0.0, 0.0);
  }

  // if (boat1.r0.x == 0)
  // {
  //   updateInitialPosition(&boat1);
  // }

  // //Boat #2
  // if (boat2.health != 0)
  // {
  //   updateInitialPosition(&boat2);
  //   drawBoat(boat2, false);
  //   drawCB(boat2);
  //   drawProjectilePath(boat2);
  //   drawHealthBar(boat2, 0.8, 0.0, 1.0, 0.0);
  // }

  // if (boat2.r0.y == 0)
  // {
  //   updateInitialPosition(&boat2);
  // }

  //Island
  // if (island.health != 0)
  // {
  //   drawIsland(island);
  //   drawCB(island);
  //   drawProjectilePath(island);
  //   drawHealthBar(island, 0.85, 0.0, 0.0, 1.0);
  //   setRadius(&island);
  // }

  drawSin(35);
  
  
  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key)
  {
    case 'q':
      if (boat1.pc.angle - 0.75 >= 0)
      {
        boat1.pc.angle -= 0.75;
        glutPostRedisplay();
      }
      break;

    case 'Q':
      if (boat1.pc.angle + 0.75 <= 90)
      {
        boat1.pc.angle += 0.75;
        glutPostRedisplay();
      }
      break;

    case 'a':
      if (!collidedWithIsland(boat1, -movementSpeed))
      {
        updateInitialPosition(&boat1);
        boat1.i.xPos -= movementSpeed;
        glutPostRedisplay();
      }
      break;

    case 'd':
      if (!collidedWithIsland(boat1, movementSpeed))
      {
        updateInitialPosition(&boat1);
        boat1.i.xPos += movementSpeed;
        glutPostRedisplay();
      }
      break;
    
    case 'e':
      if (!boat1.i.fire) 
      {
        boat1.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState(&boat1);
        boat1.i.fire = true;
      }
      else 
      {
        boat1.i.fire = false;
      }
      break;
    
    case 'o':
      if (boat2.pc.angle + 0.75 <= 180)
      {
        boat2.pc.angle += 0.75;
        glutPostRedisplay();
      }
      break;

    case 'O':
      printf("pressed o\n");
      if (boat2.pc.angle - 0.75 >= 90)
      {
        boat2.pc.angle -= 0.75;
        glutPostRedisplay();
      }
      break;

    case 'j':
      printf("pressed j\n");
      if (!collidedWithIsland(boat2, -movementSpeed))
      {
        updateInitialPosition(&boat2);
        boat2.i.xPos -= movementSpeed;
        glutPostRedisplay();
      }
      break;

    case 'l':
      printf("pressed l\n");
      if (!collidedWithIsland(boat2, movementSpeed))
      {
        updateInitialPosition(&boat2);
        boat2.i.xPos += movementSpeed;
        glutPostRedisplay();
      }
      break;
    
    case 'i':
      printf("pressed i\n");
      if (!boat2.i.fire) 
      {
        boat2.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState(&boat2);
        boat2.i.fire = true;
      }
      break;

    case 'r':
      if (island.pc.angle + 0.75 <= 180)
      {
        printf("pressed f\n");
        printf("island angle %f\n", island.pc.angle);
        island.pc.angle += 0.75f;
        glutPostRedisplay();
      }
      break;
    
    case 'f':
      if (island.pc.angle - 0.75 >= 0)
      {
        printf("pressed f\n");
        printf("island angle %f\n", island.pc.angle);
        island.pc.angle -= 0.75f;
        glutPostRedisplay();
      }
      break;

    case 'h':
      if (island.pc.angle - 0.75 >= 0)
      {
        printf("pressed h\n");
        island.pc.angle -= 0.75f;
        printf("island angle %f\n", island.pc.angle);
        glutPostRedisplay();
      }
      break;
    
    case 32:
      if (!island.i.fire) 
      {
        island.startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
        updateProjectileState(&island);
        island.i.fire = true;
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
