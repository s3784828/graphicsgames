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


enum { shoulder, elbow, wrist, left_plate, right_plate, n_joints } Joint;
float robot_arm[n_joints] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
float increment[n_joints] = { 1.0, 1.0, 1.0, 0.01, 0.01 };

void init(void)
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
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

void myWireRect(float l, float h)
{
  glPushMatrix();
    glScalef(l, h, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.5, -0.5);
    glVertex2f(0.5, -0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(-0.5, 0.5);
    glEnd();
  glPopMatrix();
}

void myWireRhombus(float l, float h)
{
  glPushMatrix();
    glScalef(l, h, 1.0);
    glScalef(1.0/sqrt(2.0), 1.0/sqrt(2.0), 1.0);
    glRotatef(45.0, 0.0, 0.0, 1.0);
    myWireRect(1.0, 1.0);
  glPopMatrix();
}

void drawArm(void)
{
  glPushMatrix();

  // Shoulder
  glTranslatef(0.0, 0.0, 0.0);
  glRotatef(robot_arm[shoulder], 0.0, 0.0, 1.0);
  drawAxes(LINE_SIZE);
  // Upper arm
  glTranslatef(0.2, 0.0, 0.0);
  myWireRhombus(0.4, 0.2);
  drawAxes(LINE_SIZE);
  // Elbow
  glTranslatef(0.2, 0.0, 0.0);
  glRotatef(robot_arm[elbow], 0.0, 0.0, 1.0);
  drawAxes(LINE_SIZE);
  // Lower arm
  glTranslatef(0.0, -0.2, 0.0);
  myWireRhombus(0.2, 0.4);
  drawAxes(LINE_SIZE);
  // Wrist
  glTranslatef(0.0, -0.2, 0.0);
  glRotatef(robot_arm[wrist], 0.0, 0.0, 1.0);
  drawAxes(LINE_SIZE);
  // Hand
  glTranslatef(0.0, -0.025, 0.0);
  myWireRect(0.2, 0.05);
  drawAxes(LINE_SIZE);
  // Gripper
  glPushMatrix();
  glTranslatef(-0.07+robot_arm[left_plate], -0.1, 0.0);
  myWireRect(0.05, 0.125);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(0.075-robot_arm[right_plate], -0.1, 0.0);
  myWireRect(0.05, 0.125);
  glPopMatrix();

  glPopMatrix();
}

void display(void)
{
  int err;

  glClear (GL_COLOR_BUFFER_BIT);

  drawArm();
  glutSwapBuffers();

  if ((err = glGetError()) != GL_NO_ERROR) {
    printf("display: %s\n",gluErrorString(err));
  }
}

void reshape (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  //Shoulder
  case 'S':
    robot_arm[shoulder] += increment[shoulder];
    glutPostRedisplay();
    break;
  case 's':
    robot_arm[shoulder] -= increment[shoulder];
    glutPostRedisplay();
    break;
  //Elbow
  case 'E':
    robot_arm[elbow] += increment[elbow];
    glutPostRedisplay();
    break;
  case 'e':
    robot_arm[elbow] -= increment[elbow];
    glutPostRedisplay();
    break;
  //Wrist
  case 'W':
    robot_arm[wrist] += increment[wrist];
    glutPostRedisplay();
    break;
  case 'w':
    robot_arm[wrist] -= increment[wrist];
    glutPostRedisplay();
    break;
  //Left plate
  case 'L':
    robot_arm[left_plate] += increment[left_plate];
    glutPostRedisplay();
    break;
  case 'l':
    robot_arm[left_plate] -= increment[left_plate];
    glutPostRedisplay();
    break;
  //Right plate
  case 'R':
    robot_arm[right_plate] += increment[right_plate];
    glutPostRedisplay();
    break;
  case 'r':
    robot_arm[right_plate] -= increment[right_plate];
    glutPostRedisplay();
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
  glutCreateWindow (argv[0]);
  init ();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
