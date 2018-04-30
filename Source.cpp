/*********
CTIS164 - Template Source Program
----------
STUDENT : Ping Cheng
SECTION : 2
HOMEWORK: 4
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
- Press spacebar to add light sources that will come out from the center (maximum 20 in total)
- Use arrow keys to move the sun
- Planet recovers HP to 100, more light sources added => more points
- If light sources touch the sun then HP decreases
- Naming system
- Scoreboard using bubble sort
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
	bool hit;
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
	bool collision;
} planet_t;

typedef struct {
	char act[10] = "On";
} activation_t;

//used for changing between "On" and "Off"
activation_t act[5];

light_t light[20] =
{
	{ { 0, 0 },{ 1, 0, 0 } },
	{ { 200, 0 },{ 0, 1, 0 } },
	{ { -200, 0 },{ 0, 0, 1 } }
};

//# of sources
int num = 3,
//used to multiply score
factor = 1,
//health point set to 100
hp = 100,
//total amount of gameplay
gameplay = 0;

//current gameplay's score
double score = 0;

typedef struct {
	double score;
	char name[100] = "";
} score_t;

//each records the player data
score_t s[10];

//player name
char playerN[100] = "";

//used for showing popup window
bool gg = false, showscore = false;

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

double distanceImpact(double d) {
	return fabs((-1.0 / 350.0) * d + 1.0);
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

//used to test collision between light sources and sun
bool testLightCollision(light_t source, light_t sun) {
	float dx = sun.pos.x - source.pos.x;
	float dy = sun.pos.y - source.pos.y;
	float d = sqrt(dx*dx + dy*dy);
	return d <= 30;
}

//used to test collosion between planets and sun
bool testPlanetCollision(light_t sun, planet_t t, float radius) {
	float dx =  sun.pos.x - radius * cos(t.angle * D2R);
	float dy =  sun.pos.y - radius * sin(t.angle * D2R);
	float d = sqrt(dx*dx + dy*dy);
	return d <= t.radius * 2;
}

void drawSun() {
	glColor3f(sun.color.r, sun.color.g, sun.color.b);
	circle(sun.pos.x, sun.pos.y, 30);
}

void drawPlanet(planet_t planet, float radius) {
	float angle;
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.3, 0.3, 0.3);
	float x = radius * cos(planet.angle * D2R), y = radius * sin(planet.angle * D2R);
	glVertex2f(x, y);

	for (int j = 0; j <= 100; j++) {
		angle = 2 * PI*j / 100;

		vec_t v = { x + planet.radius*cos(angle), y + planet.radius*sin(angle) };
		vertex_t P = { { v.x, v.y } };

		P.N = unitV(subV({ v.x, v.y }, { x, y }));

		color_t res = { 0, 0, 0 };
		for (int k = 0; k < num; k++) {
			res = addColor(res, calculateColor(light[k], P));
		}
		res = addColor(res, calculateColor(sun, P));

		glColor3f(res.r, res.g, res.b);
		glVertex2f(v.x, v.y);
	}
	glEnd();

}

void swap(score_t *x, score_t *y) {
	score_t temp = *x;
	*x = *y;
	*y = temp;
}

void bubbleSort(score_t arr[], int n) {
	bool sort = true;
	for (int i = 0; i < n - 1; i++)
		if (arr[i].score < arr[i + 1].score) {
			swap(&arr[i], &arr[i + 1]);
			sort = false;
		}

	if (!sort)
		bubbleSort(arr, n - 1);
}

//gameover popup window
void GameOver() {
	glColor3ub(154, 247, 32);
	glRectf(-300, -200, 300, 200);

	glColor3ub(89, 32, 247);
	vprint(-70, 120, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");

	glColor3f(1, 1, 1);
	glRectf(-120, 40, 120, 60);

	glColor3ub(255, 131, 0);
	vprint(-120, 0, GLUT_BITMAP_HELVETICA_18, "Please type down your name");
	vprint(-115, 45, GLUT_BITMAP_HELVETICA_18, "%s", playerN);
	vprint(-110, -20, GLUT_BITMAP_HELVETICA_18, "Press <Enter> to continue");
}

//scoreboard popup window
void scoreBoard() {
	bubbleSort(s, gameplay);
	glColor3ub(154, 247, 32);
	glRectf(-300, -200, 300, 200);

	glColor3f(1, 0, 0);
	vprint(-290, 180, GLUT_BITMAP_9_BY_15, "Press F6 to Restart");

	glColor3ub(89, 32, 247);
	vprint(-70, 120, GLUT_BITMAP_TIMES_ROMAN_24, "SCOREBOARD");

	for (int i = 0; i < gameplay; i++) {
		if (i < 5)
			vprint(-200, 50 - i * 50, GLUT_BITMAP_TIMES_ROMAN_24, "%d: %s (%.0f)", i + 1, s[i].name, s[i].score);
		else
			vprint(50, 50 - i * 50 + 250, GLUT_BITMAP_TIMES_ROMAN_24, "%d: %s (%.0f)", i + 1, s[i].name, s[i].score);
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

	glColor3f(1, 1, 1);
	vprint(-400, 380, GLUT_BITMAP_9_BY_15, "Homework #4");
	vprint(-400, 360, GLUT_BITMAP_9_BY_15, "by Ping Cheng");

	glColor3f(0, 1, 0);
	vprint(-150, 380, GLUT_BITMAP_9_BY_15, "F1(Red): %s   F2(Green): %s   F3(Blue): %s", act[0].act, act[1].act, act[2].act);
	vprint(-150, 360, GLUT_BITMAP_9_BY_15, "F4(Sun): %s   F5(Animation): %s   F6:Restart", act[3].act, act[4].act);
	vprint(-400, 340, GLUT_BITMAP_9_BY_15, "# of light sources: %d", num);
	
	vprint(-400, 310, GLUT_BITMAP_9_BY_15, "HP:");
	glColor3f(1, 0, 0);
	glRectf(-350, 310, -350 + hp * 1.2, 320);
	vprint(-200, 310, GLUT_BITMAP_9_BY_15, "%d%%", hp);

	glColor3f(1, 1, 0);
	vprint(-400, 280, GLUT_BITMAP_9_BY_15, "Score: %.0f", score);

	drawSun();


	// light source 
	for (int i = 0; i < num; i++) {
		glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
		circle(light[i].pos.x, light[i].pos.y, 10);
		
		//test light collision
		if (testLightCollision(light[i], sun) && activeTimer && hp > 0) {
			hp -= 1;
		}
		float radius = 150;
		for (int j = 0; j < 3; j++) {
			if (testPlanetCollision(sun, planet[j], radius)) {
				planet[j].collision = true;
				//recover hp
				hp = 100;
			}
			radius += 50;
		}
	}

	//planets
	float radius = 150;
	for (int i = 0; i < 3; i++) {
		if (!planet[i].collision)
			drawPlanet(planet[i], radius);
		radius += 50;
	}

	if (hp == 0 && !gg && !showscore) {
		activeTimer = false;
		gg = true;
	}
	
	if (gg && !showscore)
		GameOver();
	else if (showscore && !gg)
		scoreBoard();

	glutSwapBuffers();

}

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	hp = 100;
	score = 0;
	factor = 1;
	activeTimer = true;
	showscore = gg = false;

	strcpy(playerN, "");

	//sun
	sun.color = { 1, 1, 0 };
	sun.bright = true;
	sun.pos = { 0, 0 };

	//# of light sources default
	num = 3;

	//light sources
	light[0] = { { 0, 200 },{ 1, 0, 0 } };
	light[1] = { { 200, 0 },{ 0, 1, 0 } };
	light[2] = { { -200, 0 },{ 0, 0, 1 } };

	for (int i = 0; i < num; i++) {
		light[i].vel = { (double)(rand() % 21) - 10 , (double)(rand() % 21) - 10 };
		light[i].bright = true;
	}

	//planets
	for (int i = 0; i < 3; i++) {
		planet[i].color = { 150 , 150 , 150 };
		planet[i].direction = rand() & 1;
		planet[i].angle = rand() % 361;
		planet[i].speed = rand() % 2 + 1;
		planet[i].radius = rand() % 6 + 20;
		planet[i].collision = false;
	}


}

void append(char* s, char c)
{
	int len = strlen(s);
	s[len] = c;
	s[len + 1] = '\0';
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ' && num < 20 && activeTimer) {

		float r = ((float)rand() / (float)(RAND_MAX)) * 1;
		float g = ((float)rand() / (float)(RAND_MAX)) * 1;
		float b = ((float)rand() / (float)(RAND_MAX)) * 1;
		light[num] = { { 0, 0 },{ r, g, b } };
		light[num].vel = { (double)(rand() % 21) - 10 , (double)(rand() % 21) - 10 };
		light[num].bright = true;
		num++;
		factor += 5;
	}

	if (gg) {
		if (key == 13 && strlen(playerN) > 1) {
			strcpy(s[gameplay].name, playerN);
			s[gameplay].score = score;
			gameplay++;
			gg = false;
			showscore = true;
		}
		else {
			//type
			if (strlen(playerN) < 20)
				append(playerN, key);
			//delete
			if (key == 8 && strlen(playerN) > 1)
				playerN[strlen(playerN) - 2] = '\0';
		}
	}


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
			strcpy(act[0].act, "Off");
		}
		else {
			light[0].color = { 1, 0, 0 };
			light[0].bright = true;
			strcpy(act[0].act, "On");
		}
		break;
	case GLUT_KEY_F2:
		if (light[1].bright) {
			light[1].color = { 0.3, 0.3, 0.3 };
			light[1].bright = false;
			strcpy(act[1].act, "Off");
		}
		else {
			light[1].color = { 0, 1, 0 };
			light[1].bright = true;
			strcpy(act[1].act, "On");
		}
		break;
	case GLUT_KEY_F3:
		if (light[2].bright) {
			light[2].color = { 0.3, 0.3, 0.3 };
			light[2].bright = false;
			strcpy(act[2].act, "Off");
		}
		else {
			light[2].color = { 0, 0, 1 };
			light[2].bright = true;
			strcpy(act[2].act, "On");
		}
		break;
	case GLUT_KEY_F4:
		if (sun.bright) {
			sun.color = { 0.3, 0.3, 0.3 };
			sun.bright = false;
			strcpy(act[3].act, "Off");
		}
		else {
			sun.color = { 1, 1, 0 };
			sun.bright = true;
			strcpy(act[3].act, "On");
		}
		break;
	case GLUT_KEY_F5:
		//activeTimer == true ? activeTimer = false : activeTimer = true;
		if (activeTimer) {
			activeTimer = false;
			strcpy(act[4].act, "Off");
		}
		else {
			activeTimer = true;
			strcpy(act[4].act, "On");
		}
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
		if (up) sun.pos.y += 10;
		if (down) sun.pos.y -= 10;
		if (left) sun.pos.x -= 10;
		if (right) sun.pos.x += 10;

		for (int i = 0; i < num; i++) {
			light[i].pos = addV(light[i].pos, light[i].vel);

			// Reflection from Walls.
			if (light[i].pos.x > (WINDOW_WIDTH / 2 - 5) || light[i].pos.x < -(WINDOW_WIDTH / 2 - 5)) {
				light[i].vel.x *= -1;
			}

			if (light[i].pos.y > (WINDOW_HEIGHT / 2 - 5) || light[i].pos.y < -(WINDOW_HEIGHT / 2 - 5)) {
				light[i].vel.y *= -1;
			}
		}

		for (int i = 0; i < 3; i++) {
			if (!planet[i].collision) {
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
			else {
				planet[i].radius = -1;
			}
		}

		score += 0.5 * factor;
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