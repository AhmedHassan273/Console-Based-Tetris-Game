#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <vector>
#include <Windows.h>
using namespace std;

#define rep(i, n, x) for(int i=n;i<x;i++)
const int current_column = 2, current_row = 4;
const int sizeof_col = 20, sizeof_row = 25;
const int logo_size = 100;


struct Game {
    char grid[sizeof_row][sizeof_col];
    int Color = 14;
    int position[current_row][current_column];
    map<int, vector<vector<int> > > block;
    map<int, int> LevelScore;
    int row = 4, col = 2;
    int select = 1;
    int total_score = 0, Difficulty = 9, Levels = 1;
    int UserChoice = 0;
    string Menu[4];
};

class collisionTest {
    // collision test
public:
    bool Downcollision(Game);

    bool Rightcollision(Game);

    bool Leftcollision(Game);

    bool Rotationcollision(Game);
};

class Move {
public:
    // blocks movement
    void move_left(Game &, collisionTest);

    void move_right(Game &, collisionTest);

    void move_down(Game &, collisionTest, bool &);

    void Recover_Grid(Game &);

    void set_pawn(Game &);

    bool isvalid(Game);

private:
    void shift_Left(Game &);

    void shift_Right(Game &);

    void shift_Down(Game &);
};

class Score {
public:
    // score managment
    void ApplyGravity(Game &, int);

    int iscompleteline(Game);

    bool isGameOver(Game);
};

/// the game
void runTetris();

// about game's map and positions
void SetCursorToPosition(int, int, int);

void ClearScreen(Game);

void gotoxy(int, int);

void SetColor(int);

class Map {
public:
    // blocks positions, rotates, defult map
    void set_ALL_pawns(Game &, int, int);

    void ChoosePawn(Game &);

    void set_grid(Game &);

    void ShowNextPawn(Game);
};

// on stat
class Menu {
public:
    Menu();

protected:
    void StartMenu(Game);
};