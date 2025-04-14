#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>
#define wHeight 40 // height of the road
#define wWidth 100 // width of the road
#define lineX 45 // x coordinate of the middle line
#define lineLEN 10 // distance of the middle line from the beginning and the end
#define EXITY 35 // coordinate showing the end of the road
#define leftKeyArrow 260 // ASCII code of the left arrow key
#define RightKeyArrow 261 // ASCII code of the right arrow key
#define leftKeyA 97// ASCII code of A
#define RightKeyD 100 // ASCII code of D
#define ESC 27 // // ASCII code of the ESC key
#define ENTER 10 // ASCII code of the ENTER key
#define KEYPUP 259 // ASCII code of the up arrow key
#define KEYDOWN 258 // ASCII code of the down arrow key
#define KEYERROR -1 // ASCII code returned if an incorrect key is pressed
#define SAVEKEY 115 // ASCII code of S
#define levelBound 300 // To increase level after 300 points
#define MAXSLEVEL 5 // maximum level
#define ISPEED 500000 // initial value for game moveSpeed
#define DRATESPEED 100000 // to decrease moveSpeed after each new level
#define MINX 5 // minimum x coordinate value when creating cars
#define MINY 10 // the maximum y coordinate value when creating the cars, then we multiply it by -1 and take its inverse
#define MINH 5 // minimum height when creating cars
#define MINW 5 // minimum width when creating cars
#define SPEEDOFCAR 3 // speed of the car driven by the player
#define YOFCAR 34 // y coordinate of the car used by the player
#define XOFCAR 45 // x coordinate of the car used by the player
#define IDSTART 10 // initial value for cars ID
#define IDMAX 20// maximum value for cars ID
#define COLOROFCAR 3 // color value of the car used by the player
#define POINTX 91 //x coordinate where the point is written
#define POINTY 42 //y coordinate where the point is written
#define MENUX 10 // x coordinate for the starting row of the menus
#define MENUY 5 // y coordinate for the starting row of the menus
#define MENUDIF 2 // difference between menu rows
#define MENUDIFX 20 // difference between menu columns
#define MENSLEEPRATE 200000 // sleep time for menu input
#define GAMESLEEPRATE 250000 // sleep time for player arrow keys
#define EnQueueSleep 1 // EnQueue sleep time
#define DeQueueSleepMin 2 // DeQueue minimum sleep time
#define numOfcolors 4 // maximum color value that can be selected for cars
#define maxCarNumber 5 // maximum number of cars in the queue
#define numOfChars 3 // maximum number of patterns that can be selected for cars
#define settingMenuItem 2 // number of options in the setting menu
#define mainMenuItem 6 // number of options in the main menu
using namespace std;
typedef struct Car{ //
    int ID;
    int x;
    int y;
    int height;
    int width;
    int speed;
    int clr;
    bool isExist;
    char chr;
}Car;
typedef struct Game{
    int leftKey;
    int rightKey;
    queue<Car> cars;
    bool IsGameRunning;
    bool IsSaveCliked;
    int counter;
    pthread_mutex_t mutexFile;
    Car current;
    int level;
    int moveSpeed;
    int points;
}Game;
Game playingGame; // Global variable used for new game
const char *gameTxt =  "game.txt";
const char *CarsTxt =  "cars.txt";
const char *pointsTxt =  "points.txt";
//Array with options for the Setting menu
const char *settingMenu[50] = {"Play with < and > arrow keys","Play with A and D keys"};
//Array with options for the Main menu
const char *mainMenu[50] = {"New Game","Load the last game","Instructions", "Settings","Points","Exit"};
void drawCar(Car c, int type, int direction); //prints or remove the given car on the screen
void printWindow(); //Draws the road on the screen
void *newGame(void *); // manages new game
void initGame(); // Assigns initial values to all control parameters for the new game
void initWindow(); //Creates a new window and sets I/O settings
void printMenu(int highlight);
void showSettings();
void showInstructions();
void showPoints();
void *enqueueCar(void *);
void *moveCars(void *);
void checkCollision();
int getRandomNumber(int sValue,int eValue);
char getRandomCarChr();
void updatePoints(int points);
void drawTree(int y, int x);
void saveGame();
void loadGame();
int main()
{
    initscr();
    start_color();
    keypad(stdscr, TRUE);
    curs_set(0);
    cbreak();
    noecho();

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    int choice = 0;
    int highlight = 0;
    int ch;
    playingGame.leftKey = leftKeyArrow;
    playingGame.rightKey = RightKeyArrow;
    srand(time(NULL));
    while (true) {
        printMenu(highlight);
        ch = getch();
        usleep(200000);
        switch (ch) {
            case KEY_UP:
                highlight = (highlight == 0) ? 5 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == 5) ? 0 : highlight + 1;
                break;
            case 10:
                if (highlight == 3) {
                    showSettings();
                }
                else if (highlight == 2) {
                    showInstructions();
                }
                else if (highlight == 4) {
                    showPoints();
                }
                else if (highlight == 5) {
                    endwin();
                    return 0;
                }else if(highlight==0){
                    clear();
                    refresh();
                    endwin();
                    initGame();
                    initWindow();
                    pthread_t thread;
                    pthread_create(&thread,NULL,newGame,NULL);
                    pthread_join(thread,NULL);
                }
                else if(highlight==1){
                    loadGame();
                }
                else {
                    mvprintw(15, 0, "Selected: %s", mainMenu[highlight]);
                    refresh();
                    getch();
                    endwin();
                    return 0;
                }
                break;
            default:
                break;
        }
    }
    return 0;
}
void saveGame() {
    clear();

    pthread_mutex_lock(&playingGame.mutexFile);

    FILE *points = fopen("game.txt", "wb");

    fwrite(&playingGame.points, sizeof(int), 1, points);
    fwrite(&playingGame.level, sizeof(int), 1, points);
    fwrite(&playingGame.current, sizeof(Car), 1, points);
    fclose(points);

    FILE *cars = fopen("cars.txt", "wb");

    queue<Car> tempQueue = playingGame.cars;
    while (!tempQueue.empty()) {
        Car car = tempQueue.front();
        tempQueue.pop();
        fwrite(&car, sizeof(Car), 1, cars);
    }

    fclose(cars);

    pthread_mutex_unlock(&playingGame.mutexFile);

    playingGame.IsGameRunning = false;
    playingGame.IsSaveCliked = true;
    refresh();
    usleep(1000000);
    getch();
    printMenu(0);
}

void loadGame() {
    clear();
    refresh();

    pthread_mutex_lock(&playingGame.mutexFile);

    FILE *points = fopen("game.txt", "rb");

    fread(&playingGame.points, sizeof(int), 1, points);
    fread(&playingGame.level, sizeof(int), 1, points);
    fread(&playingGame.current, sizeof(Car), 1, points);
    fclose(points);

    FILE *cars = fopen("cars.txt", "rb");

    while (!playingGame.cars.empty()) {
        playingGame.cars.pop();
    }

    Car car;
    while (fread(&car, sizeof(Car), 1, cars)) {
        playingGame.cars.push(car);
    }

    fclose(cars);

    pthread_mutex_unlock(&playingGame.mutexFile);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    playingGame.IsGameRunning = true;
    playingGame.IsSaveCliked = false;
    playingGame.moveSpeed = ISPEED - (playingGame.level - 1) * DRATESPEED;

    initWindow();

    printWindow();
    drawCar(playingGame.current, 2, 1);

    pthread_t thread;
    pthread_create(&thread, NULL, newGame, NULL);
    pthread_join(thread, NULL);
}

void initGame()
{
    playingGame.cars = queue<Car>();
    playingGame.counter =IDSTART;
    pthread_mutex_t mutexFile;
    pthread_mutex_init(&playingGame.mutexFile, NULL);
    playingGame.mutexFile = mutexFile;
    playingGame.level = 1;
    playingGame.moveSpeed = ISPEED;
    playingGame.points = 0;
    playingGame.IsSaveCliked = false;
    playingGame.IsGameRunning = true;
    playingGame.current.ID = IDSTART-1;
    playingGame.current.height = MINH;
    playingGame.current.width = MINW;
    playingGame.current.speed = SPEEDOFCAR;
    playingGame.current.x = XOFCAR;
    playingGame.current.y = YOFCAR;
    playingGame.current.clr = COLOROFCAR;
    playingGame.current.chr = '*';
}

void* enqueueCar(void*) {
    while (playingGame.IsGameRunning) {
        if (playingGame.cars.size() < maxCarNumber) {
            Car newCar;
            newCar.ID = ++playingGame.counter;
            newCar.width = getRandomNumber(5, 7);

            bool validPosition = false;
            while (!validPosition) {
                newCar.x = getRandomNumber(5, 90);
                if (newCar.x + newCar.width < lineX - 2 || newCar.x > lineX + 2) {
                    validPosition = true;
                }
            }

            newCar.y = getRandomNumber(-10, 0);
            newCar.height = getRandomNumber(5, 7);
            newCar.speed = (newCar.height / 2);
            newCar.clr = getRandomNumber(1, 4);
            newCar.chr = getRandomCarChr();
            newCar.isExist = true;

            playingGame.cars.push(newCar);
        }
        sleep(getRandomNumber(2,4));
    }
    return NULL;
}

void *newGame(void *) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    printWindow();
    drawCar(playingGame.current, 2, 1);
    pthread_t carThread, moveCarsThread;
    pthread_create(&carThread, NULL, enqueueCar, NULL);
    pthread_create(&moveCarsThread, NULL, moveCars, NULL);

    int key;

    while (playingGame.IsGameRunning) {
        key = getch();
        if (key != ERR) {
            if (key == playingGame.leftKey) {
                if (!(playingGame.current.x <= 4)) {
                    drawCar(playingGame.current, 1, 1);
                    playingGame.current.x -= playingGame.current.speed;
                    drawCar(playingGame.current, 2, 1);
                }
            } else if (key == playingGame.rightKey) {
                if (!(playingGame.current.x + playingGame.current.width >= (wWidth - 5))){
                    drawCar(playingGame.current, 1, 1);
                    playingGame.current.x += playingGame.current.speed;
                    drawCar(playingGame.current, 2, 1);
                }
            }else if(key==SAVEKEY){
                saveGame();
            }else if (key == ESC) {
                playingGame.IsGameRunning = false;
            }
        }
        usleep(GAMESLEEPRATE);
    }

    pthread_join(carThread, NULL);
    pthread_join(moveCarsThread, NULL);
    endwin();

    return NULL;
}

void initWindow()
{
    initscr();            // initialize the ncurses window
    start_color();        // enable color manipulation
    keypad(stdscr, true); // enable the keypad for the screen
    nodelay(stdscr, true);// set the getch() function to non-blocking mode
    curs_set(0);          // hide the cursor
    cbreak();             // disable line buffering
    noecho();             // don't echo characters entered by the user
    clear();              // clear the screen
    sleep(1);
}
void printWindow()
{
    for (int i = 1; i < wHeight - 1; ++i) {
        //mvprintw: Used to print text on the window, paramters order: y , x , string
        mvprintw(i, 2, "*"); //left side of the road
        mvprintw(i, 0, "*");
        mvprintw(i, wWidth - 1, "*");// right side of the road
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) { //line in the middle of the road
        mvprintw(i, lineX, "#");
    }
    int treeX = wWidth + 5;
    int treeY = 5;

    drawTree(treeY, treeX);
    drawTree(treeY + 10, treeX);
    drawTree(treeY + 20, treeX);
    mvprintw(POINTY, POINTX, "Points: %d", playingGame.points);
    refresh();
}

void updatePoints(int points) {
    playingGame.points += points;

    if (playingGame.points / (levelBound * playingGame.level) == 1){
        if(playingGame.level != MAXSLEVEL)
        {
            playingGame.level++;
            playingGame.moveSpeed -= DRATESPEED;
        }
    }

    mvprintw(POINTY, POINTX, "Points: %d", playingGame.points);
    refresh();
}

void drawCar(Car c, int type, int direction )
{
    //If the user does not want to exit the game and the game continues
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {
        init_pair(c.ID, c.clr, 0);// Creates a color pair: init_pair(short pair ID, short foregroundcolor, short backgroundcolor);
        //0: Black (COLOR_BLACK)
        //1: Red (COLOR_RED)
        //2: Green (COLOR_GREEN)
        //3: Yellow (COLOR_YELLOW)
        //4: Blue (COLOR_BLUE)
        attron(COLOR_PAIR(c.ID));//enable color pair
        char drawnChar;
        if (type == 1 )
            drawnChar = ' '; // to remove car
        else
            drawnChar= c.chr; //  to draw char
        //mvhline: used to draw a horizontal line in the window
        //shallow. : mvhline(int y, int x, chtype ch, int n)
        //y: horizontal coordinate
        //x: vertical coordinate
        //ch: character to use
        //n: Length of the line
        mvhline(c.y, c.x, drawnChar, c.width);// top line of rectangle
        mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); //bottom line of rectangle
        if(direction == 0) // If it is any car on the road
            mvhline(c.y + c.height, c.x, drawnChar, c.width);
        else //player's card
            mvhline(c.y -1, c.x, drawnChar, c.width);
        //mvvline: used to draw a vertical line in the window
        //shallow. : mvhline(int y, int x, chtype ch, int n)
        //y: horizontal coordinate
        //x: vertical coordinate
        //ch: character to use
        //n: Length of the line
        mvvline(c.y, c.x, drawnChar, c.height); //left line of rectangle
        mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); //right line of rectangle
        char text[5];
        if (type == 1 )
            sprintf(text,"  "); //to remove point
        else
            sprintf(text,"%d",c.height * c.width); // to show car's point in rectangle
        mvprintw(c.y+1, c.x +1, text);// display car's point in rectangle
        attroff(COLOR_PAIR(c.ID));// disable color pair
    }
}

void printMenu(int highlight)
{
    int x = MENUX, y = MENUY;
    int column = 0;
    clear();
    for (int i = 0; i < 6; i++) {
        if (i > 0 && i % 10 == 0) {
            column++;
            x = 10 + column * 20;
            y = 5;
        }
        if (i == highlight) {
            attron(COLOR_PAIR(2));
            mvprintw(y, x - 2, "->");
            mvprintw(y, x, mainMenu[i]);
            attroff(COLOR_PAIR(2));
        } else {
            attron(COLOR_PAIR(1));
            mvprintw(y, x, mainMenu[i]);
            attroff(COLOR_PAIR(1));
        }
        y += 2;
    }
    refresh();
}

void showInstructions()
{
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(5, 10, "< or A: Move the car to the left");
    mvprintw(7, 10, "> or D: Move the car to the right");
    mvprintw(9, 10, "ESC: Exits the game without saving");
    mvprintw(11, 10, "S: Saves and exits the game");
    attroff(COLOR_PAIR(1));
    refresh();
    sleep(5);
}
void showPoints()
{
    clear();
    FILE *file = fopen("points.txt", "r");
    if(file!=NULL){
        char buffer[256];
        int gameNumber = 1;
        int x = 3, y = 3;
        int maxY = LINES - 1;

        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            attron(COLOR_PAIR(1));
            mvprintw(y, x, "Game %d: %s", gameNumber++, buffer);
            y += 2;

            if (y + 2 > maxY) {
                x += 20;
                y = 3;
            }
            attroff(COLOR_PAIR(1));
        }
        refresh();
        sleep(5);
    }else{
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        attron(COLOR_PAIR(1));
        mvprintw(5, 10, "You haven't played a game yet!");
        attroff(COLOR_PAIR(1));
        refresh();
        sleep(2);
        printMenu(0);
    }
}

void showSettings()
{
    int highlight = 0;
    int ch;

    while (true) {
        clear();
        int x = 10, y = 5;
        for (int i = 0; i < 2; i++) {
            if (i == highlight) {
                attron(COLOR_PAIR(2));
                mvprintw(y, x - 2, "->");
                mvprintw(y, x, settingMenu[i]);
                attroff(COLOR_PAIR(2));
            } else {
                attron(COLOR_PAIR(1));
                mvprintw(y, x, settingMenu[i]);
                attroff(COLOR_PAIR(1));
            }
            y += 2;
        }
        refresh();

        ch = getch();
        usleep(200000);
        switch (ch) {
            case KEY_UP:
                highlight = (highlight == 0) ? 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == 1) ? 0 : highlight + 1;
                break;
            case 10:
                if (highlight == 0) {
                    playingGame.leftKey = leftKeyArrow;
                    playingGame.rightKey = RightKeyArrow;
                } else {
                    playingGame.leftKey = leftKeyA;
                    playingGame.rightKey = RightKeyD;
                }
                refresh();
                sleep(2);
                return;
            default:
                break;
        }
    }
}

void* moveCars(void*) {
    while (playingGame.IsGameRunning) {
        pthread_mutex_lock(&playingGame.mutexFile);

        queue<Car> tempQueue;
        while (!playingGame.cars.empty()) {
            Car car = playingGame.cars.front();
            playingGame.cars.pop();
            drawCar(car, 1, 0);
            car.y += car.speed;
            if (car.y < EXITY) {
                drawCar(car, 2, 0);
                tempQueue.push(car);
            } else {
                int points = car.height * car.width;
                updatePoints(points);
            }
        }
        playingGame.cars = tempQueue;

        checkCollision();
        pthread_mutex_unlock(&playingGame.mutexFile);
        usleep(playingGame.moveSpeed);
    }
    return NULL;
}


int getRandomNumber(int sValue, int eValue){
    return sValue + rand() % (eValue - sValue + 1);
}

char getRandomCarChr(){
    int rnd = getRandomNumber(1,3);
    if (rnd==1)
        return '*';
    else if(rnd==2)
        return '+';
    else
        return '#';
}

void drawTree(int y, int x) {
    attron(COLOR_PAIR(1));
    mvprintw(y, x, "   * ");
    mvprintw(y + 1, x, "  * * ");
    mvprintw(y + 2, x, " * * * ");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    mvprintw(y + 3, x, "   # ");
    mvprintw(y + 4, x, "   # ");
    attroff(COLOR_PAIR(2) );
}

void checkCollision() {
    queue<Car> tempQueue = playingGame.cars;
    while (!tempQueue.empty()) {
        Car car = tempQueue.front();
        tempQueue.pop();
        if (car.isExist) {
            if (car.y + car.height >= playingGame.current.y &&
                car.y <= playingGame.current.y + playingGame.current.height &&
                car.x + car.width >= playingGame.current.x &&
                car.x <= playingGame.current.x + playingGame.current.width) {
                playingGame.IsGameRunning = false;
                mvprintw(EXITY, lineX + lineLEN / 2, "Game Over!");
                FILE *points = fopen("points.txt", "a+");
                fprintf(points, "%d\n", playingGame.points);
                fclose(points);
                refresh();
                usleep(1000000);
                getch();
                printMenu(0);
            }
        }
        break;
    }
}
