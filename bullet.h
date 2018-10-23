#pragma once
class Bullet
{
public:
	Bullet(int direction, int x, int y);
	~Bullet();
	void draw();
	void move();
	bool  wallCollision();
	void rotation();
	int x;
	int y;
	int direction; //0: up, 1:down, 2:left, 3:right
};


