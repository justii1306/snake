#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "cpoint.h"
#include "screen.h"
#include "winsys.h"
#include <vector>

enum Statuses{MENU, GAME, PAUSE, SCORE};

class CSnake:public CFramedWindow{
private:
	std::vector<CPoint> body;
	CPoint food_position;
	CPoint direction;
	int level;
	enum Statuses status;
public:
	CSnake(CRect r, char _c = ' ');
	void paint();
	bool handleEvent(int key);
	bool detectCollision(CPoint, CPoint);
	void printSnake();
	void moveSnake();
	void detectFood();
	void spawnFood();
	void detectWall();
	bool detectSnake();
	bool detectReversion(int key);
	void reset();
	void printFood();
	
};

#endif
