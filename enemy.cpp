#include "enemy.h"
#include "player.h"
#include <cmath>
#include <GL/glut.h>
#include <iostream>
#include <cmath>

using namespace std;

extern int map_wall[20][20];
extern int map_bullet[20][20];
extern int map_enemy[20][20];
extern Player player;

Enemy::Enemy(int x, int y)
{
	this->x = x;
	this->y = y;
	this->direction = 0;
}

void Enemy::draw()
{
	//triangle
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x * 50, y * 50, 0);
	glColor3f(1.0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(0, 0);
	glVertex2f(50, 0);
	glVertex2f(25, 50*sin(60*3.14159/180));
	glEnd();
	glPopMatrix();

	//RIGHT eyebrow
	glPushMatrix();
	glTranslatef(x * 50, y * 50,0);
	glTranslatef(27, 15, 0);
	glColor3f(0, 0, 0);
	glRotatef(30, 0, 0, 1);
	glRectf(2, 0, 13, 3);
	glPopMatrix();

	//LEFT eyebrow
	glPushMatrix();
	glTranslatef(x * 50, y * 50, 0);
	glTranslatef(25, 15, 0);
	glColor3f(0, 0, 0);
	glRotatef(-30, 0, 0, 1);
	glRectf(-13, 0, -2, 3);
	glPopMatrix();
}

void Enemy::move() 
{
	int direction, prev_x, prev_y;
	prev_x = this->x;
	prev_y = this->y;
	direction = getDirectionToMove();
	switch(direction) {
	case UP:
		this->y = this->y + 1; break;
	case DOWN: 
		this->y = this->y - 1; break;
	case LEFT:
		this->x = this->x - 1; break;
	case RIGHT:
		this->x = this->x + 1; break;
	}
	map_enemy[prev_x][prev_y] = 0;
	map_enemy[this->x][this->y] = 1;
}

bool Enemy::wallCollision(int x, int y)
{
	return map_wall[x][y] == 1;
}

bool Enemy::bulletCollision()
{
	return map_bullet[this->x][this->y] == 1;
}

bool Enemy::isNearPlayer(int dist_x, int dist_y)
{
	if (abs(dist_x) <= 3 && abs(dist_y) <= 3) {//in close region
		return true;
	}
	return false;
}
int Enemy::getPlayerPartition(int dist_x, int dist_y)
{
	//UP, DOWN, RIGHT, LEFT
	int partition;
	if (dist_y < 0) {//1,2
		if (dist_x < 0) {//1 - UP, RIGHT
			partition = 1;
		}
		else {//2 - UP, LEFT
			partition = 2;
			if (dist_x == 0) partition = 5;
		}
	}
	else if (dist_y == 0) {//6,8
		if (dist_x > 0) partition = 6;
		else partition = 8;
	}
	else {//3,4
		if (dist_x > 0) {//3-DOWN, LEFT
			partition = 3;
		}
		else {//4-DOWN,RIGHT
			partition = 4;
			if (dist_x == 0) partition = 7;
		}
	}
	return partition;
}

bool Enemy::isWallThere(int direction) {//check if there is wall in certain direction
	int new_x = this->x;
	int new_y = this->y;
	switch (direction) {
	case UP: new_y++; break;
	case DOWN: new_y--; break;
	case RIGHT: new_x++; break;
	case LEFT: new_x--; break;
	}
	return wallCollision(new_x, new_y);
}

int Enemy::getDirectionToMove()//return Direction according to partition
{
	int dist_x, dist_y, partition, direction;
	dist_x = this->x - player.x;
	dist_y = this->y - player.y;
	if (!isNearPlayer(dist_x, dist_y)) {//far from player
		direction = getDirectionWithNoWall();
	}
	else {//near player
		partition = getPlayerPartition(dist_x, dist_y);
		//UP, DOWN, RIGHT, LEFT
		switch (partition) {
		case 1://up, right
			direction = UP;
			if (isWallThere(direction))
				direction = RIGHT;
			break;
		case 2://up, left
			direction = LEFT;
			if (isWallThere(direction))
				direction = UP;
			break;
		case 3://down, left
			direction = DOWN;
			if (isWallThere(direction))
				direction = LEFT;
			break;
		case 4://down, right
			direction = RIGHT;
			if (isWallThere(direction))
				direction = DOWN;
			break;
		case 5:
			direction = UP; break;
		case 6:
			direction = LEFT; break;
		case 7:
			direction = DOWN; break;
		case 8:
			direction = RIGHT; break;
		}
		//if there are walls everywhere
		if (isWallThere(direction))
			direction = getDirectionWithNoWall();
	}
	return direction;
}


int Enemy::getDirectionWithNoWall() //Return direction with no Wall
{
	int dir;
	do {
		dir = rand() % 4;
	} while (isWallThere(dir));
	return dir;
}
