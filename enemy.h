class Enemy
{
public:
	int x;
	int y;
	int direction;
	Enemy(int x, int y);
	void draw();
	void move();
	bool wallCollision(int, int);
	bool isWallThere(int);
	bool bulletCollision();
	bool isNearPlayer(int, int);
	int getDirectionWithNoWall();
	int getPlayerPartition(int, int);
	int getDirectionToMove();
};

enum direction{UP, DOWN, RIGHT, LEFT};