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
-Timer
-Best record
-Stage system
-Harder after each stage (objects get more likely to move faster)
-Aiming dotted line animation
-As player presses mouse longer fire will be faster
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

enum stat {MENU, START, RUN, SCOREBOARD};
enum opt {OUT, PLAY, SCORE, EXIT};

//state
int state = MENU;
int opt = OUT;

//scoreboard
int gameplay = 0;

//stage level
int stage = 1;

//timer
typedef struct {
	int min1 = 0, min2 = 0, sec1 = 0, sec2 = 0, msec1 = 0, msec2 = 0;
} timer_t;

timer_t timer[100];
//int min1 = 0, min2 = 0, sec1 = 0, sec2 = 0, msec1 = 0, msec2 = 0;

//dotted lines animation
int lock = 0;

//loading animation
int fireloader = 0;

//mouse click
bool pressed = false;

//power that increases when user holds on clicking
float strength = 10;

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
	color_t color;
	float angle;
	float speed;
	bool direction;
	bool hit = false;
	float radius;
} target_t;

//for shining animation
typedef struct {
	int r, g, b;
} maxpow_t;

player_t pl;
fire_t fr;
target_t target[3];
maxpow_t maxpow = { 0, 0, 0 };

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

	glColor3ub(212, 0, 255);

	//Name info
	vprint(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_9_BY_15, "Homework #3");
	vprint(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 40, GLUT_BITMAP_9_BY_15, "by Ping Cheng");

	//white
	glColor3f(1, 1, 1);

	//draw orbits
	circle_wire(0, 0, 250);
	circle_wire(0, 0, 300);
	circle_wire(0, 0, 350);

	//x y axis
	glBegin(GL_LINES);
	glVertex2f(-WINDOW_WIDTH / 2, 0);
	glVertex2f(WINDOW_WIDTH / 2, 0);

	glVertex2f(0, WINDOW_HEIGHT);
	glVertex2f(0, -WINDOW_HEIGHT);
	glEnd();

	//stage number
	glColor3f(0, 0, 0);
	glRectf(-40, 390, 40, 360);
	glColor3ub(212, 0, 255);
	if (stage < 11)
		vprint(-35, 370, GLUT_BITMAP_HELVETICA_18, "STAGE %d", stage);

	if (state == START) {
		glColor3f(0, 0, 0);
		glRectf(-50, -150, 50, -100);
		glColor3f(1, 1, 0);
		if (stage < 11)
			vprint(-60, -130, GLUT_BITMAP_9_BY_15, "CLICK TO START");
		else
			vprint(-60, -130, GLUT_BITMAP_9_BY_15, "CLICK TO RETURN");
	}

	//time
	glColor3f(0, 1, 0);
	vprint(340, 380, GLUT_BITMAP_9_BY_15, "TIME");
	vprint(320, 360, GLUT_BITMAP_9_BY_15, "%d%d:%d%d:%d%d", timer[gameplay].min2, timer[gameplay].min1, timer[gameplay].sec2, timer[gameplay].sec1, timer[gameplay].msec2, timer[gameplay].msec1);
}

void object(target_t t, float radius) {
	glColor3ub(t.color.r, t.color.g, t.color.b);
	circle(radius * cos(t.angle * D2R), radius * sin(t.angle * D2R), t.radius);
	glColor3f(1, 1, 1);
	if (t.angle < 0)
		t.angle += 360;
	glColor3f(0, 0, 0);
	vprint(-10 + (radius)* cos(t.angle * D2R), -10 + (radius)* sin(t.angle * D2R), GLUT_BITMAP_9_BY_15, "%.0f", t.angle);
}

void player(player_t pl) {
	glColor3ub(255, 123, 0);
	glLineWidth(6);

	glBegin(GL_LINE_LOOP);
	glVertex2f(-80 * cos((pl.angle - 45) * D2R), -80 * sin((pl.angle - 45) * D2R));
	glVertex2f(-50 * cos((pl.angle - 45) * D2R), -50 * sin((pl.angle - 45) * D2R));
	glVertex2f(30 * cos((pl.angle + 90) * D2R), 30 * sin((pl.angle + 90) * D2R));
	glVertex2f(50 * cos(pl.angle * D2R), 50 * sin(pl.angle * D2R));
	glVertex2f(30 * cos((pl.angle - 90) * D2R), 30 * sin((pl.angle - 90) * D2R));
	glVertex2f(-50 * cos((pl.angle + 45) * D2R), -50 * sin((pl.angle + 45) * D2R));
	glVertex2f(-80 * cos((pl.angle + 45) * D2R), -80 * sin((pl.angle + 45) * D2R));

	glVertex2f(-55 * cos((pl.angle + 45) * D2R), -55 * sin((pl.angle + 45) * D2R));
	glVertex2f(25 * cos((pl.angle - 90) * D2R), 25 * sin((pl.angle - 90) * D2R));
	glVertex2f(45 * cos(pl.angle * D2R), 45 * sin(pl.angle * D2R));
	glVertex2f(25 * cos((pl.angle + 90) * D2R), 25 * sin((pl.angle + 90) * D2R));
	glVertex2f(-55 * cos((pl.angle - 45) * D2R), -55 * sin((pl.angle - 45) * D2R));
	glVertex2f(-85 * cos((pl.angle - 45) * D2R), -85 * sin((pl.angle - 45) * D2R));
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex2f(-50 * cos((pl.angle - 45) * D2R), -50 * sin((pl.angle - 45) * D2R));
	glVertex2f(-50 * cos((pl.angle + 45) * D2R), -50 * sin((pl.angle + 45) * D2R));
	glEnd();

	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3ub(135, 135, 135);
	glVertex2f(-50 * cos((pl.angle - 45) * D2R), -50 * sin((pl.angle - 45) * D2R));
	glVertex2f(-50 * cos((pl.angle + 45) * D2R), -50 * sin((pl.angle + 45) * D2R));
	glEnd();

	//aiming dotted lines
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	if (lock < 5) {
		glVertex2f(50 * cos(pl.angle * D2R), 50 * sin(pl.angle * D2R));
		glVertex2f(100 * cos(pl.angle * D2R), 100 * sin(pl.angle * D2R));
	}
	else if (lock < 10) {
		glVertex2f(100 * cos(pl.angle * D2R), 100 * sin(pl.angle * D2R));
		glVertex2f(150 * cos(pl.angle * D2R), 150 * sin(pl.angle * D2R));
	}
	else if (lock < 15) {
		glVertex2f(150 * cos(pl.angle * D2R), 150 * sin(pl.angle * D2R));
		glVertex2f(200 * cos(pl.angle * D2R), 200 * sin(pl.angle * D2R));
	}
	else if (lock < 20) {
		glVertex2f(200 * cos(pl.angle * D2R), 200 * sin(pl.angle * D2R));
		glVertex2f(250 * cos(pl.angle * D2R), 250 * sin(pl.angle * D2R));
	}
	else if (lock < 25) {
		glVertex2f(250 * cos(pl.angle * D2R), 250 * sin(pl.angle * D2R));
		glVertex2f(300 * cos(pl.angle * D2R), 300 * sin(pl.angle * D2R));
	}
	else if (lock < 30) {
		glVertex2f(300 * cos(pl.angle * D2R), 300 * sin(pl.angle * D2R));
		glVertex2f(350 * cos(pl.angle * D2R), 350 * sin(pl.angle * D2R));
	}
	else if (lock < 35) {
		glVertex2f(350 * cos(pl.angle * D2R), 350 * sin(pl.angle * D2R));
		glVertex2f(400 * cos(pl.angle * D2R), 400 * sin(pl.angle * D2R));
	}
	else if (lock < 40) {
		glVertex2f(400 * cos(pl.angle * D2R), 400 * sin(pl.angle * D2R));
		glVertex2f(450 * cos(pl.angle * D2R), 450 * sin(pl.angle * D2R));
	}
	else if (lock < 45) {
		glVertex2f(450 * cos(pl.angle * D2R), 450 * sin(pl.angle * D2R));
		glVertex2f(500 * cos(pl.angle * D2R), 500 * sin(pl.angle * D2R));
	}
	else {
		glVertex2f(500 * cos(pl.angle * D2R), 500 * sin(pl.angle * D2R));
		glVertex2f(550 * cos(pl.angle * D2R), 550 * sin(pl.angle * D2R));
	}
	glEnd();

	glLineWidth(1);
}

void fire() {
	if (fr.active) {
		glColor3ub(39, 232, 225);
		circle(fr.pos.x, fr.pos.y, 10);
		glColor3ub(206, 237, 236);
		circle(fr.pos.x, fr.pos.y, 5);

		glColor3f(0, 0, 0);
		glRectf(-50, 160, 50, 130);

		glColor3f(0, 1, 0);
		glRectf(-120, 120, -120 + fireloader, 100);
		vprint(-40, 140, GLUT_BITMAP_9_BY_15, "Reloading...");
	}
	else {
		if (pressed) {
			if (strength > 20) {
				//while pressing keep the color as red
				if (strength * 2 < 200)
					glColor3f(1, 1, 0);
				//color shines when it reaches max
				else
					glColor3ub(maxpow.r, maxpow.g, maxpow.b);

				glRectf(-100, -120, -100 + strength * 2, -100);
			}
		}
	}
}

bool testCollision(fire_t fr, target_t t, float radius) {
	float dx = radius * cos(t.angle * D2R) - fr.pos.x;
	float dy = radius * sin(t.angle * D2R) - fr.pos.y;
	float d = sqrt(dx*dx + dy*dy);
	return d <= (t.radius);
}

void swap(timer_t *x, timer_t *y) {
	timer_t temp = *x;
	*x = *y;
	*y = temp;
}

void bubbleSort(timer_t timer[], int n) {
	int k, pass = 1, sort;
	int num1, num2;
	
	do {
		sort = 1;
		for (k = 0; k < n - pass; k++) {
			num1 = timer[k].min2 * 60000 + timer[k].min1 * 6000 + timer[k].sec2 * 1000 + timer[k].sec1 * 100 + timer[k].msec2 * 10 + timer[k].msec1;
			num2 = timer[k + 1].min2 * 60000 + timer[k + 1].min1 * 6000 + timer[k + 1].sec2 * 1000 + timer[k + 1].sec1 * 100 + timer[k + 1].msec2 * 10 + timer[k + 1].msec1;
			if (num1 > num2) {
				swap(&timer[k], &timer[k + 1]);
				sort = 0;
			}
		}
		pass++;
	} while (!sort);
}

void displayScoreBoard() {
	glColor3f(0, 1, 0);
	if (gameplay == 0)
		vprint(-110, 0, GLUT_BITMAP_TIMES_ROMAN_24, "You haven't played yet!");
	else {
		bubbleSort(timer, gameplay);
		for (int i = 0; i < gameplay; i++) {
			vprint(-300, 350 - i * 50, GLUT_BITMAP_TIMES_ROMAN_24, "%d. %d%d:%d%d:%d%d", i + 1, timer[i].min2, timer[i].min1, timer[i].sec2, timer[i].sec1, timer[i].msec2, timer[i].msec1);
		}
	}
	vprint(-100, -200, GLUT_BITMAP_TIMES_ROMAN_24, "Right Click to return");
}

void displayMenu() {
	glColor3ub(0, 93, 255);
	vprint2(-225, 250, 0.5, "Homework #3");

	if (opt == PLAY) {
		glColor3ub(0, 93, 255);
		vprint(-50, 50, GLUT_BITMAP_TIMES_ROMAN_24, "START");
	}
	else {
		glColor3f(1, 1, 1);
		vprint(-50, 50, GLUT_BITMAP_TIMES_ROMAN_24, "START");
	}
	
	if (opt == SCORE) {
		glColor3ub(0, 93, 255);
		vprint(-50, 0, GLUT_BITMAP_TIMES_ROMAN_24, "SCORE");
	}
	else {
		glColor3f(1, 1, 1);
		vprint(-50, 0, GLUT_BITMAP_TIMES_ROMAN_24, "SCORE");
	}

	if (opt == EXIT) {
		glColor3ub(0, 93, 255);
		vprint(-40, -50, GLUT_BITMAP_TIMES_ROMAN_24, "EXIT");
	}
	else {
		glColor3f(1, 1, 1);
		vprint(-40, -50, GLUT_BITMAP_TIMES_ROMAN_24, "EXIT");
	}
	
	
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
	
	if (state == MENU)
		displayMenu();
	else if (state == SCOREBOARD)
		displayScoreBoard();
	else {
		displayBackground();
		//angle next to orbit
		vprint(370 * cos(pl.angle * D2R), 370 * sin(pl.angle * D2R), GLUT_BITMAP_9_BY_15, "%.0f", pl.angle);

		player(pl);
		fire();
	}

	if (state == RUN && stage != 11) {
		
		float radius = 250;
		for (int i = 0; i < 3; i++) {
			if (testCollision(fr, target[i], radius)) {
				target[i].hit = true;
			}
			if (!target[i].hit)
				object(target[i], radius);
			radius += 50;
		}

		//initialize
		if (target[0].hit && target[1].hit && target[2].hit) {
			fireloader = 0;
			fr.active = false;
			strength = 10;
			fr.pos.x = 0;
			fr.pos.y = 0;
			for (int i = 0; i < 3; i++) {
				target[i].color.r = rand() % 256;
				target[i].color.g = rand() % 256;
				target[i].color.b = rand() % 256;
				target[i].direction = rand() & 1;
				target[i].angle = rand() % 361;
				//speed gets faster as after every stage
				target[i].speed = rand() % (1 + stage) + 1;
				target[i].hit = false;
				target[i].radius = rand() % 11 + 20;
			}

			//add stage after completing each time
			stage++;
			state = START;
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
	if (state == MENU) {
		if (opt == PLAY && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
			state = START;
		else if (opt == SCORE && button == GLUT_LEFT_BUTTON && stat == GLUT_UP)
			state = SCOREBOARD;
		else if (opt == EXIT && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
			exit(0);
	}

	if (state == SCOREBOARD && button == GLUT_RIGHT_BUTTON && stat == GLUT_DOWN)
		state = MENU;

	if (state == START && button == GLUT_LEFT_BUTTON && stat == GLUT_UP) {
		if (stage == 11) {
			state = MENU;
			gameplay++;
			
			//initialize
			stage = 0;
			fireloader = 0;
			fr.active = false;
			strength = 10;
			fr.pos.x = 0;
			fr.pos.y = 0;
			for (int i = 0; i < 3; i++) {
				target[i].color.r = rand() % 256;
				target[i].color.g = rand() % 256;
				target[i].color.b = rand() % 256;
				target[i].direction = rand() & 1;
				target[i].angle = rand() % 361;
				//speed gets faster as after every stage
				target[i].speed = rand() % (1 + stage) + 1;
				target[i].hit = false;
				target[i].radius = rand() % 11 + 20;
			}
		}
		else
			state = RUN;
	}
	//increase speed while holding
	else if (state == RUN && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		pressed = true;
	}
	//shoot when release
	else if (state == RUN && button == GLUT_LEFT_BUTTON && stat == GLUT_UP) {
		pressed = false;
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

int getMouseAngle(int x, int y) {
	
	int angle = (atan2f(y, x)) * (180 / PI);
	if (angle < 0)
		angle += 360;

	return angle;
}

void onMoveDown(int x, int y) {
	// Write your codes here.
	int x2 = x - winWidth / 2;
	int y2 = winHeight / 2 - y;
	pl.angle = getMouseAngle(x2, y2);


	// to refresh the window it calls display() function   
	glutPostRedisplay();
}


// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	int x2 = x - winWidth / 2;
	int y2 = winHeight / 2 - y;

	if (state == MENU) {
		if (x2 >= -50 && x2 <= 50 && y2 >= 50 && y2 <= 75)
			opt = PLAY;
		else if (x2 >= -50 && x2 <= 50 && y2 >= 0 && y2 <= 25)
			opt = SCORE;
		else if (x2 >= -40 && x2 <= 40 && y2 >= -50 && y2 <= -25)
			opt = EXIT;
		else
			opt = OUT;
	}
	pl.angle = getMouseAngle(x2, y2);
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	if (pressed) {
		if (strength < 100)
			strength += 1;
		else {
			//maxpower color animation
			maxpow.r = rand() % 256;
			maxpow.g = rand() % 256;
			maxpow.b = rand() % 256;
		}
	}

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (state == RUN) {
		//aiming animation
		lock++;
		if (lock == 50)
			lock = 0;

		for (int i = 0; i < 3; i++) {
			//clockwise or counter clockwise
			if (target[i].direction)
				target[i].angle += target[i].speed;
			else
				target[i].angle -= target[i].speed;

			if (target[i].angle > 360)
				target[i].angle -= 360;
			else if (target[i].angle < -360)
				target[i].angle += 360;
		}

		if (fr.active) {
			fr.pos.x += strength * cos(fr.angle * D2R);
			fr.pos.y += strength * sin(fr.angle * D2R);
			fireloader += 5;

			//when fire reaches the border
			if (fr.pos.x > 400 || fr.pos.x < -400 || fr.pos.y > 400 || fr.pos.y < -400) {
				fr.active = false;
				fireloader = 0;
				strength = 10;
			}
		}

		//time
		if (timer[gameplay].msec1 == 9) {
			timer[gameplay].msec2++;
			timer[gameplay].msec1 = 0;
			if (timer[gameplay].msec2 == 10) {
				timer[gameplay].sec1++;
				timer[gameplay].msec2 = 0;
			}
		}
		if (timer[gameplay].sec1 == 10) {
			timer[gameplay].sec1 = 0;
			timer[gameplay].sec2++;
			if (timer[gameplay].sec2 == 6) {
				timer[gameplay].min1++;
				timer[gameplay].sec2 = 0;
			}
		}
		timer[gameplay].msec1++;
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
		target[i].color.r = rand() % 256;
		target[i].color.g = rand() % 256;
		target[i].color.b = rand() % 256;
		target[i].direction = rand() & 1;
		target[i].angle = rand() % 361;
		target[i].speed = rand() % 2 + 1;
		target[i].radius = rand() % 11 + 20;
	}

}

void main(int argc, char *argv[]) {
	srand(time(NULL));
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

	glutMainLoop();
}