/*********
CTIS164 - Template Source Program
----------
STUDENT : Ping
SECTION : 2
HOMEWORK: 2
----------
PROBLEMS: If your program does not function correctly,
explain here which parts are not running.
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define FIN 0
#define RUN 1
#define STOP 2
#define CON 3

typedef struct {
	int x;
	int y;
}devil_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
int sec2 = 0, sec1 = 0, msec2 = 0, msec1 = 0;
int shurikenX = 0;
int shurikenY = 0;
int kunaiX = 0, kunaiY = 0;
int ninjaY = 0;

int ranDevil = 0;
int dev = 180;
devil_t d[5];

bool shoot = false;
bool activeTimer = false;
int state = FIN;

void onTimer(int v);
						  //
						  // to draw circle, center at (x,y)
						  // radius r
						  //

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void displayBackground() {
	//time
	glColor3f(1, 1, 1);
	vprint(-310, 250, GLUT_BITMAP_9_BY_15, "TIME");
	vprint(240, 250, GLUT_BITMAP_9_BY_15, "TOTAL POINTS");

	//Top rectangle
	glColor3ub(19, 242, 201);
	glRectf(-200, 230, 200, 270);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-210, 280);
	glVertex2f(-210, 220);

	glVertex2f(210, 220);
	glVertex2f(210, 280);

	glEnd();
	

	//Name
	glColor3ub(250, 0, 255);
	print(-150, 245, "Hit the Target by Ping Cheng", GLUT_BITMAP_9_BY_15);

	//background
	glBegin(GL_QUADS);
	glColor3ub(117, 97, 97);
	glVertex2f(400, 185);
	glVertex2f(-400, 185);
	glColor3ub(31, 221, 145);
	glVertex2f(-400, -300);
	glVertex2f(400, -300);
	glEnd();

}

void displayTime() {
	glColor3f(0, 1, 0);
	vprint(-320, 220, GLUT_BITMAP_9_BY_15, "%d%d:%d%d", sec2, sec1, msec2, msec1);
}

void ninja() {
	//ninja head
	glColor3ub(168, 163, 163);
	glRectf(-400, -25 + ninjaY, -350, 25 + ninjaY);
	//skin
	glColor3ub(255, 220, 177);
	glRectf(-395, 0 + ninjaY, -355, 15 + ninjaY);
	//eyes
	glColor3f(0, 0, 0);
	glLineWidth(2);
	glBegin(GL_LINES);
	//left eyeborw
	glVertex2f(-390, 15 + ninjaY);
	glVertex2f(-380, 10 + ninjaY);
	//right eyebrow
	glVertex2f(-360, 15 + ninjaY);
	glVertex2f(-370, 10 + ninjaY);
	glEnd();
	//left eye
	circle(-385, 7 + ninjaY, 3);
	//right eye
	circle(-365, 7 + ninjaY, 3);
}

void kunai() {
	//kunai rectangle
	glColor3f(0.6, 0.6, 0.6);
	glRectf(-340 + kunaiX, -5 + kunaiY, -280 + kunaiX, 5 + kunaiY);
	//kunai lines
	glLineWidth(2);
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex2f(-325 + kunaiX, -5 + kunaiY);
	glVertex2f(-325 + kunaiX, 5 + kunaiY);

	glVertex2f(-315 + kunaiX, -5 + kunaiY);
	glVertex2f(-315 + kunaiX, 5 + kunaiY);

	glVertex2f(-305 + kunaiX, -5 + kunaiY);
	glVertex2f(-305 + kunaiX, 5 + kunaiY);
	glEnd();

	//kunai top
	glColor3ub(91, 91, 91);
	glBegin(GL_QUADS);
	glVertex2f(-300 + kunaiX, 0 + kunaiY);
	glVertex2f(-280 + kunaiX, 10 + kunaiY);
	glVertex2f(-250 + kunaiX, 0 + kunaiY);
	glVertex2f(-280 + kunaiX, -10 + kunaiY);
	glEnd();

	glColor3f(0.5, 0.5, 0.5);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(-300 + kunaiX, 0 + kunaiY);
	glVertex2f(-250 + kunaiX, 0 + kunaiY);

	glVertex2f(-280 + kunaiX, 10 + kunaiY);
	glVertex2f(-280 + kunaiX, -10 + kunaiY);
	glEnd();

	//kunai bottom
	glColor3ub(91, 91, 91);
	circle(-340 + kunaiX, 0 + kunaiY, 10);
	glColor3f(0, 0, 0);
	circle(-340 + kunaiX, 0 + kunaiY, 5);

}

void scoreboard() {
	glColor3f(0, 0, 0);
	glRectf(-400, -300, 400, -225);
}

void devil(int devilX, int devilY) {
	//corns
	//left corn
	glColor3f(0, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(devilX, devilY);
	glVertex2f(devilX + 20, devilY - 5);
	glVertex2f(devilX + 10, devilY - 10);

	//right corn
	glVertex2f(devilX + 50, devilY);
	glVertex2f(devilX + 30, devilY - 5);
	glVertex2f(devilX + 40, devilY - 10);
	glEnd();

	//face
	glColor3ub(252, 0, 0);
	circle(devilX + 25, devilY - 20, 20);

	//eyes
	glColor3f(0, 0, 0);
	glLineWidth(2);
	glBegin(GL_LINES);
	//left eyeborw
	glVertex2f(devilX + 22, devilY - 20);
	glVertex2f(devilX + 12, devilY - 10);
	//right eyebrow
	glVertex2f(devilX + 28, devilY - 20);
	glVertex2f(devilX + 38, devilY - 10);
	glEnd();

	//left eye
	circle(devilX + 17, devilY - 20, 3);
	//right eye
	circle(devilX + 33, devilY - 20, 3);
}

//
// To display onto window using OpenGL commands
//

void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	displayBackground();
	displayTime();
	scoreboard();
	ninja();

	if (!shoot)
		kunaiY = ninjaY;
	if (shoot)
		kunai();

	
	for (int i = 0; i < 5; i++) {
		d[i].x = rand() % (351 - 50) + 50;
		d[i].y = 180 + i * 70;
		devil(d[i].x, d[i].y);
	}



	glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (shoot == false && key == ' ')
		shoot = true;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_F1:
		if (state == FIN) {
			ninjaY = 0;
			kunaiX = kunaiY = 0;
			state = RUN;
			sec2 = 2;
			activeTimer = true;
		}
		else if (state == RUN) {
			state = STOP;
			activeTimer = false;
		}
		else if (state == STOP) {
			state = RUN;
			activeTimer = true;
		}
		break;
	case GLUT_KEY_UP:
		if (ninjaY < 155 && activeTimer)
			ninjaY += 10;
		break;
	case GLUT_KEY_DOWN:
		if (ninjaY > -200 && activeTimer)
			ninjaY -= 10;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (activeTimer) {
		//time
		if (sec1 == 0) {
			if (sec2 != 0) {
				sec1 = 10;
				sec2--;
			}
		}
		if (msec2 == 0) {
			if (msec1 == 0 && (sec1 != 0 || sec2 != 0)) {
				msec2 = 10;
				sec1--;
			}
		}
		if (msec1 == 0) {
			if (msec2 != 0 || sec1 != 0 || sec2 != 0) {
				msec1 = 10;
				msec2--;
			}
		}
		if (sec2 == 0 && sec1 == 0 && msec2 == 0 && msec1 == 0) {
			state = FIN;
		}
		else 
			msec1--;

		//shuriken motion
		if (shoot && state == RUN) {
			if (kunaiX < 800)
				kunaiX += 8;
			else
				shoot = false;
		}
		else
			kunaiX = 0;

		//devils
		if (state == RUN) {

			for (int i = 0; i < 5; i++) {
				if (d[i].y == -175) {
					d[i].x = rand() % (351 - 50) + 50;
					d[i].y = 180;
				}
				d[i].y--;
			}

		}
		
	}



	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Template File");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif
	srand(time(NULL));
	Init();

	glutMainLoop();
}