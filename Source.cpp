/*********
CTIS164 - Template Source Program
----------
STUDENT :
SECTION :
HOMEWORK:
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

#define WINDOW_WIDTH  400
#define WINDOW_HEIGHT 400

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_ON         0 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
int user[4];
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

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

//
// To display onto window using OpenGL commands
//
void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.2, 0.2, 0.2);
	glPointSize(20);

	//grid lines
	glBegin(GL_LINES);
	for (int i = 0; i < 400; i += 20) {
		glVertex2f(-200, 200 - i);
		glVertex2f(200, 200 - i);
		glVertex2f(200 - i, -200);
		glVertex2f(200 - i, 200);
	}

	glEnd();

	//top oval
	glColor3f(0.8, 0.8, 0);
	circle(-120, 160, 20);
	circle(120, 160, 20);

	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0);
	glVertex2f(-120, 180);
	glVertex2f(-120, 140);
	glVertex2f(120, 140);
	glVertex2f(120, 180);
	glEnd();

	//text
	glColor3f(1, 0.4, 0);
	print(-120, 150, "Lab #2 Class Work, Fall 2017", GLUT_BITMAP_HELVETICA_18);
	glColor3f(0, 0.8, 0);
	print(-200, 100, "Import", GLUT_BITMAP_HELVETICA_18);
	print(125, -160, "Months", GLUT_BITMAP_HELVETICA_18);

	//graph lines
	glBegin(GL_LINES);
	glColor3f(0.8, 0.8, 0);
	glVertex2f(-140, 120);
	glVertex2f(-140, -140);
	glVertex2f(-140, -140);
	glVertex2f(160, -140);
	glEnd();

	//graph arrow
	glBegin(GL_TRIANGLES);
	glColor3f(0.8, 0, 0);
	glVertex2f(-140, 120);
	glVertex2f(-145, 115);
	glVertex2f(-135, 115);

	glVertex2f(160, -140);
	glVertex2f(155, -135);
	glVertex2f(155, -145);
	glEnd();

	//user value lines
	glBegin(GL_LINES);
	glColor3f(0.8, 0.8, 0.8);

	for (int i = 0; i < 4; i++) {
		glVertex2f(-145, -140 + user[i] * 2);
		glVertex2f(-135, -140 + user[i] * 2);
	}
	glEnd();
	vprint(-160, -145 + user[0] * 2, GLUT_BITMAP_8_BY_13, "%d", user[0]);
	vprint(-160, -145 + user[1] * 2, GLUT_BITMAP_8_BY_13, "%d", user[1]);
	vprint(-160, -145 + user[2] * 2, GLUT_BITMAP_8_BY_13, "%d", user[2]);
	vprint(-160, -145 + user[3] * 2, GLUT_BITMAP_8_BY_13, "%d", user[3]);

	//value in rectangles
	glBegin(GL_QUADS);
	int space = 80;
	for (int i = 0; i < 4; i++) {
		glColor3f(0.8, 0 + i * 0.2, 0.8);
		glVertex2f(-120 + space * i, -140);
		glVertex2f(-120 + space * i, -140 + user[i] * 2);
		glVertex2f(-100 + space * i, -140 + user[i] * 2);
		glVertex2f(-100 + space * i, -140);
	}
	glEnd();

	//Months
	glColor3f(0.8, 0.8, 0.8);
	print(-120 + space * 0, -130 + user[0] * 2, "January", GLUT_BITMAP_HELVETICA_10);
	print(-120 + space * 1, -130 + user[1] * 2, "February", GLUT_BITMAP_HELVETICA_10);
	print(-120 + space * 2, -130 + user[2] * 2, "March", GLUT_BITMAP_HELVETICA_10);
	print(-120 + space * 3, -130 + user[3] * 2, "April", GLUT_BITMAP_HELVETICA_10);

	glColor3f(0.2, 0.2, 0.7);
	print(-60, -190, "BAR CHART", GLUT_BITMAP_TIMES_ROMAN_24);

	glutSwapBuffers();

}

void f2display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.2, 0.2, 0.2);
	glPointSize(20);

	//grid lines
	glBegin(GL_LINES);
	for (int i = 0; i < 400; i += 20) {
		glVertex2f(-200, 200 - i);
		glVertex2f(200, 200 - i);
		glVertex2f(200 - i, -200);
		glVertex2f(200 - i, 200);
	}

	glEnd();

	//top oval
	glColor3f(0.8, 0.8, 0);
	circle(-120, 160, 20);
	circle(120, 160, 20);

	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0);
	glVertex2f(-120, 180);
	glVertex2f(-120, 140);
	glVertex2f(120, 140);
	glVertex2f(120, 180);
	glEnd();

	//text
	glColor3f(1, 0.4, 0);
	print(-120, 150, "Lab #2 Class Work, Fall 2017", GLUT_BITMAP_HELVETICA_18);
	glColor3f(0, 0.8, 0);
	print(-200, 100, "Import", GLUT_BITMAP_HELVETICA_18);
	print(125, -160, "Months", GLUT_BITMAP_HELVETICA_18);

	//graph lines
	glBegin(GL_LINES);
	glColor3f(0.8, 0.8, 0);
	glVertex2f(-140, 120);
	glVertex2f(-140, -140);
	glVertex2f(-140, -140);
	glVertex2f(160, -140);
	glEnd();

	//graph arrow
	glBegin(GL_TRIANGLES);
	glColor3f(0.8, 0, 0);
	glVertex2f(-140, 120);
	glVertex2f(-145, 115);
	glVertex2f(-135, 115);

	glVertex2f(160, -140);
	glVertex2f(155, -135);
	glVertex2f(155, -145);
	glEnd();

	//user value lines
	glBegin(GL_LINES);
	glColor3f(0.8, 0.8, 0.8);

	for (int i = 0; i < 4; i++) {
		glVertex2f(-145, -140 + user[i] * 2);
		glVertex2f(-135, -140 + user[i] * 2);
	}
	glEnd();
	vprint(-160, -145 + user[0] * 2, GLUT_BITMAP_8_BY_13, "%d", user[0]);
	vprint(-160, -145 + user[1] * 2, GLUT_BITMAP_8_BY_13, "%d", user[1]);
	vprint(-160, -145 + user[2] * 2, GLUT_BITMAP_8_BY_13, "%d", user[2]);
	vprint(-160, -145 + user[3] * 2, GLUT_BITMAP_8_BY_13, "%d", user[3]);

	//value in dots
	int space = 80;
	for (int i = 0; i < 4; i++) {
		glColor3f(0.8, 0 + i * 0.2, 0.8);
		circle(-120 + space * i, -140 + user[i] * 2, 5);
	}

	//line connecting dots
	glBegin(GL_LINE_STRIP);

	glColor3f(0.8, 0, 0);
	for (int i = 0; i < 4; i++) {
		glVertex2f(-120 + space * i, -140 + user[i] * 2);
	}
	glEnd();

	//Months
	glColor3f(0.8, 0.8, 0.8);
	print(-120 + space * 0, -130 + user[0] * 2, "January", GLUT_BITMAP_HELVETICA_10);
	print(-120 + space * 1, -130 + user[1] * 2, "February", GLUT_BITMAP_HELVETICA_10);
	print(-120 + space * 2, -130 + user[2] * 2, "March", GLUT_BITMAP_HELVETICA_10);
	print(-120 + space * 3, -130 + user[3] * 2, "April", GLUT_BITMAP_HELVETICA_10);

	glColor3f(0.2, 0.2, 0.7);
	print(-60, -190, "LINE CHART", GLUT_BITMAP_TIMES_ROMAN_24);

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
	case GLUT_KEY_F1: glutDisplayFunc(display); break;
	case GLUT_KEY_F2: glutDisplayFunc(lineChart); break;
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
	for (int i = 0; i < 4; i++) {
		printf("Enter user %d value (0 - 100): ", i + 1);
		scanf("%d", &user[i]);
		while (user[i] < 0 || user[i] > 100) {
			printf("Enter user %d value (0 - 100): ", i + 1);
			scanf("%d", &user[i]);
		}
	}

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

	Init();

	glutMainLoop();
}
