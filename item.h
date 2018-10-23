#pragma once
class Item
{
public:
	Item(int type, int x, int y);
	~Item();
	void draw();
	bool playerCollision();
	int x;
	int y;
	int type;// subclasses
};

