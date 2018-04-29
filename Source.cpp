/*********
CTIS164 - Template Source Program
----------
STUDENT : Ping Cheng
SECTION : 2
HOMEWORK: 4
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
#include "vec.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool activeTimer = true;

typedef struct {
	double r, g, b;
} color_t;

typedef struct {
	vec_t   pos;
	color_t color;
	vec_t   vel;
	bool bright;
} light_t;

typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

typedef struct {
	color_t color;
	float angle;
	float speed;
	bool direction;
	float radius;
} planet_t;

#define NUM 3

light_t light[4] =
{
	{ { 0, 0 },{ 1, 0, 0 }},
	{ { 200, 0 },{ 0, 1, 0 }},
	{ { -200, 0 },{ 0, 0, 1 }}
};

light_t sun;

planet_t planet[3];

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
	return (-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) * distanceImpact(magV(L));
	return mulColor(factor, source.color);
}

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

void drawSun() {
	glColor3f(sun.color.r, sun.color.g, sun.color.b);
	circle(0, 0, 30);
}

void drawPlanet(planet_t t, float radius) {
	glColor3ub(t.color.r, t.color.g, t.color.b);
	circle(radius * cos(t.angle * D2R), radius * sin(t.angle * D2R), t.radius);

	if (t.angle < 0)
		t.angle += 360;
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

	glColor3f(1, 1, 1);
	vprint(-400, 380, GLUT_BITMAP_9_BY_15, "Homeword #4");
	vprint(-400, 360, GLUT_BITMAP_9_BY_15, "by Ping Cheng");

	drawSun();
	

	// light source 
	for (int i = 0; i < NUM; i++) {
		glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
		circle(light[i].pos.x, light[i].pos.y, 10);
	}
	
	//planets
	float radius = 250;
	for (int i = 0; i < 3; i++) {
		float angle;
		glBegin(GL_TRIANGLE_FAN);
		glColor3f(0.3, 0.3, 0.3);
		glVertex2f(radius * cos(planet[i].angle * D2R), radius * sin(planet[i].angle * D2R));
		for (int j = 0; j < 100; j++)
		{
			angle = 2 * PI*j / 100;

			vertex_t P = { { radius * cos(planet[i].angle * D2R) + planet[i].radius*cos(angle), radius * sin(planet[i].angle * D2R) + planet[i].radius*sin(angle) },{ 0, 1 } };
			color_t res = { 0, 0, 0 };
			for (int k = 0; k < NUM; k++) {
				res = addColor(res, calculateColor(light[k], P));
			}
			res = addColor(res, calculateColor(sun, P));
			glColor3f(res.r, res.g, res.b);
			glVertex2f(radius * cos(planet[i].angle * D2R) + planet[i].radius*cos(angle), radius * sin(planet[i].angle * D2R) + planet[i].radius*sin(angle));
		}
		radius += 50;
		glEnd();
	}
	
	glBegin(GL_TRIANGLE_FAN);

	glEnd();

	glutSwapBuffers();

}

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sun.color = { 1, 1, 0 };
	sun.bright = true;

	light[0] = { { 0, 0 },{ 1, 0, 0 } };
	light[1] = { { 200, 0 },{ 0, 1, 0 } };
	light[2] = { { -200, 0 },{ 0, 0, 1 } };

	for (int i = 0; i < NUM; i++) {
		light[i].vel = { (double)(rand() % 21) - 10 , (double)(rand() % 21) - 10 };
		light[i].bright = true;
	}

	for (int i = 0; i < 3; i++) {
		planet[i].color = { 150 , 150 , 150 };
		planet[i].direction = rand() & 1;
		planet[i].angle = rand() % 361;
		planet[i].speed = rand() % 2 + 1;
		planet[i].radius = rand() % 6 + 20;
	}
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
	case GLUT_KEY_F1:
		if (light[0].bright) {
			light[0].color = { 0.3, 0.3, 0.3 };
			light[0].bright = false;
		}
		else {
			light[0].color = { 1, 0, 0 };
			light[0].bright = true;
		}
		break;
	case GLUT_KEY_F2:
		if (light[1].bright) {
			light[1].color = { 0.3, 0.3, 0.3 };
			light[1].bright = false;
		}
		else {
			light[1].color = { 0, 1, 0 };
			light[1].bright = true;
		}
		break;
	case GLUT_KEY_F3:
		if (light[2].bright) {
			light[2].color = { 0.3, 0.3, 0.3 };
			light[2].bright = false;
		}
		else {
			light[2].color = { 0, 0, 1 };
			light[2].bright = true;
		}
		break;
	case GLUT_KEY_F4: 
		if (sun.bright) {
			sun.color = { 0.3, 0.3, 0.3 };
			sun.bright = false;
		}
		else {
			sun.color = { 1, 1, 0 };
			sun.bright = true;
		}
		break;
	case GLUT_KEY_F5:
		activeTimer == true ? activeTimer = false : activeTimer = true;
		break;
	case GLUT_KEY_F6:
		Init();
		break;
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
		for (int i = 0; i < NUM; i++) {
			light[i].pos = addV(light[i].pos, light[i].vel);

			// Reflection from Walls.
			if (light[i].pos.x > 395 || light[i].pos.x < -395) {
				light[i].vel.x *= -1;
			}

			if (light[i].pos.y > 395 || light[i].pos.y < -395) {
				light[i].vel.y *= -1;
			}
		}

		for (int i = 0; i < 3; i++) {
			//clockwise or counter clockwise
			if (planet[i].direction)
				planet[i].angle += planet[i].speed;
			else
				planet[i].angle -= planet[i].speed;

			if (planet[i].angle > 360)
				planet[i].angle -= 360;
			else if (planet[i].angle < -360)
				planet[i].angle += 360;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif



void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Homework #4");

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