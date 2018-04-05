/*********
CTIS164 - Template Source Program
----------
STUDENT : Ping Cheng
SECTION : 2
HOMEWORK: 3
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define START 0
#define RUN 1

int state = START;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct {
	float x, y;
} point_t;

typedef struct {
	float angle;
} player_t;

typedef struct {
	point_t pos;
	float angle;
	bool active = false;
} fire_t;

typedef struct {
	int r, g, b;
} color_t;

typedef struct {
	point_t center;
	color_t color;
	float radius;
	float angle;
	float speed;
	bool direction;
	bool hit = false;
} target_t;

player_t pl;
fire_t fr;
int fire_rate = 0, direction=1;
target_t target[3];

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

	//white
	glColor3f(1, 1, 1);

	//draw orbits
	circle_wire(0, 0, 250);
	circle_wire(0, 0, 300);
	circle_wire(0, 0, 350);

	//Name info
	vprint(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_9_BY_15, "Homework #3");
	vprint(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 40, GLUT_BITMAP_9_BY_15, "by Ping Cheng");

	//x y axis
	glBegin(GL_LINES);
	glVertex2f(-WINDOW_WIDTH / 2, 0);
	glVertex2f(WINDOW_WIDTH / 2, 0);

	glVertex2f(0, WINDOW_HEIGHT);
	glVertex2f(0, -WINDOW_HEIGHT);
	glEnd();

	if (state == START) {
		glColor3f(0, 0, 0);
		glRectf(-50, -150, 50, -100);
		glColor3f(1, 1, 1);
		vprint(-60, -130, GLUT_BITMAP_9_BY_15, "CLICK TO START");
	}
}

void object(target_t t, float radius) {
	glColor3ub(t.color.r, t.color.g, t.color.b);
	circle(radius * cos(t.angle * D2R), radius * sin(t.angle * D2R), 20);
}

void player(player_t pl) {
	glColor3f(1, 1, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(50 * cos(pl.angle * D2R), 50 * sin(pl.angle * D2R));
	glEnd();
	glLineWidth(1);
}

void fire() {
	if (fr.active) {
		glColor3f(1, 1, 1);
		circle(fr.pos.x, fr.pos.y, 6);
	}
}

bool testCollision(fire_t fr, target_t t, float radius) {
	float dx = t.center.x - fr.pos.x;
	float dy = t.center.y - fr.pos.y;
	float d = sqrt(dx*dx + dy*dy);
	return d <= radius;
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
	vprint(370 * cos(pl.angle * D2R), 370 * sin(pl.angle * D2R), GLUT_BITMAP_9_BY_15, "%.0f", pl.angle);

	player(pl);
	fire();

	if (state == RUN) {
		float radius = 350;
		for (int i = 0; i < 3; i++) {
			if (testCollision(fr, target[i], radius))
				target[i].hit = true;
			if (!target[i].hit)
				object(target[i], radius);
			radius -= 50;
		}
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
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
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
	if (state == START && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		state = RUN;
	}
	else if (state == RUN && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		if (!fr.active) {
			fr.pos.x = 0;
			fr.pos.y = 0;
			fr.angle = pl.angle;
			fr.active = true;
		}
	}


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

int getMouseAngle(int x, int y) {
	int x2 = x - winWidth / 2;
	int y2 = winHeight / 2 - y;
	int angle = (atan2f(y2, x2)) * (180 / PI);
	if (angle < 0)
		angle += 360;

	return angle;
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	pl.angle = getMouseAngle(x, y);
	glColor3f(1, 1, 1);
	

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (state == RUN) {
		for (int i = 0; i < 3; i++) {
			if (target[i].direction) {
				target[i].angle += target[i].speed;
			}
			else
				target[i].angle -= target[i].speed;
			if (target[i].angle > 360)
				target[i].angle -= 360;
		}
		if (fr.active) {
			fr.pos.x += 10 * cos(fr.angle * D2R);
			fr.pos.y += 10 * sin(fr.angle * D2R);

			if (fr.pos.x > 400 || fr.pos.x < -400 || fr.pos.y > 400 || fr.pos.y < -400) {
				fr.active = false;
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

	for (int i = 0; i < 3; i++) {
		target[i].center.x = 0;
		target[i].center.y = 350;
		target[i].color.r = rand() % 256;
		target[i].color.g = rand() % 256;
		target[i].color.b = rand() % 256;
		target[i].direction = rand() & 1;
		target[i].angle = rand() % 361;
		target[i].speed = rand() % 2 + 1;
	}

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("HW3");

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

	Init();
	srand(time(NULL));
	glutMainLoop();
}