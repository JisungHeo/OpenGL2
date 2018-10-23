#include "player.h"
#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#define PI 3.141592
extern int map_wall[20][20];
extern int map_enemy[20][20];
extern int map_item[20][20];
using namespace std;

extern Player player;
Player::Player(int x, int y) {
	this->x = x;
	this->y = y;
	this->direction = -1;//0:up, 1:down, 2:left, 3:right
	this->itemlist = (bool *)malloc(2 * sizeof(bool));//item inventory can hold 2 items
	itemlist[0] = false;
	itemlist[1] = false;
	life = 3;
}

void Player::draw() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x*50, y*50, 0);
	
	//head
	glColor3f(91/255.0, 155/255.0, 213/255.0);
	double rad = 10;
	glBegin(GL_POLYGON);
	for (int i = 0; i<360; i++)
	{
		double angle = i * 3.141592 / 180;
		double x = rad * cos(angle);
		double y = rad * sin(angle);
		glVertex2f(x + 25, y + 40);
	}
	glEnd();

	//arms
	glRectf(10, 25, 40, 30);

	//body
	glRectf(22.5, 13, 27.5, 30);
	glPopMatrix();

	//left leg
	glPushMatrix();
	glTranslatef(x * 50, y * 50, 0);
	glTranslatef(18, 0, 0);
	glRotatef(60, 0, 0, 1);
	glRectf(0, 0, 18, 5);
	glPopMatrix();

	//right leg
	glPushMatrix();
	glTranslatef(x * 50, y * 50, 0);
	glTranslatef(32, 0, 0);
	glRotatef(-60, 0, 0, 1);
	glRectf(-18, 0, 0, 5);
	glPopMatrix();
}

void Player::move(int x, int y)
{

	if (x == (this->x + 1))
		this->direction = 3; //RIGHT
	else if (x == (this->x - 1))
		this->direction = 2; //LEFT
	else if (y == (this->y + 1))
		this->direction = 0; //UP
	else
		this->direction = 1; //DOWN

	if (!wallCollision(x,y))
	{
		this->x = x;
		this->y = y;
	}	
}

bool Player::wallCollision(int x, int y)
{
	return map_wall[x][y] == 1;
}

bool Player::enemyCollision()
{
	return map_enemy[player.x][player.y] == 1;
}

bool Player::itemCollision(int x, int y)
{
	return map_item[x][y] == 1;
}

void Player::lifeUpdate() {
	if (enemyCollision())
			life--;
	return;
}
