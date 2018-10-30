#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "statusbar.h"
#include "player.h"

using namespace std;
extern Player player;
extern int width;
extern int height;
extern int time;
extern int game_round;

// function for printing text
// reference : http://programmingexamples.net/wiki/OpenGL/Text 
void printtext(int x, int y, string String)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 500, 0, 500);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glRasterPos2i(x, y);
	for (int i = 0; i<String.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, String[i]);
	}
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void writeItemInfo() {
	printtext(330, 80, "Item List");
	// when player got an item1
	if (player.itemlist[0])
		printtext(330, 50, "three bullets");
	// when the player got item2
	if (player.itemlist[1])
		printtext(330, 30, "speed up bullets");
}

void writeRoundInfo() {
	string game_round_s = std::to_string(game_round);
	printtext(20, 80, "Round");
	printtext(20, 50, game_round_s);
}

void writeTimerInfo() {
	string time_s = std::to_string(time);
	printtext(110, 80, "Timer");
	printtext(110, 50, time_s);
}

void writeLifeInfo() {
	string life_s = std::to_string(player.life);
	printtext(200, 80, "Life");
	printtext(200, 50, life_s);
}

// function that draws a status bar below the screen
void drawStatusBar()
{
	glViewport(0, 0, width, height / 5);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 500, 0, 100);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.3, 0.3, 0.3);
	glRectf(0, 0, 500, 100);

	glColor3f(0, 0, 0);
	writeItemInfo();
	writeLifeInfo();
	writeTimerInfo();
	writeRoundInfo();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
} 