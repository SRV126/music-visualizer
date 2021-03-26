#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl/glut.h>

#ifdef GL_VERSION_1_1

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265
#endif

#define MAXSIZE 512             /* Set this to your maximum texture size (square)
*/
#define TEXT    "Welcome"

float ang = 2.0;
float scale = 1.05;
float tx = 0.0, ty = 0.0;

int oldx, oldy;
int lmb = 0;
int mmb = 0;
int autospin = 0;
float atime = 0.0;

int smooth = 1;
int seedmode = 0;
float seedsize = 0.5;

int primtype = GL_LINES;
float primsize = 1.0;
int nprims = 10;

float r, g, b;
float dr, dg, db;
int randomcolours = 0;


/* returns a random floating point number between 0.0 and 1.0 */
float frand(void) {
    return (float) (rand() / 32767.0);
}

void init_colours(float speed)
{
    r = frand(); g = frand(); b = frand();
    dr = frand() / speed; dg = frand() / speed; db = frand() / speed;    
}

void bounce(float *n, float *dn)
{
    *n += *dn;
    if (*n > 1.0) { *n = 1.0; *dn = -*dn; }
    if (*n < 0.0) { *n = 0.0; *dn = -*dn; }
}

/* generate pretty colours by bouncing rgb values up and down */
void set_colour(void)
{
    if (randomcolours) {
        glColor3f(frand(), frand(), frand());
    } else {
        bounce(&r, &dr);
        bounce(&g, &dg);
        bounce(&b, &db);
        glColor3f(r, g, b);
    }
}


/* seed pattern with some random primitives in centre of screen */
void seed(void)
{
    int i;
        
    glBegin(primtype);
    for(i=0; i<nprims; i++) {
        set_colour();
        glVertex2f((frand() - 0.5) * seedsize, (frand() - 0.5) * seedsize);
    }
    glEnd();
}


/* seed pattern with a circular pattern */
void seed_circle(void)
{   
    int i;
    double a;
        
    glBegin(primtype);
        for(i=0; i<nprims; i++) {
        a = ((double) i * 2 * M_PI) / nprims;
        glColor4f(0.0, 0.0, 0.0, 1.0);
        glVertex2d(0.0, 0.0);
        set_colour();
        glVertex2d(sin(a) * (seedsize / 2.0), cos(a) * (seedsize / 2.0));
    }
    glEnd();    
}

/* seed with text string */
void seed_text(char *string)
{
    int i;
    int width = 0;
    
    for (i = 0; i < strlen(string); i++) {
        width += glutStrokeWidth(GLUT_STROKE_ROMAN, string[i]);
    }
    
    glLoadIdentity();
    glScalef(seedsize / 200.0, seedsize / 200.0, seedsize / 200.0);
    glTranslatef(-width / 2.0, -50.0, 0.0);
    
    for (i = 0; i < strlen(string); i++) {
        set_colour();
        glutStrokeCharacter(GLUT_STROKE_ROMAN, string[i]);
    }
}

/* copy screen image to texture memory */
void grab_screen(void)
{
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, MAXSIZE, MAXSIZE,0);

    if (smooth) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}


void redraw(void)
{
    glClear(GL_COLOR_BUFFER_BIT);           

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(tx, ty, 0.0);
    glRotatef(ang, 0.0, 0.0, 1.0);
    glScalef(scale, scale, scale);

    if (autospin) {
        ang = 3.0 * cos(atime);
        scale = 1.0 + ( sin(atime / 4.0) * 0.1) ;
        atime += 0.01;
    }

    /* draw feedback square */
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
        glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
        glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    /* draw square outline */
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-1.0, -1.0);
        glVertex2f(1.0, -1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(-1.0, 1.0);
    glEnd();
    
    /* seed pattern */
    glLoadIdentity();
    switch(seedmode) {
    case 0:
        seed();
        break;
    case 1:
        seed_circle();
        break;
    case 2:
        seed_text(TEXT);
        break;
    }

    /* grab screen as texture */
    grab_screen();
     
    glutSwapBuffers();
}


void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lmb = 1;
        oldx = x; oldy = y;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        lmb = 0;
    }
  
    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        mmb = 1;
        oldx = x; oldy = y;
    }
    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
        mmb = 0;
    }

}

void motion(int x, int y)
{
    if (lmb) {
        ang += ((oldx - x) / 4.0 );
        scale += ((oldy - y) / 400.0);

        oldx = x; oldy = y;
        glutPostRedisplay();
    }
    if (mmb) {
        tx += ((float) (x - oldx)) / 500.0;
        ty += ((float) (oldy - y)) / 500.0;

        oldx = x; oldy = y;
        glutPostRedisplay();      
    }
}


void main_menu(int i)
{
    switch(i) {
    case 1:
        autospin = !autospin;
        atime = 0.0;
        break;
    case 2:
        exit(0);
    }
}

void mode_menu(int i)
{
    smooth = i;
}

void seed_menu(int i)
{
    seedmode = i;
}

void prim_menu(int i)
{
    primtype = i;
}


void psize_menu(int i)
{
    primsize = (float) i;

    glPointSize(primsize);
    glLineWidth(primsize);
}


void no_menu(int i)
{
    nprims = i;
}


void colour_menu(int i)
{
    switch(i) {
    case 0:
        init_colours(5000.0);
        randomcolours = 0;
        break;
    case 1:
        init_colours(1000.0);
        randomcolours = 0;
        break;
    case 2:
        init_colours(100.0);
        randomcolours = 0;
        break;
    case 3:
        randomcolours = 1;
        break;
    }
}

int mainmenu;
int modemenu, seedmenu, primmenu, sizemenu, psizemenu, nomenu,
colourmenu;

void create_menus(void)
{
    modemenu = glutCreateMenu(mode_menu);    
    glutAddMenuEntry("Chunky", 0);
    glutAddMenuEntry("Smooth", 1);

    seedmenu = glutCreateMenu(seed_menu);
    glutAddMenuEntry("Primitive", 0);
    glutAddMenuEntry("Circle", 1);

    colourmenu = glutCreateMenu(colour_menu);    
    glutAddMenuEntry("Classical", 0);
    glutAddMenuEntry("Jazz", 1);
    glutAddMenuEntry("Rock", 2);
    glutAddMenuEntry("Random", 3);
    
    primmenu = glutCreateMenu(prim_menu);    
    glutAddMenuEntry("Dots", GL_POINTS);
    glutAddMenuEntry("Lines", GL_LINES);
    glutAddMenuEntry("Triangles", GL_TRIANGLES);

    mainmenu = glutCreateMenu(main_menu);
    glutAddSubMenu("Visualizer", seedmenu);
    glutAddSubMenu("Type", colourmenu);
    glutAddSubMenu("Source", primmenu);
    glutAddSubMenu("Sharpness", modemenu);

    glutAddMenuEntry("-----------", -1);
    glutAddMenuEntry("Autospin", 1);
    glutAddMenuEntry("Quit", 2);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Visualizer");
    glutFullScreen();
    glutReshapeWindow(MAXSIZE,MAXSIZE);
    glutDisplayFunc(redraw);
    glutIdleFunc(redraw);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
  
    create_menus();
    seed_menu(2);
    init_colours(100.0);
  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 1.0);
    glViewport(0, 0, MAXSIZE, MAXSIZE);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutMainLoop();
    return 0;             /* ANSI C requires main to return int. */
}

#else

int
main(int argc, char** argv)
{
    fprintf (stderr, "This program demonstrates a feature which is not in OpenGL Version 1.0.\n");
    fprintf (stderr, "If your implementation of OpenGL Version 1.0 has the right extensions,\n");
    fprintf (stderr, "you may be able to modify this program to make it run.\n");
    return 0;
}

#endif

