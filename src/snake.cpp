#include "snake.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define SNAKE_HEAD body[0]
#define DIR_UP direction.x = 0; direction.y = -1;
#define DIR_DOWN direction.x = 0; direction.y = 1;
#define DIR_LEFT direction.x = -1; direction.y = 0;
#define DIR_RIGHT direction.x = 1; direction.y = 0;

#define TIME 125*1000
#define DIFFICULTY 100*100
#define FINAL_LEVEL 10

CSnake::CSnake(CRect r, char _c /*=' '*/):
  CFramedWindow(r, _c){
	body.push_back(CPoint(3, 1));
	body.push_back(CPoint(2, 1));
	body.push_back(CPoint(1, 1));
	status = MENU;
	DIR_RIGHT
	spawnFood();
	level = 1;
}

void CSnake::paint(){
  	gotoyx(geom.topleft.y -1, geom.topleft.x);
	printl("|LEVEL: %d|", level);
	if(status == MENU){
		CFramedWindow::paint();
		gotoyx(geom.topleft.y + 4, geom.topleft.x + 2);
		printl("h - toggle help information");
		gotoyx(geom.topleft.y + 5, geom.topleft.x + 2);
		printl("p - toggle pause/play mode");
		gotoyx(geom.topleft.y + 6, geom.topleft.x + 2);
		printl("r - restart game");
		gotoyx(geom.topleft.y + 7, geom.topleft.x + 2);
		printl("arrows - move window (in pause mode)");
		gotoyx(geom.topleft.y + 8, geom.topleft.x + 2);
		printl("         move snake (in play mode)");
		printSnake();
		printFood();
	} else if(status == GAME){
		CFramedWindow::paint();
		if (level >= 10){
			usleep(TIME - (DIFFICULTY*FINAL_LEVEL)); //Nie zwiększaj bardziej prędkości
		} else {
			usleep(TIME - (DIFFICULTY*level));
		}
		moveSnake();
		printSnake();
		printFood();
		detectFood();
		if(detectSnake()){
			status = SCORE;
		}
	} else if(status == PAUSE){
		CFramedWindow::paint();
		printSnake();
		printFood();
		gotoyx(geom.topleft.y + 6, geom.topleft.x + 2);
		printl("PAUSED");
		gotoyx(geom.topleft.y + 10, geom.topleft.x + 2);
		printl("Press p to continue...");
		printSnake();
		printFood();
		
	} else if(status == SCORE)
	{
		CFramedWindow::paint();
		printSnake();
		printFood();
		gotoyx(geom.topleft.y + 6, geom.topleft.x + 2);
		printl("GAME OVER");
		gotoyx(geom.topleft.y + 8, geom.topleft.x + 2);
		printl("LEVEL: %d", level);
		gotoyx(geom.topleft.y + 10, geom.topleft.x + 2);
		printl("Press r to exit to menu...");
	}
}

bool CSnake::detectCollision(CPoint firstPoint, CPoint secondPoint){
	return (firstPoint.x == secondPoint.x && firstPoint.y == secondPoint.y) ? true: false;
}

bool CSnake::handleEvent(int key){

	if(status != GAME && CFramedWindow::handleEvent(key)){
		return false;
	} else{
		if(status == GAME){
			if(key == KEY_UP){
				if(detectReversion(key)){ //jeśli będzie false, to nie zmienimy kierunku
					DIR_UP
				}
			} else if(key == KEY_DOWN){
				if(detectReversion(key)){
					DIR_DOWN
				}
			} else if(key == KEY_LEFT){
				if(detectReversion(key)){
					DIR_LEFT
				}
			} else if(key == KEY_RIGHT){
				if(detectReversion(key)){
					DIR_RIGHT
				}
			}	
		}
		if(key == 'r'){
			reset();
			
		} else if(key == 'p'){
			if(status == MENU || status == PAUSE){
				status = GAME;
			} else if(status == GAME){
				status = PAUSE;
			}
		} else if(key == 'h'){
			if(status == MENU){
				status = GAME;
			} else if(status == GAME){
				status = MENU;
			}
		}
	}
	return true;
}

void CSnake::printSnake(){
	unsigned int i = 0;
	//Wyświetl głowę
	gotoyx(body[i].y + geom.topleft.y, body[i].x + geom.topleft.x);
	printc('*');
	//Wyświetl ciało
	for(i = 1; i < body.size(); i++){
		gotoyx(body[i].y + geom.topleft.y, body[i].x + geom.topleft.x);
		printc('+');
	}
}

void CSnake::moveSnake(){
	unsigned int i = body.size() - 1;
	for(; i >= 1; i--){
		body[i] = body [i - 1];
	}
	detectWall();
}

void CSnake::detectFood(){
	if(detectCollision(SNAKE_HEAD, food_position)){
		level++;
		spawnFood();
		body.push_back(body[body.size()-1]); //dodaj segment do ciała
	}
}

void CSnake::spawnFood(){
	srand(time(NULL));
	food_position.x = rand()%(geom.size.x - 2) + 1;
	food_position.y = rand()%(geom.size.y - 2) + 1;
	unsigned int i = 0;
	for(i = 0; i < body.size(); i++){
		if(detectCollision(body[i], food_position)){ //jeśli jedzenie pojawiło się na wężu, spróbuj jeszcze raz
			spawnFood();
		}
	}
}

void CSnake::printFood(){
	gotoyx(food_position.y + geom.topleft.y, food_position.x + geom.topleft.x);
	printc('O');
}

void CSnake::detectWall(){
	int x = SNAKE_HEAD.x + direction.x; 
	int y = SNAKE_HEAD.y + direction.y;
	
	if(x <= 0){
		SNAKE_HEAD = CPoint(geom.size.x - 2, y);
		return;	
	} else if(x >= geom.size.x - 1){
		SNAKE_HEAD = CPoint(1, y);
		return;
	} else if(y <= 0){
		SNAKE_HEAD = CPoint(x, geom.size.y - 2);
		return;	
	} else if(y >= geom.size.y - 1){
		SNAKE_HEAD = CPoint(x, 1);
		return;	
	}
	SNAKE_HEAD = CPoint(x, y);
}

bool CSnake::detectSnake(){
	unsigned int i = 1;
	for (i = 1; i < body.size(); i++){
		if(detectCollision(SNAKE_HEAD, body[i])){
			return true;
		}
	}
	return false;
}

bool CSnake::detectReversion(int key){ //Jeśli wąż porusza się w jendym kierunku, a wciśniemy przeciwny kierunek to zwróć false
	if(key == KEY_UP){
		if(direction.y == 1){
			return false;
		}	
		return true;
	} else if(key == KEY_DOWN){
		if(direction.y == -1){
			return false;
		}	
		return true;
		
	} else if(key == KEY_LEFT){
		if(direction.x == 1){
			return false;
		}	
		return true;
		
	} else if(key == KEY_RIGHT){
		if(direction.x == -1){
			return false;
		}	
		return true;	
	}	
	return false;
}

void CSnake::reset(){
	body.clear();
	body.push_back(CPoint(3, 1));
	body.push_back(CPoint(2, 1));
	body.push_back(CPoint(1, 1));
	status = MENU;
	DIR_RIGHT
	spawnFood();
	level = 1;
}

