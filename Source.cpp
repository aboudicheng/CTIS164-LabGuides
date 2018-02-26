/*********
CTIS164 - Template Source Program
----------
STUDENT : Ping Cheng
SECTION : 2
HOMEWORK: 1
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
#define START 0
#define RUN 1
#define STOP 2
#define END 3

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

						  //
						  // to draw circle, center at (x,y)
						  // radius r
						  //
						  //variables for timer
int min1 = 0, min2 = 0, sec1 = 0, sec2 = 0, msec1 = 0, msec2 = 0;
int win;
int diff = 96;
//variables for competitors
int p1 = 1, p2 = 2, p3 = 3, p4 = 4, p5 = 5;
int winner;

//set default state as START
int state = START;

//variables for motion
int x[5] = { 0 };

int speed = 3;

void onTimer(int v);

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
	vprint(-300, 250, GLUT_BITMAP_9_BY_15, "TIME");
	vprint(260, 250, GLUT_BITMAP_9_BY_15, "WINNER");

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
	print(-150, 245, "Racing Animation by Ping Cheng", GLUT_BITMAP_9_BY_15);

	//Racing background
	glColor3ub(216, 17, 53);
	glRectf(-400, -300, 400, 180);

	//Lines
	glLineWidth(5);
	glColor3ub(255, 255, 255);
	glBegin(GL_LINES);
	for (int i = diff; i <= 4 * diff; i += diff) {
		glVertex2f(-400, 180 - i);
		glVertex2f(400, 180 - i);
	}
	glEnd();
}

void displayTime() {
	glColor3f(0, 1, 0);
	vprint(-320, 220, GLUT_BITMAP_9_BY_15, "%d%d:%d%d:%d%d", min2, min1, sec2, sec1, msec2, msec1);
}

void displayObj(int x[], int p) {
	//object 2
	if (x[p - 1] < 700) {

		//car's front & back
		circle(-380 + x[p - 1], 140 - ((p - 1) * diff), 20);
		circle(-320 + x[p - 1], 140 - ((p - 1) * diff), 20);

		glRectf(-380 + x[p - 1], 120 - ((p - 1) * diff), -320 + x[p - 1], 160 - ((p - 1) * diff));

		//player number
		glColor3f(1, 1, 1);
		vprint(-355 + x[p - 1], 135 - ((p - 1) * diff), GLUT_BITMAP_HELVETICA_12, "P%d", p);

		//tail
		glColor3f(0, 0, 0);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(-380 + x[p - 1], 160 - ((p - 1) * diff));
		glVertex2f(-380 + x[p - 1], 170 - ((p - 1) * diff));
		glEnd();

		circle(-385 + x[p - 1], 170 - ((p - 1) * diff), 5);
		circle(-375 + x[p - 1], 170 - ((p - 1) * diff), 5);

		//tail hole
		glColor3ub(216, 17, 53);
		circle(-385 + x[p - 1], 170 - ((p - 1) * diff), 2);
		circle(-375 + x[p - 1], 170 - ((p - 1) * diff), 2);

		//tires
		glColor3f(0.1, 0.1, 0.1);
		circle(-372 + x[p - 1], 115 - ((p - 1) * diff), 15);
		circle(-328 + x[p - 1], 115 - ((p - 1) * diff), 15);
		//center of tires
		glColor3f(1, 1, 1);
		circle(-372 + x[p - 1], 115 - ((p - 1) * diff), 5);
		circle(-328 + x[p - 1], 115 - ((p - 1) * diff), 5);

	}
	else if (x[p - 1] >= 1400) {
		//car's front & back
		circle(-380, 140 - ((p - 1) * diff), 20);
		circle(-320, 140 - ((p - 1) * diff), 20);

		glRectf(-380, 120 - ((p - 1) * diff), -320, 160 - ((p - 1) * diff));

		//player number
		glColor3f(1, 1, 1);
		vprint(-355, 135 - ((p - 1) * diff), GLUT_BITMAP_HELVETICA_12, "P%d", p);

		//tail
		glColor3f(0, 0, 0);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(-320, 160 - ((p - 1) * diff));
		glVertex2f(-320, 170 - ((p - 1) * diff));
		glEnd();

		circle(-325, 170 - ((p - 1) * diff), 5);
		circle(-315, 170 - ((p - 1) * diff), 5);

		//tail hole
		glColor3ub(216, 17, 53);
		circle(-325, 170 - ((p - 1) * diff), 2);
		circle(-315, 170 - ((p - 1) * diff), 2);

		//tires
		glColor3f(0.1, 0.1, 0.1);
		circle(-372, 115 - ((p - 1) * diff), 15);
		circle(-328, 115 - ((p - 1) * diff), 15);
		//center of tires
		glColor3f(1, 1, 1);
		circle(-372, 115 - ((p - 1) * diff), 5);
		circle(-328, 115 - ((p - 1) * diff), 5);
		if (state == RUN)
			state = END;
	}
	else if (x[p - 1] >= 700) {
		//car's front & back
		circle(380 - x[p - 1] + 700, 140 - ((p - 1) * diff), 20);
		circle(320 - x[p - 1] + 700, 140 - ((p - 1) * diff), 20);

		glRectf(380 - x[p - 1] + 700, 120 - ((p - 1) * diff), 320 - x[p - 1] + 700, 160 - ((p - 1) * diff));

		//player number
		glColor3f(1, 1, 1);
		vprint(345 - x[p - 1] + 700, 135 - ((p - 1) * diff), GLUT_BITMAP_HELVETICA_12, "P%d", p);

		//tail
		glColor3f(0, 0, 0);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(380 - x[p - 1] + 700, 160 - ((p - 1) * diff));
		glVertex2f(380 - x[p - 1] + 700, 170 - ((p - 1) * diff));
		glEnd();

		circle(385 - x[p - 1] + 700, 170 - ((p - 1) * diff), 5);
		circle(375 - x[p - 1] + 700, 170 - ((p - 1) * diff), 5);

		//tail hole
		glColor3ub(216, 17, 53);
		circle(385 - x[p - 1] + 700, 170 - ((p - 1) * diff), 2);
		circle(375 - x[p - 1] + 700, 170 - ((p - 1) * diff), 2);

		//tires
		glColor3f(0.1, 0.1, 0.1);
		circle(372 - x[p - 1] + 700, 115 - ((p - 1) * diff), 15);
		circle(328 - x[p - 1] + 700, 115 - ((p - 1) * diff), 15);
		//center of tires
		glColor3f(1, 1, 1);
		circle(372 - x[p - 1] + 700, 115 - ((p - 1) * diff), 5);
		circle(328 - x[p - 1] + 700, 115 - ((p - 1) * diff), 5);

	}
}


void displayWinner() {
	//make change on winner when it is ended
	if (state == END) {
		glColor3f(0, 1, 0);

		//make winner appear and disappear
		if (win < 50) {
			vprint(275, 220, GLUT_BITMAP_TIMES_ROMAN_24, "P%d", winner);
		}
	}
	//show winner during the race
	if (state != START && state != END) {
		if (x[0] > x[1] && x[0] > x[2] && x[0] > x[3] && x[0] > x[4])
			winner = p1;
		else if (x[1] > x[0] && x[1] > x[2] && x[1] > x[3] && x[1] > x[4])
			winner = p2;
		else if (x[2] > x[0] && x[2] > x[1] && x[2] > x[3] && x[2] > x[4])
			winner = p3;
		else if (x[3] > x[0] && x[3] > x[1] && x[3] > x[2] && x[3] > x[4])
			winner = p4;
		else if (x[4] > x[0] && x[4] > x[1] && x[4] > x[2] && x[4] > x[3])
			winner = p5;

		glColor3f(0, 1, 0);
		vprint(275, 220, GLUT_BITMAP_TIMES_ROMAN_24, "P%d", winner);
	}
}

void displayPress() {
	glColor3f(0, 1, 1);

	if (state == START)
		print(-160, 200, "Press <SpaceBar> to start animation", GLUT_BITMAP_9_BY_15);
	else if (state == RUN)
		print(-120, 200, "Press <SpaceBar> to stop", GLUT_BITMAP_9_BY_15);
	else if (state == STOP)
		print(-130, 200, "Press <SpaceBar> to continue", GLUT_BITMAP_9_BY_15);
	else if (state == END)
		print(-130, 200, "Press <F1> to start a new game", GLUT_BITMAP_9_BY_15);

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
	displayPress();

	glColor3ub(57, 148, 219);
	displayObj(x, p1);

	glColor3ub(57, 219, 75);
	displayObj(x, p2);

	glColor3ub(213, 219, 57);
	displayObj(x, p3);

	glColor3ub(217, 57, 219);
	displayObj(x, p4);

	glColor3ub(255, 93, 0);
	displayObj(x, p5);

	displayWinner();

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
	if (state == START && key == ' ')
		state = RUN;
	else if (state == RUN && key == ' ')
		state = STOP;
	else if (state == STOP && key == ' ')
		state = RUN;

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
void onSpecialKeyDown(int key, int z, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_F1: state = START; break;
	case GLUT_KEY_RIGHT: if (state == RUN) { for (int i = 0; i < 5; i++) { x[i] += 5; } } break;

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
	if (state == START) {
		min1 = min2 = sec1 = sec2 = msec1 = msec2 = 0;
		for (int i = 0; i < 5; i++)
			x[i] = 0;
	}
	if (state == RUN) {
		if (msec1 == 9) {
			msec2++;
			msec1 = 0;
			if (msec2 == 10) {
				sec1++;
				msec2 = 0;
			}
		}
		if (sec1 == 10) {
			sec1 = 0;
			sec2++;
			if (sec2 == 6) {
				min1++;
				sec2 = 0;
			}
		}
		msec1++;

		for (int i = 0; i < 5; i++) {
			x[i] += rand() % speed;
		}

	}

	if (state == END) {
		win++;
		if (win == 100)
			win = 0;
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
	glutCreateWindow("Ping Cheng - HW1");

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
