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

float toRad(float angle)
{
  return angle * (PI / 180);
}
float vel = 0.5;
float startTime = 0.0;
float g = -0.25;
float bAngle = 0.0;
typedef struct {float x, y; } V2;
typedef struct {V2 r0, v0, r, v; } C;
typedef struct {float angle, length; } P;
typedef struct {bool fire; } F;
F f = {false};
P p = {-90, 0.5};
//C c = {{0.0, 0.0}, {vel * cosf(toRad(p.angle + bAngle)), vel * sinf(toRad(p.angle + bAngle))}, {0,0}, {0,0}};
C c = {{0.0, 0.0}, {0, 0}, {0,0}, {0,0}};



V2 rr = {0.0, 0.0};

void updateProjectileStateNumerical(float dt)
{
  // Euler integration

  // Position
  c.r.x += c.v.x * dt;
  c.r.y += c.v.y * dt;

  // Velocity
  c.v.y += g * dt;
}

void updateState()
{
  //c.r0.x = p.length * cosf(toRad(p.angle + bAngle + 90)) + 0.25 * cosf(toRad(bAngle));
  c.r0.x = p.length * cosf(toRad(p.angle + bAngle + 90)) + 0.5;
  c.r0.y = p.length * sinf(toRad(p.angle + bAngle + 90));
  //c.r0.y = p.length * sinf(toRad(p.angle + bAngle + 90)) + 0.25 * sinf(toRad(bAngle));
  c.v0.x = vel * cosf(toRad(p.angle + bAngle + 90));
  c.v0.y = vel * sinf(toRad(p.angle + bAngle + 90));
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

void rect(float l, float h)
{
  glPushMatrix();
    glScalef(l, h, 1.0);
    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    glVertex2f(-0.5, -0.5);
    glVertex2f(0.5, -0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(-0.5, 0.5);
    glEnd();
  glPopMatrix();
}

void drawCannonFull() 
{
  glPushMatrix();
    glTranslatef(1, 0.0, 0.0);
    glRotatef(bAngle, 0.0, 0.0, 1.0);
    //glTranslatef(0.25, 0.25, 0.0);
    glRotatef(p.angle, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.25, 0.0);
    rect(0.05, 0.5);
  glPopMatrix();
}

void drawLine(float left, float right) 
{
  glPushMatrix();
    glTranslatef(1, 0.0, 0.0);
    glRotatef(bAngle, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    glVertex3f(left, 0.0, 0.0);
    glVertex3f(right, 0.0, 0.0);
    glEnd();
  glPopMatrix();
}

void update(void)
{
  printf("r0x: %f, r0y: %f\n", c.r0.x, c.r0.y);
  static float lastT = 0.0;
  float t, dt;

  if (!f.fire) 
  {
    updateState();
    return;
  }

  t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - startTime;
  
  dt = t - lastT;
  printf("t: %f, dt: %f, lastT: %f\n", t, dt, lastT);
  updateProjectileStateNumerical(dt);
  lastT = t;

  if (c.r.y < 0) {
    f.fire = false;
  }
  glutPostRedisplay();
}

void display(void)
{
  int err;

  //glClear (GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawAxes(1);
  drawCannonFull();
  drawLine(-1.0, 1.0);

  if (f.fire) {
    glPushMatrix();
      glTranslatef(c.r.x, c.r.y, 0.0);
      glutWireSphere(0.05,8,3);
    glPopMatrix();
  } else {
    glPushMatrix();
      glTranslatef(c.r0.x, c.r0.y, 0.0);
      glutWireSphere(0.05,8,3);
    glPopMatrix();
  }

  glutSwapBuffers();

  if ((err = glGetError()) != GL_NO_ERROR) {
    printf("display: %s\n",gluErrorString(err));
  }
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  case 'a':
    p.angle += 0.75f;
    updateState();
    glutPostRedisplay();
    break;
  case 'A':
    p.angle -= 0.75f;
    updateState();
    glutPostRedisplay();
    break;
  case 'b':
    bAngle += 0.75f;
    updateState();
    glutPostRedisplay();
    break;
  case 'B':
    bAngle -= 0.75f;
    updateState();
    glutPostRedisplay();
    break;
  case 'f':
  printf("fire called");
    if (!f.fire) {
      startTime = glutGet(GLUT_ELAPSED_TIME) / (float) 1000.0;
      c.r = c.r0;
      c.v = c.v0;
      f.fire = true;
      
    }
    break;
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
  glutCreateWindow ("lab05");
  glutIdleFunc(update);
  glutDisplayFunc(display);
  //glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
