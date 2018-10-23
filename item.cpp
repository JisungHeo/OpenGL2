#include "item.h"
#include <GL/glut.h>
#include "player.h"
#include <cmath>

#define PI 3.14159
extern Player player;

Item::Item(int type, int x, int y)
{
	this->type = type;
	this->x = x;
	this->y = y;
}

Item::~Item()
{
}

void Item::draw() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	int x_fit = this->x * 50;
	int y_fit = this->y * 50;
	glTranslatef(x_fit, y_fit, 0);
	if (type == 1)
		glColor3f(1, 0.7, 0.6);
	else if (type == 2)
		glColor3f(0.6, 0.7, 1);

	double rad_outer = 25;
	double rad_inner = 12.5;
	glTranslatef(25, 25, 0);
	glRotatef(90, 0, 0, 1);
	glBegin(GL_POLYGON);
	
	for (int i = 0; i < 5; i++)
	{
		glVertex2f(rad_inner*cos((i-0.5) * 2 * PI / 5), rad_inner*sin((i-0.5) * 2 * PI / 5));
		glVertex2f(rad_outer*cos(i * 2 * PI / 5), rad_outer*sin(i * 2 * PI / 5));
	}
	glEnd();
	glPopMatrix();
}

bool Item::playerCollision()
{
	return (this->x == player.x) && (this->y == player.y);
}