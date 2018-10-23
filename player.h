class Player
{
public:
	Player(int x, int y);
	void draw();
	void move(int x, int y);
	bool wallCollision(int, int);
	bool enemyCollision();
	bool itemCollision(int x, int y);
	void lifeUpdate();

	int x;
	int y;
	int direction;
	int life;
	bool* itemlist;
};
