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

bool debug = true;
typedef struct
{
  float x;
  float y;
  float speed;
} MouseRotations;

typedef struct
{
  float x;
  float z;
} Vector2;

bool rotateY = true;
MouseRotations rot = {0.0, 0.0, 1.0};

// Utility function for drawing axes
void drawAxes(float len)
{
  glBegin(GL_LINES);
  glColor3f(0.0, 1.0, 0.0);
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

void drawSquare(float x, float z)
{

    
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glVertex3f(x, 0.0, z);
    glVertex3f(x, 0.0, -z);
    glVertex3f(-x, 0.0, -z);
    glVertex3f(-x, 0.0, z);

    glEnd();
}

void drawTriangles(float x, float z)
{

    
    glBegin(GL_TRIANGLES);
    glColor3f(1,1,1);
    glVertex3f(x, 0.0, z);
    glVertex3f(x, 0.0, -z);
    glVertex3f(-x, 0.0, -z);
    glVertex3f(-x, 0.0, -z);
    glVertex3f(-x, 0.0, z);
    glVertex3f(x, 0.0, z);

    glEnd();
}

void drawGrid(float cols, float rows)
{
  float xInc = 0.025;
  float zInc = 0.025;

  Vector2 iterPos = {0, 0};

  Vector2 startPoints[] = {{xInc, zInc}, {-xInc, zInc}, {-xInc, -zInc}, {xInc, -zInc}};

  
  for (int segment = 0; segment < 4; segment++)
  {
    glPushMatrix();
    glTranslatef(startPoints[segment].x, 0.0, startPoints[segment].z);
    drawTriangles(xInc, zInc);
    for (int col = 0; col < cols; col++)
    {
      if (col != 0) 
      {
        if (startPoints[segment].x > 0)
          glTranslatef(xInc * 2, 0.0, 0.0);
        else 
          glTranslatef(-xInc * 2, 0.0, 0.0);

        if (startPoints[segment].z > 0)
          glTranslatef(0.0, 0.0, -zInc * 2 * rows);
        else 
          glTranslatef(0.0, 0.0, zInc * 2 * rows);

        drawTriangles(xInc,zInc);
      }
      for(int row = 0; row < rows; row++)
      { 
        if (startPoints[segment].z > 0)
          glTranslatef(0.0, 0.0, zInc * 2); 
        else 
          glTranslatef(0.0, 0.0, -zInc * 2);

        drawTriangles(xInc, zInc);
      }

    }
    glPopMatrix();
  }
  

}

// Display callback
void display()
{
  int err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST);
  glLoadIdentity();
  //drawAxes(0.90);
  

  /* Modifications for GL_LIGHT0 */
  //glShadeModel(GL_FLAT);
  //glEnable(GL_LIGHTING);
  //glEnable(GL_LIGHT0);
  float lightpos[] = {2.0, 0.0, 0.0, 0.0};
  float lamb[] = {0.35, 0.35, 0.35, 0.0};
  float ldiff[] = {0.5, 0.5, 0.5, 0.0};
  float lspec[] = {0.25, 0.25, 0.25, 0.0};
  //float lig

  float amb[] = {1.0, 0.0, 0.0, 1.0};
  float diff[] = {1.0, 0.0, 0.0, 1.0};
  float spec[] = {1.0, 1.0, 1.0, 1.0};

  
  // glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  // glLightfv(GL_LIGHT0, GL_AMBIENT, lamb);
  // glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiff);
  // glLightfv(GL_LIGHT0, GL_SPECULAR, lspec);


  // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);

  /* DISABLE TO NOT HAVE WIREFRAME MODE */
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glPushMatrix();
    // if (rotateY)
    //   glRotatef(rot.x, 0.0, 1.0, 0.0);
    // else
    //   glRotatef(rot.y, 1.0, 0.0, 0.0);
    glRotatef(rot.x, 0.0, 1.0, 0.0);
    glRotatef(rot.y, 1.0, 0.0, 0.0);
    //glutSolidSphere(0.25, 8, 8);
    //glutSolidTorus(0.25, 0.5, 8, 8);
    
    //drawTriangles(1, 1);
    drawGrid(5, 5);
    //glColor3f(0, 1, 0);
    drawAxes(0.25);
  glPopMatrix();
  

  if ((err = glGetError()) != GL_NO_ERROR)
    fprintf(stderr, "Error: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

void mouseMotion(int x, int y)
{
  
  if (x < 250)
  {
    rot.x -= rot.speed;
  }
  else
  {
    rot.x += rot.speed;
  }

  if (y < 250)
  {
    rot.y -= rot.speed;
  }
  else
  {
    rot.y += rot.speed;
  }
  //printf("MOTION DETECTED, x: %f, y: %f\n", rot.x, rot.y);
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 27:
  case 'x':
    if (rotateY)
      rotateY = false;
    else
      rotateY = true;
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
  glutCreateWindow("LIGHTING");
  glEnable(GL_DEPTH_TEST);
  glutDisplayFunc(display);
  glutMotionFunc(mouseMotion);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}
