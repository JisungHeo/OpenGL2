/*
   Computer graphics assignment 2
   20150309 컴퓨터공학과 허지성
   20150863 컴퓨터공학과 김지수  
*/
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <algorithm>
#include <list>
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "item.h"
#include "statusbar.h"
#include "shader.hpp"
#include <glm/glm.hpp>

using namespace std;
Player player(10, 10);
list<Bullet> listBullet;//list for managing bullet objects.
list<Enemy> listEnemy;//list for managing enemy objects
list<Item> listItem;// list for managing item objects.

//maps
extern int map_wall[20][20];
extern int map_enemy[20][20];
extern int map_bullet[20][20];
extern int map_item[20][20];

//information about game
bool game_over = false;
bool once = true;
int game_round = 1;
int enemy_timer, time_timer;
int bullet_speed, time;
int width = 500;
int height = 400;


//Return if game is over
void checkGameOver() {
	if (player.life == 0 || 
		listEnemy.empty()||
		time == 0)
		game_over = true;
}

// function called when the window size is changed
void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);// modifying viewport as window size changes
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 
	gluOrtho2D(-250, 250, -200, 200); 
}

// function for drawing a wall cell
void drawWall(int i, int j)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(i * 50, j * 50, 0);
	glColor3f(86 / 255.0, 52 / 255.0, 27 / 255.0);
	glRectf(0, 0, 50, 50);
	glColor3f(151 / 255.0, 84 / 255.0, 23 / 255.0);
	glRectf(0, 2, 16, 12.5);
	glRectf(18, 2, 50, 12.5);
	glRectf(0, 14.5, 32, 25);
	glRectf(34, 14.5, 50, 25);
	glRectf(0, 27, 16, 37.5);
	glRectf(18, 27, 50, 37.5);
	glRectf(0, 39.5, 32, 50);
	glRectf(34, 39.5, 50, 50);
	glPopMatrix();
}

void display()
{
	if (!game_over) { // if game is not over.
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, height / 5, width, height * 4 / 5); // upper part of the viewport
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-250, 250, -200, 200);
		
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		double camera_x = min(max(player.x * 50, 250), 750);
		double camera_y = min(max(player.y * 50, 200), 800);
		gluLookAt(camera_x, camera_y, 0, camera_x, camera_y, -1, 0, 1, 0);
		
		for (int i = 0; i < 20; i++) // draw walls
			for (int j = 0; j < 20; j++)
				if (map_wall[i][j])
					drawWall(i, j);
		for (list<Bullet>::iterator it = listBullet.begin(); it != listBullet.end(); it++) // draw bullets
			(*it).draw();
		for (list<Item>::iterator it = listItem.begin(); it != listItem.end(); it++)// draw items
			(*it).draw();
		for (list<Enemy>::iterator it = listEnemy.begin(); it != listEnemy.end(); it++) //draw enemies
			(*it).draw();
		player.draw(); // draw player
		drawStatusBar();
		glutSwapBuffers();
	}
	// if the game is over(all enemies are defeated or player touched an enemy)
	else {
		if (once) {
			glViewport(0, 0, width, height);
			printtext(140, 180, "Gameover"); 
			once = false;
			glutSwapBuffers();
		}
	}//drawStatusBar();
	//glutSwapBuffers();
}

// function that describes the effects of pushing arrow keys.
void special(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		player.move(player.x, player.y + 1);
		break;
	case GLUT_KEY_DOWN:
		player.move(player.x, player.y - 1);
		break;
	case GLUT_KEY_LEFT:
		player.move(player.x - 1, player.y);
		break;
	case GLUT_KEY_RIGHT:
		player.move(player.x + 1, player.y);
		break;
	}
	player.lifeUpdate();
	glutPostRedisplay();
}

void bulletLoad() {
	int bul_x = player.x;
	int bul_y = player.y;
	int dir = player.direction;
	if (player.itemlist[0]) {
		switch (dir) {
		case UP:
		case DOWN:
			listBullet.push_back(Bullet(dir, bul_x - 1, bul_y));
			listBullet.push_back(Bullet(dir, bul_x + 1, bul_y));
			break;
		case LEFT:
		case RIGHT:
			listBullet.push_back(Bullet(dir, bul_x, bul_y - 1));
			listBullet.push_back(Bullet(dir, bul_x, bul_y + 1));
			break;
		}
	}
	// item2: speed up bullets.
	if (player.itemlist[1])
		bullet_speed = 2;
	listBullet.push_back(Bullet(dir, bul_x, bul_y));
}

//bullet management
void bulletUpdate() {
	for (list<Bullet>::iterator it = listBullet.begin(); it != listBullet.end();) {
		(*it).move();
		if ((*it).wallCollision())// when a bullet reached to wall, it disappears.
		{
			(*it).~Bullet();
			listBullet.erase(it++);
		}
		else
			it++;
	}
}

//enemy management
void enemyUpdate() {
	for (list<Enemy>::iterator it = listEnemy.begin(); it != listEnemy.end();)
	{
		if ((*it).bulletCollision()) // when an enemy got fired, it disappears.
		{
			map_enemy[(*it).x][(*it).y] = 0;
			(*it).~Enemy();
			listEnemy.erase(it++);
		}
		else
			it++;
	}
	//Enemy moves periodically. (enemy move is a kind of timer)
	if (enemy_timer >= 100 * bullet_speed) {
		for (list<Enemy>::iterator it = listEnemy.begin(); it != listEnemy.end(); it++) {
			(*it).move();
		}
		enemy_timer = 0;
		player.lifeUpdate();
	} 
	enemy_timer += bullet_speed;// enemy move timer update
}

void itemUpdate() {
	for (list<Item>::iterator it = listItem.begin(); it != listItem.end();)//item management
	{
		if ((*it).playerCollision()) // player got an item
		{
			int type = (*it).type;
			player.itemlist[type - 1] = true;
			(*it).~Item();
			listItem.erase(it++);
		}
		else
			it++;
	}
}

void timeUpdate() {
	if (time_timer > 200) {
		time--;
		time_timer = 0;
	}
	time_timer += 1;
}

// timer function for updating the status of objects.
void timer(int value)
{
	if (!game_over) {
		bulletUpdate();
		itemUpdate();
		enemyUpdate();
		timeUpdate();
	}
	checkGameOver();
	glutPostRedisplay();
	glutTimerFunc(bullet_speed, timer, value + 1);
}

//Erase every entity in item, enemy , bullet list
void map_clear() {
	for (list<Item>::iterator it = listItem.begin(); it != listItem.end();) {
		(*it).~Item();
		listItem.erase(it++);
	}
	for (list<Enemy>::iterator it = listEnemy.begin(); it != listEnemy.end(); ) {
		(*it).~Enemy();
		listEnemy.erase(it++);
	}

	for (list<Bullet>::iterator it = listBullet.begin(); it != listBullet.end();){
		(*it).~Bullet();
		listBullet.erase(it++);
	}
}

GLuint ProgramID;
glm::mat4 MVP;
GLuint MVPID;
// function that reads map_item and creates the objects.
void init()
{
	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++)
		{
			if (map_item[i][j]) // creates item objects 
				listItem.push_back(Item(map_item[i][j], i, j));
			if (map_enemy[i][j]) // creates enemy objects
				listEnemy.push_back(Enemy(i, j));
		} 
	player.life = 3;
	player.x = 10;
	player.y = 10;
	time = 30;
	bullet_speed = 4;
	game_over = false;
	enemy_timer = 0;
	time_timer = 0;
	once = true;

	ProgramID = LoadShaders("myVS.glsl", "myFS.glsl");
}
// function for keyboard event
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		//space
	case ' ':
		bulletLoad();
		break;
	case 'r':
	case 'R':
		if (game_over) {
			//map_clear();
			init();
			game_round += 1;
		}
	}
	glutPostRedisplay();
}

void display1() {
	glClearColor(0, 0, 0, 0);
	
	glUseProgram(ProgramID);
	GLuint vertexarray;
	glGenVertexArrays(1, &vertexarray);
	glBindVertexArray(vertexarray);

	static const GLfloat vertices[] = {
		0.0,0.0,0.0,
		0.0,50.0,0.0,
		50.0,50.0,0.0,
	};
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vertexarray);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	GLuint MVPID = glGetUniformLocation(ProgramID, "gl_ModelViewProjectionMatrix");


	glutSwapBuffers();
}
void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL Assignment 1");
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutDisplayFunc(display1);
	//glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 1);
	init();
	glutMainLoop();
}