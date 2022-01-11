#include <stdio.h>
#include <string.h>
#include <time.h>
#include <graphics.h>
#include <conio.h>
#include <fstream>
#include <iostream>

using namespace std;

#define 	MINW 		25
#define 	MINH 		25
#define 	MAXW 		425
#define 	MAXH 		225

#define 	BACKGROUND	15
#define		SNAKE_COLOR 9
#define		MAX			100
#define 	R			5

#define		WIDTH		800
#define		HEIGHT		600

struct Point {
	int x,y;
};

struct Direction {
	int x, y;
};

const int DIRECTION = 10;
const struct Direction RIGHT = {DIRECTION, 0};
const struct Direction LEFT = {-DIRECTION, 0};
const struct Direction UP = {0, -DIRECTION};
const struct Direction DOWN = {0, DIRECTION};

void copyPoint(Point &src, Point &point);
void drawPoint(Point p);
void drawSnake(Point *snake, int size);
void moveSnake(Point *snake, int size, Direction direction);
void eatPoint(Point *snake, int &size, Direction direction);
void changeDirection(char c, Direction &direction);
Point randomPoint(Point *snake, int size);
void drawGame(Point *snake, int size, Point food);
bool isSnakeDead(Point *snake, int size);
void showText(int x, int y, char *str);
void showTextColor(int x, int y, char *str, int color);
void run();
int startGame();
int readHightestScore();
void writeHighestScore(int score);

int main() {
	srand(time(NULL));
	initwindow(WIDTH, HEIGHT);
	cleardevice();
	
	run();
	

	getch();
	return 0;
}

int startGame() {
	// Choose level
	cleardevice();
	int level;
	int x = WIDTH / 2 - WIDTH / 4 + WIDTH / 8 - WIDTH / 16;
	int y = HEIGHT / 2;
	showText(x, y, "Enter level(1-3): ");

	while(true) {
		if(kbhit()) {
			char c = getch();
			if(c == -32) c = getch();
			
			if(c >= '1' && c <= '3') {
				char s[64];
				sprintf(s, "%c", c);
				showText(x + 280, y, s);
				
				if(getch() == 13) {
					level = c - '0';
					break;
				} else {
					showText(x + 280, y, " ");
				}
			}
		}
		delay(50);		
	}
	
	
	// Init game
	int size = 1;
	int initX = 50;
	int initY = 50;
	int time = 200 - 30 * level;
	Direction direction = DOWN;
	
	Point *snake = new Point[MAX];
	snake[0].x = initX;
	snake[0].y = initY;
	
	Point food = randomPoint(snake, size);
	bool isAlive = true;
	
	while(isAlive) {
		if(kbhit()) {
			char c = getch();
			if(c == -32) c = getch();
			
			changeDirection(c, direction);
		}
		
		if(snake[0].x == food.x && snake[0].y == food.y) {
			eatPoint(snake, size, direction);
			food = randomPoint(snake, size);
		} else {
			moveSnake(snake, size, direction);
		}
		
		drawGame(snake, size, food);
		
		if(isSnakeDead(snake, size)) {
			int x = MAXW / 2 - MAXW / 4 + MAXW / 8 - MAXW / 16 + MAXW / 32;
			int y = MAXH / 2;
			
			showText(x, y, "Game Over!");
			isAlive = false;
			getch();
		}
		
		delay(time);
	}
	
	return size;
}

void run() {
	int x = WIDTH / 2 - WIDTH / 4 + WIDTH / 8 - WIDTH / 16;
	int y = HEIGHT / 2 - 60;
	
	bool isPLaying = true;
	int hightestScore = readHightestScore();
	char str[256];
	
	while(true) {
		if(kbhit()) {
			char c = getch();
			if(c == -32) c = getch();
			
			if(c == 72 || c == 80) {
				isPLaying = !isPLaying;
			}
			else if (c == 13){
				if(isPLaying) {
					int score = startGame();
					
					if(score > hightestScore) {
						hightestScore = score;
						writeHighestScore(score);
					}
				} else {
					exit(0);
				}
			}
		}
		
		cleardevice();
		sprintf(str, "Hightest Score: %d", hightestScore);
		
		if(isPLaying) {
			showTextColor(x, y, "Play Game", SNAKE_COLOR);
			showText(x, y + 30, "Exit");
			showText(x, y + 60, str);
		} else {
			showText(x, y, "Play Game");
			showTextColor(x, y + 30, "Exit", SNAKE_COLOR);
			showText(x, y + 60, str);
		}
		
		delay(100);
	}
}

int readHightestScore(){
	int score = 0;
	
	ifstream file("score.txt");
	file >> score;
	file.close();
	return score;
}

void writeHighestScore(int score) {
	ofstream file("score.txt");
	file << score;
	file.close();
}

void showTextColor(int x, int y, char *str, int color) {
	setcolor(color);
	showText(x, y, str);
	setcolor(BACKGROUND);
}

void showText(int x, int y, char *str) {
	settextstyle(0, 0, 2);
	outtextxy(x, y, str);
}

bool isSnakeDead(Point *snake, int size) {
	Point head = snake[0];
	
	if(head.x <= MINW || head.x >= MAXW) return true;
	if(head.y <= MINH || head.y >= MAXH) return true;
	
	for(int i=1; i < size; i++) {
		if(head.x == snake[i].x && head.y == snake[i].y) return true;
	}
	
	return false;
}

void drawGame(Point *snake, int size, Point food) {
	cleardevice();
	
	drawPoint(food);
	drawSnake(snake, size);
	
	setcolor(SNAKE_COLOR);
	setlinestyle(0, 0, 2);
	
	line(MINW, MINH, MAXW, MINH);
	line(MINW, MAXH, MAXW, MAXH);
	line(MINW, MINH, MINW, MAXH);
	line(MAXW, MINH, MAXW, MAXH);
	
	char s[64];
	sprintf(s, "SCORE: %d", size);
	settextstyle(0, 0, 2);
	outtextxy(MAXW + DIRECTION, MINH, s);
	
	setcolor(BACKGROUND);
}

Point randomPoint(Point *snake, int size) {
	Point p;
	
	int x;
	int y;
	
	while(true) {
		x = (rand() % MAXW + MINW) / DIRECTION;
		y = (rand() % MAXH + MINH) / DIRECTION;
		
		x *= DIRECTION;
		y *= DIRECTION;
		
		bool isCorrect = true;	
		
		for(int i=0; i<size; i++) {
			if(snake[i].x == x && snake[i].y == y) {
				isCorrect = false;
				break;
			}
		}
		
		if(x <= MINW + DIRECTION || x >= MAXW - DIRECTION) isCorrect = false;
		if(y <= MINH + DIRECTION || y >= MAXH - DIRECTION) isCorrect = false;
		
		if(isCorrect) break;
	}
	
	p.x = x;
	p.y = y;
	return p;
}

void changeDirection(char c, Direction &direction) {
	switch(c){
		case 72:
			// Key Up
			if(direction.y != DIRECTION) {
				direction = UP;
			}
			break;
		case 80:
			// Key Down
			if(direction.y != -DIRECTION) {
				direction = DOWN;
			}
			break;
		case 77:
			// Key Right
			if (direction.x != -DIRECTION) {
			 	direction = RIGHT;
			}
			break;
		case 75:
			// Key Left
			if (direction.x != DIRECTION) {
			 	direction = LEFT;
			}
			break;
	}
}

void eatPoint(Point *snake, int &size, Direction direction) {
	snake[size] = Point();
	size++;
	moveSnake(snake, size, direction);
}

void moveSnake(Point *snake, int size, Direction direction) {
	for(int i=size-1; i>0; i--) {
		copyPoint(snake[i], snake[i - 1]);
	}
	
	snake[0].x += direction.x;
	snake[0].y += direction.y;
}

void drawSnake(Point *snake, int size) {
	for(int i=0; i<size; i++) {
		drawPoint(snake[i]);
	}
}

void drawPoint(Point p) {
	setcolor(SNAKE_COLOR);
	circle(p.x, p.y, R);
	setcolor(BACKGROUND);
}

void copyPoint(Point &src, Point &point) {
	src.x = point.x;
	src.y = point.y;
}

