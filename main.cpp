/*
   Computer graphics assignment 2
   20150309 ��ǻ�Ͱ��а� ������
   20150863 ��ǻ�Ͱ��а� ������  
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
#include "scene_node.hpp"
#include <stack>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
Player player(10, 10);
list<Bullet> listBullet;//list for managing bullet objects.
list<Enemy> listEnemy;//list for managing enemy objects
list<Item> listItem;// list for managing item objects.

//maps
extern int map_wall[20][20];
extern int map_enemy_origin[20][20];
extern int map_bullet[20][20];
extern int map_item_origin[20][20];
int map_enemy[20][20];
int map_item[20][20];

//information about game
bool game_over = false;
bool once = true;
int game_round = 1;
int enemy_timer, time_timer;
int bullet_speed, time;
int width = 500;
int height = 400;

stack<glm::mat4> mvstack; //model-view stack
glm::mat4 model_view;
glm::mat4 projection;
glm::mat4 MVP;
GLuint ProgramID;
GLuint MVPID;
// reference: 06.hierar.pdf 27 page
void traverse(SceneNode *root) {
	if (root == NULL)
		return;
	mvstack.push(model_view);
	model_view = model_view * root->m; 
	root->f();
	if (root->child != NULL)
		traverse(root->child);
	model_view = mvstack.top();
	mvstack.pop();
	if (root->sibling != NULL)
		traverse(root->sibling);
}

//Return if game is over
void checkGameOver() {
	if (player.life == 0) {
		cout << "life done" << endl;
		game_over = true;
	}
	else if (listEnemy.empty()) {
		cout << "enemy empty" << endl;
		game_over = true;
	}
	else if (time == 0) {
		cout << "time up" << endl;
		game_over = true;
	}
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

void drawEntity() {
	//draw walls
	for (int i = 0; i < 20; i++) 
		for (int j = 0; j < 20; j++)
			if (map_wall[i][j])
				drawWall(i, j);
	//draw bullets
	for (list<Bullet>::iterator it = listBullet.begin(); it != listBullet.end(); it++) 
		(*it).draw();
	//draw items
	for (list<Item>::iterator it = listItem.begin(); it != listItem.end(); it++)
		(*it).draw();
	//draw enemy
	for (list<Enemy>::iterator it = listEnemy.begin(); it != listEnemy.end(); it++) 
		(*it).draw();
	//draw player
	player.draw(); 
}
void display()
{
	//cout << "display work" << endl;
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
		
		drawEntity();
		drawStatusBar();
		glutSwapBuffers();
	}
	// if the game is over(all enemies are defeated or player touched an enemy)
	else {
		if (once) {
			glViewport(0, 0, width, height);
			printtext(250, 250, "Gameover"); 
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
	cout << "map clear" << endl;
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

// function that reads map_item and creates the objects.
void init()
{
	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++)
		{
			map_enemy[i][j] = map_enemy_origin[i][j];
			map_item[i][j] = map_item_origin[i][j];
			map_bullet[i][j] = 0;
			if (map_item[i][j]) // creates item objects 
				listItem.push_back(Item(map_item[i][j], i, j));
			if (map_enemy[i][j]) // creates enemy objects
				listEnemy.push_back(Enemy(i, j));
		} 
	cout << "init" << endl;
	cout << "number of enemy :" << listEnemy.size()<<endl;
	player.life = 3;
	player.x = 10;
	player.y = 10;
	player.itemlist[0] = false;
	player.itemlist[1] = false;
	time = 30;
	bullet_speed = 4;
	game_over = false;
	enemy_timer = 0;
	time_timer = 0;
	once = true;

	ProgramID = LoadShaders("myVS.glsl", "myFS.glsl");
	GLuint MVPID = glGetUniformLocation(ProgramID, "MVP");
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
	case 'R': {
		if (game_over) {
			cout << "r insert" << endl;
			game_over = false;
			map_clear();
			init();
			cout << "game_over :" << game_over << endl;
			game_round += 1;
		}
	}
	}
	glutPostRedisplay();
}

void display1() {
	glClearColor(0, 0, 0, 0);

	projection = glm::ortho(-250.0f, 250.0f, -200.0f, 200.0f);
	model_view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	MVP = glm::mat4(1.0) * projection *model_view;
	glUseProgram(ProgramID);

	GLuint vertexarray;
	glGenVertexArrays(1, &vertexarray);
	glBindVertexArray(vertexarray);

	static const GLfloat vertices[] = {
		0.0,0.0,0.0,
		50.0,0.0,0.0,
		50.0,50.0,0.0,
		//50.0,50.0,0.0,
	};

	static const GLfloat colors[] = {
		1.0,1.0,0.0,
		1.0,0.0,0.0,
		0,0,0,
		//0,0,0,
	};
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ProgramID);
	
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);

	glBindVertexArray(vertexarray);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glutSwapBuffers();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL Assignment 2");
	init();
	sceneGraphInit();
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutDisplayFunc(display1);
	//glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 1);
	
	glutMainLoop();
}

/** Homework #6.1 Animate the box robot 2007-10-30  200311624 ������


- idleó�� ����� ������� �ʰ� glutTimerFunc�Լ��� ��� �߽��ϴ�.
animation�� �ӵ� ������ idle��� ���� �����մϴ�.

- ������ �ϰ� �Ǵ� �κ��� ��4 �κ����� �Ӹ�, ����, �ٸ�, �� �Դϴ�.
�ȴ� ����� ���� �ڿ������� ǥ���ϱ� ���� �� �ٸ� ���ۿ� �����
�Ӹ������� �߰� �߽��ϴ�.

- ȭ���� �߽����� �߽����� robot�� ȸ���ϸ鼭 �Ƚ��ϴ�.


- ������ ����, �Ӹ�, ����ƴ, �ٸ� ����, �� ������ ũ�Ⱚ��
�ٸ����ؼ� �׸� �� �ֽ��ϴ�.
- �� ���� �������� �߽������ؼ� ������ �����Ӱ� ������ �� �ֽ��ϴ�.
- �� ������ Ȯ�� �� �� �ֵ��� �������� ���� �׷Ƚ��ϴ�.
- ���� ������ �ּ�ó��.

*//*
#include <windows.h>
#include <gl/GL.H>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>

#define CUBE_SIDE 10.0f			//�ҽ� ť�� ũ��.(ũ��=�Ѻ��� ����)
#define RB_AXIS_X 400.0f		//�κ��� �߽� ��ǥ x.
#define RB_AXIS_Y 0.0f		//�κ��� �߽� ��ǥ y.
#define RB_BODY_SIDE 120.0f		//�κ� ���� ũ��.
#define RB_HEAD_SIDE 70.0f		//�κ� �Ӹ� ũ��.
#define RB_JOINT_GAP 20.0f		//�κ� ���� ƴ ũ��.
#define RB_LEG_LENGTH 120.0f	//�κ� �ٸ� ����.
#define RB_ARM_LENGTH 140.0f	//�κ� �� ����.
#define VIEW_SIZE 700.0f	

static double time = 0;		//time ����.
GLfloat center_Y = 0.0f;		//ȭ�� �߽� y�� angle����.
GLfloat left_arm_X = 0.0f;	//�� �� X�� angle.
GLfloat right_arm_X = 0.0f;	//���� �� X�� angle.
GLfloat left_up_leg_X = 0.0f;	//�� �ٸ� ���� X��.
GLfloat right_up_leg_X = 0.0f;//���� �ٸ� ���� X��.
GLfloat left_down_leg_X = 0.0f;//�� ���� ����.
GLfloat right_down_leg_X = 0.0f;//���� ���� ����.
GLfloat body_Y = 0.0f;//���߽� ��.
GLfloat head_Y = 0.0f;//�Ӹ��߽� ��.


void glInit(void) {	// GL�ʱ�ȭ �Լ�.

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	GLfloat ambientLight[] = { 0.3f,0.3f,0.3f,1.0f };
	GLfloat diffuseLight[] = { 0.7f,0.7f,0.7f,1.0f };
	GLfloat specular[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat specref[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat position[] = { 400.0f,300.0f,700.0f,1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	glClearColor(1.0, 1.0, 1.0, 0.0); // ��� ���.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-VIEW_SIZE, VIEW_SIZE, -VIEW_SIZE, VIEW_SIZE, -VIEW_SIZE, VIEW_SIZE);

}

void drawAxis(GLfloat angle) {	// ���� �׸��� �޼ҵ�.

	glBegin(GL_LINES);

	glColor3f(1, 0, 0);
	glVertex3f(-angle, 0, 0);
	glVertex3f(angle, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, -angle, 0);
	glVertex3f(0, angle, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, -angle);
	glVertex3f(0, 0, angle);

	glColor3f(0.3f, 0.7f, 1.0f);
	glEnd();
}

void drawArm() { //�ȱ׸���.

	glTranslatef(RB_BODY_SIDE / 2.0f + RB_JOINT_GAP, RB_BODY_SIDE*0.9, 0.0f);
	//drawAxis(50.0f);
	glRotatef(15, 0, 0, 1);	// �� ���� �߽����� �� ���� ���� ������.
	glRotatef(left_arm_X, 1, 0, 0);   //�� ����.
	glRotatef(-30, 0, 1, 0);

	glScalef(RB_ARM_LENGTH / 4.0 / CUBE_SIDE, RB_ARM_LENGTH / CUBE_SIDE, RB_ARM_LENGTH / 4.0 / CUBE_SIDE);
	glTranslatef(0.0f, -CUBE_SIDE / 2.0f, 0.0f);
	glutSolidCube(CUBE_SIDE); // ���� �׸���.

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-(RB_BODY_SIDE / 2.0f + RB_JOINT_GAP), RB_BODY_SIDE*0.9, 0.0f);
	//drawAxis(50.0f);
	glRotatef(-15, 0, 0, 1);  // �� ���� �߽����� �� ���� ���� ������.
	glRotatef(right_arm_X, 1, 0, 0);	//�� ����.
	glRotatef(30, 0, 1, 0);

	glScalef(RB_ARM_LENGTH / 4.0 / CUBE_SIDE, RB_ARM_LENGTH / CUBE_SIDE, RB_ARM_LENGTH / 4.0 / CUBE_SIDE);
	glTranslatef(0.0f, -CUBE_SIDE / 2.0f, 0.0f);
	glutSolidCube(CUBE_SIDE); // ������ �׸���.

}

void drawLeg() {	//�ٸ� �׸���.

	glTranslatef(RB_BODY_SIDE / 4.0f, -RB_JOINT_GAP, 0.0f);  //�� �ٸ� �߽�.
															 //drawAxis(50.0f);
	glRotatef(2, 0, 0, 1);		// ����� �ٸ� ����.
	glRotatef(left_up_leg_X, 1, 0, 0); //�ٸ� ����.
	glScalef(RB_LEG_LENGTH / 2.5 / CUBE_SIDE, RB_LEG_LENGTH / CUBE_SIDE, RB_LEG_LENGTH / 2.5 / CUBE_SIDE);
	glTranslatef(0.0f, -CUBE_SIDE / 2.0f, 0.0f);
	glutSolidCube(CUBE_SIDE);	//���� �ٸ� �������� �׸���.

	glScalef(CUBE_SIDE*2.5 / RB_LEG_LENGTH, CUBE_SIDE / RB_LEG_LENGTH, CUBE_SIDE*2.5 / RB_LEG_LENGTH);
	// ������ ���� ����.

	glTranslatef(0.0f, -RB_LEG_LENGTH / 2.0f - RB_JOINT_GAP, 0.0f); // �� �ٸ� �߽�.
																	//drawAxis(50.0f);
	glRotatef(left_down_leg_X, 1, 0, 0);// ���� ����. ���� ����.
	glScalef(RB_LEG_LENGTH / 2.5 / CUBE_SIDE, RB_LEG_LENGTH / CUBE_SIDE, RB_LEG_LENGTH / 2.5 / CUBE_SIDE);
	glTranslatef(0.0f, -CUBE_SIDE / 2.0f, 0.0f);
	glutSolidCube(CUBE_SIDE);	//���� �ٸ� ������ �׸���.

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-RB_BODY_SIDE / 4.0f, -RB_JOINT_GAP, 0.0f);  //�� �ٸ� �߽�.
															  //drawAxis(50.0f);
	glRotatef(-2, 0, 0, 1);		// ����� �ٸ� ����.
	glRotatef(right_up_leg_X, 1, 0, 0);//�ٸ� ����.
	glScalef(RB_LEG_LENGTH / 2.5 / CUBE_SIDE, RB_LEG_LENGTH / CUBE_SIDE, RB_LEG_LENGTH / 2.5 / CUBE_SIDE);
	glTranslatef(0.0f, -CUBE_SIDE / 2.0f, 0.0f);
	glutSolidCube(CUBE_SIDE);	//������ �ٸ� �������� �׸���.

	glScalef(CUBE_SIDE*2.5 / RB_LEG_LENGTH, CUBE_SIDE / RB_LEG_LENGTH, CUBE_SIDE*2.5 / RB_LEG_LENGTH);
	//������ ���� ����.
	glTranslatef(0.0f, -RB_LEG_LENGTH / 2.0f - RB_JOINT_GAP, 0.0f); // �� �ٸ� �߽�.
																	//drawAxis(50.0f);
	glRotatef(right_down_leg_X, 1, 0, 0);// ���� ����. ���� ����.
	glScalef(RB_LEG_LENGTH / 2.5 / CUBE_SIDE, RB_LEG_LENGTH / CUBE_SIDE, RB_LEG_LENGTH / 2.5 / CUBE_SIDE);
	glTranslatef(0.0f, -CUBE_SIDE / 2.0f, 0.0f);
	glutSolidCube(CUBE_SIDE);	//���� �ٸ� ������ �׸���.

}

void drawBody() { //���� �׸���.

	glTranslatef(RB_AXIS_X, RB_AXIS_Y, 0.0f); // �κ� �߽�.
											  //drawAxis(80.0f);	//�κ� �߽� �� �׸���.
	glRotatef(0, 0, 1, 0); //�κ��� ������ �ޱ�.
	glPushMatrix(); //�κ��� �� ������ �׸��� �߽��� �Ǵ� ��Ʈ���� Ǫ��.

	glScalef(RB_BODY_SIDE / CUBE_SIDE, RB_BODY_SIDE / CUBE_SIDE, RB_BODY_SIDE / CUBE_SIDE);
	glTranslatef(0.0f, CUBE_SIDE / 2, 0.0f);
	glRotatef(body_Y, 0, 1, 0);	//���� ����.
	glutSolidCube(CUBE_SIDE);  // ����׸���.

}

void drawHead() { //�Ӹ� �׸���.

	glTranslatef(0.0f, RB_BODY_SIDE + RB_JOINT_GAP + (RB_HEAD_SIDE / 2.0f), 0.0f); // �� �߽�.
																				   //drawAxis(80.0f);
	glRotatef(head_Y, 0, 1, 0);//�� ������.
	glScalef(RB_HEAD_SIDE / CUBE_SIDE, RB_HEAD_SIDE / CUBE_SIDE, RB_HEAD_SIDE / CUBE_SIDE);
	glutSolidCube(CUBE_SIDE); // �� �׸���.

}

void drawBoard() { // ���׸���.

	glBegin(GL_TRIANGLE_FAN); //������ �� �� �׸���.


	glColor3f(0.8f, 0.8f, 0.8f); //���� �Ķ�.
	glNormal3f(0, 1, 0);
	glVertex3f(0.0f, -278, 0.0f); //������ �� �� ����.
	float angle;
	for (angle = 0.0f; angle < (2.0f*3.14159); angle += (3.14159 / 360.0f)) {

		glVertex3f((500 * cos(angle)), -278.0f, (500 * sin(angle)));
	}
	glEnd();

}

void robot_Display() {  // ���÷��� �ݹ��Լ�.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW); // �𵨺� ��Ʈ���� �ʱ�ȭ.
	glLoadIdentity();

	gluLookAt(50, 40, 80, 0, 0, 0, 0, 1, 0); // ī�޶� ����.
	drawBoard();
	drawAxis(50.0f);	//�� �׸���.	

	glRotatef(center_Y, 0, 1, 0);
	glPushMatrix();



	drawBody(); // ����.
	glPopMatrix();
	glPushMatrix();

	drawHead();	// �Ӹ�.
	glPopMatrix();
	glPushMatrix();

	drawLeg();	// �ٸ�.
	glPopMatrix();
	glPushMatrix();

	drawArm();  // ��.

	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers(); //���� ����.

}

void timerFunction(int value) { //�ִϸ��̼� ó���� ���� timefunc�ݹ� �Լ�.

	time = time + 0.1047;//�ð� ��������.

	static float step = 10;

	if (step <= 21) { //�ٸ��� �߷� ���� ������ �κ��� ������ ������ ����.
		center_Y -= 0.65;
		step++;
	}
	else if (21 < step) { //���� ���� �ٸ��� �ٲ�� ������ �ణ ����.
		center_Y -= 0.04;
		step++;
		if (step >= 30) step = 0;
	}

	head_Y = sin(time) * 5; //�Ӹ��� 5�� ������ ��鸲.
	body_Y = sin(time) * 10; //������ 10�� ������ ��鸲.

	left_arm_X = sin(time) * 20;//������ 20�� ������.
	right_arm_X = -left_arm_X;//�������� ���� �ݴ�� 20�� ����.

	right_up_leg_X = sin(time) * 20;//�����ٸ��� ���Ȱ� ���� ������.
	left_up_leg_X = -right_up_leg_X;//�޴ٸ��� �����Ȱ� ���� ������.

	
	right_down_leg_X = abs(long(right_up_leg_X / 1.2f));//���������� �ٸ� ������ 1.2�� ���� ���밢��.
	left_down_leg_X = abs(long(left_up_leg_X / 1.2f));//�޹����� �ٸ� ������ 1.2�� ���� ���밢��.

	glutPostRedisplay();//�ٽ� �׸���.

	glutTimerFunc(22, timerFunction, 1); //���� Ÿ�̸� ����.

}

void main(int argc, char** argv) {

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //���� ���� �÷� ���.
	glutInitWindowSize(700, 700); //������ ũ�� ����.
	glutInitWindowPosition(0, 0); //������ ��ġ����.
	glutCreateWindow("robert"); //Ÿ��Ʋ.
	glutTimerFunc(1000, timerFunction, 1);//Ÿ�̸� ����.
	glutDisplayFunc(robot_Display);
	glInit();
	glutMainLoop();
}*/