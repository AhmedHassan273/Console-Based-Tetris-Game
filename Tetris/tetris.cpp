#include "tetris.h"

// on start
Menu::Menu() {
	start:;
	system("cls");
	system("color 0F");
	// read from TetrisHeadline.txt and print the game's logo
	ifstream Mylogo;
	Mylogo.open("TetrisHeadline.txt");
	string logo[logo_size];
	int printed_size = 0;
	while (!Mylogo.eof()) {
		getline(Mylogo, logo[printed_size++]);
	}
	rep(i, 0, printed_size) {
		gotoxy(25, 10 + i);
		rep(j, 0, logo[i].size()) {
			if (logo[i][j] != ' ') {
				// colot the logo's letters to specific colors
				if (logo[i][j] == 'T') SetColor(13);
				else if (logo[i][j] == 'E') SetColor(12);
				else if (logo[i][j] == 'R') SetColor(10);
				else if (logo[i][j] == 'I') SetColor(9);
				else SetColor(15);
				cout << char(219);
			}
			else
				cout << ' ';
		}
		cout << endl;
	}
	// end of logo reading/printing

	Game event;
	// display main menu
	StartMenu(event);
	while (true) {
		// move down select
		if (GetAsyncKeyState(VK_DOWN)) {
			// stay inside menu's bounds
			if (event.UserChoice + 1 >= 4)
				continue;
			event.UserChoice += 1;
			StartMenu(event);
			Sleep(100);
		}
		// move up select
		if (GetAsyncKeyState(VK_UP)) {
			// stay inside menu's bounds
			if (event.UserChoice - 1 < 0)
				continue;
			event.UserChoice -= 1;
			StartMenu(event);
			Sleep(100);
		}
		// a choice has been choosen
		if (GetAsyncKeyState(VK_RETURN)) {
			system("cls");
			// if choice is play mode then start the game
			if (event.UserChoice == 0) {
				runTetris();
				return;
			}
				// else if choice is controls then show contols.txt on screen
			else if (event.UserChoice == 1) {
				ifstream MyFile;
				MyFile.open("controls.txt");
				while (!MyFile.eof()) {
					string instructions;
					getline(MyFile, instructions);
					cout << instructions << endl;
				}
				while (true) {
					// to return to main menu user must push backspace
					if (GetAsyncKeyState(VK_BACK)) {
						system("cls");
						goto start;
					}
				}
			}
				// else if choice is credits then show credits.txt
			else if (event.UserChoice == 2) {
				ifstream MyFile;
				MyFile.open("credits.txt");
				while (!MyFile.eof()) {
					string credits;
					getline(MyFile, credits);
					cout << credits << endl;
				}
				while (true) {
					// to return to main menu user must push backspace
					if (GetAsyncKeyState(VK_BACK)) {
						system("cls");
						goto start;
					}
				}
			}
				// else the user have choosed to quit the game
			else if (event.UserChoice == 3) {
				system("Color 0F");
				SetCursorToPosition(20, 10, 14);
				cout << "Thank You For Playing My Tetris Game";
				SetCursorToPosition(20, 13, 14);
				cout << "Come Back Later ";
				SetCursorToPosition(20, 20, 14);
				return;
				// end game
			}
			return;
		}
	}
}

void Menu::StartMenu(Game game) {
	// Display the Main Menu
	int ShifTs = 0;
	// user's choices in main menu
	string MainMenu[] = {"Play Mode", "Controls", "Credits", "Quit"};
	rep(i, 0, game.row) {
		game.Menu[i] = MainMenu[i];
		SetCursorToPosition(50, i + 20 + ShifTs, 14);
		if (game.UserChoice == i)
			SetCursorToPosition(50, i + 20 + ShifTs, 23);
		cout << game.Menu[i];
		ShifTs += 2;
	}
}


/*  blocks movement
 moving process [recover - shift - check if valid (Down movement only) - set]
 position[][] is 2D array with 4 rows and 2 columns , each row represent a point with 2 columns ( i , j ) 
 shifting to right means the j will be j + 1
 shifting to left means the j will be j - 1
 shifting down means the i will be i + 1
 shifting up means the i will be i - 1 (but we won't use it in this game)
*/
void Move::move_down(Game &game, collisionTest test, bool &get_out) {
	// is block reached a dead end we stop
	if (test.Downcollision(game)) {
		get_out = true;
		return;
	}
	Recover_Grid(game);

	shift_Down(game);

	if (!isvalid(game)) {

		set_pawn(game);

		ClearScreen(game);

		get_out = true;
		return;
	}

	set_pawn(game);
	// re-print the screen
	ClearScreen(game);
}

void Move::move_right(Game &game, collisionTest test) {
	Recover_Grid(game);

	shift_Right(game);

	set_pawn(game);

	ClearScreen(game);
}

void Move::move_left(Game &game, collisionTest test) {
	Recover_Grid(game);

	shift_Left(game);

	set_pawn(game);

	ClearScreen(game);
}

void Move::Recover_Grid(Game &game) {
	// re-set the block's position to blank spaces before moving/shifting/sitting 
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1];
		game.grid[I][J] = ' ';
	}
}

void Move::shift_Left(Game &game) {
	// j = j - 1
	rep(i, 0, game.row)
		game.position[i][1]--;
}

void Move::shift_Right(Game &game) {
	// j = j + 1
	rep(i, 0, game.row)
		game.position[i][1]++;
}

void Move::shift_Down(Game &game) {
	// i = i + 1
	rep(i, 0, game.row)
		game.position[i][0]++;
}

void Move::set_pawn(Game &game) {
	// set the current block's positions on the grid to be visualised as char(219)
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1];
		game.grid[I][J] = char(219);
	}
}

bool Move::isvalid(Game game) {
	// just to check if block has reached the bottom
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1];
		if (game.grid[I][J] != ' ')
			return false;

	}
	return true;
}


/* score managment
  score gets higher when a complete line exists 
  then we apply gravity and then +1000 the score
  if the game is over the score stops and GAME OVER appears
  ( game stops if user choosed to )
*/
void Score::ApplyGravity(Game &game, int CompleteRow) {
	/*
	  to apply gravity the idea was to assign each point to it's top point from down-up
	  starting from the complete line ofcourse 
	*/
	for (int j = 1; j < sizeof_col - 1; j++) {
		// erasing the complete line
		game.grid[CompleteRow][j] = ' ';
	}
	for (int i = CompleteRow; i > 1; i--) {
		for (int j = sizeof_col - 1; j > 0; j--) {
			// the assign of each point ot it's top point
			game.grid[i][j] = game.grid[i - 1][j];
		}
	}
}

int Score::iscompleteline(Game game) {
	// if a line is full of blocks
	for (int i = sizeof_row - 2; i >= 1; i--) {
		// blocks counter
		int count = 0;
		for (int j = 1; j < sizeof_col - 1; j++) {
			// if is block then counter + 1
			if (game.grid[i][j] == char(219))
				count++;
			// is space then there's no complete line here and we stop
			if (game.grid[i][j] == ' ')
				break;
		}
		// if counter makes the line full of blocks then returns the i th row which is complete
		if (count == sizeof_col - 2)
			return i;
	}
	// no lines has been found
	return 0;
}

bool Score::isGameOver(Game game) {
	// if and only if the top rows have blocks in them then the game is over
	rep(i, 0, sizeof_row) {
		if (game.grid[2][i] == char(219) || game.grid[3][i] == char(219))
			return 1;
	}
	return 0;

}


/// the game
void runTetris() {
	Game event;
	collisionTest test;
	Move go;
	Score ValidState;
	Map Action;

	// set and print defult grid
	Action.set_grid(event);
	ClearScreen(event);

	// for next block choose
	bool next = 0;
	srand(time(NULL));

	// set current choose and next choose
	int next_choice = rand() % 7;
	int choice = rand() % 7;
	// game loop
	while (true) {
		// set our color of current block
		event.Color = 3 + rand() % 115;

		// some colors are not allowed
		if (event.Color % 16 == 0)
			event.Color = 12;

		// initial spawn position
		int X = 3, Y = (sizeof_row / 2) - 2;

		// if there's a complete line
		int CompleteLine = ValidState.iscompleteline(event);
		while (CompleteLine) {
			// erase it and apply gravity
			ValidState.ApplyGravity(event, CompleteLine);
			// update score
			event.total_score += 1000;
			CompleteLine = ValidState.iscompleteline(event);
		}
		// set the block's position
		Action.set_ALL_pawns(event, X, Y);

		// our initial list of blocks in int[]
		int current_choose[] = {1, 5, 9, 13, 17, 21, 25}, bound_of_choosen_pawn = 0;

		// determine how next block would be choosen
		if (!next) {
			bound_of_choosen_pawn = choice;
			event.select = current_choose[next_choice] + 1;
			Action.ChoosePawn(event);
			next = !next;
			choice = rand() % 7;
		}
		else {
			bound_of_choosen_pawn = next_choice;
			event.select = current_choose[choice] + 1;
			Action.ChoosePawn(event);
			next = !next;
			next_choice = rand() % 7;
		}
		// show the next block
		Action.ShowNextPawn(event);

		// select our block from the list
		event.select = current_choose[bound_of_choosen_pawn] + 1;

		// set position[] to block's
		Action.ChoosePawn(event);
		if (ValidState.isGameOver(event)) {
			// if blocks reached the ceiling the game ends
			ClearScreen(event);
			SetColor(12);
			printf_s("\n\n\n\t\tGame Over\n");
			printf_s("\n\n\t\tFinal Score : %d\n\n\n\n\n", event.total_score);
			printf_s("\n\n\t\tback To return to game menu\n");
			printf_s("\n\n\t\tEnter To quit\n");

			// user choice - continue to menu or quit game
			while (true) {
				if (GetAsyncKeyState(VK_BACK)) {
					Menu Start;
					return;
				}
				if (GetAsyncKeyState(VK_RETURN)) {
					system("Color 0F");
					SetCursorToPosition(20, 10, 14);
					cout << "Thank You For Playing My Tetris Game";
					SetCursorToPosition(20, 13, 14);
					cout << "Come Back Later ";
					SetCursorToPosition(20, 20, 14);
					return;
				}
			}
			Sleep(1000);
		}
		// if game is over final level - 5 - then game is finished
		if (event.Levels > 5) {
			system("Color 0F");
			system("cls");
			SetCursorToPosition(30 + 5, 20, 14);
			cout << "Congratulations ! You Have Finished The Game , You'r My Hero";

			SetCursorToPosition(30 + 5, 22, 14);
			printf_s("\t\tback To return to game menu");

			SetCursorToPosition(30 + 5, 24, 14);
			printf_s("\t\tEnter To quit");

			// user choice - continue to menu or quit game
			while (true) {
				if (GetAsyncKeyState(VK_BACK)) {
					Menu Start;
					return;
				}
				if (GetAsyncKeyState(VK_RETURN)) {
					system("cls");
					system("Color 0F");

					SetCursorToPosition(20, 10, 14);
					cout << "\t\tThank You For Playing My Tetris Game";

					SetCursorToPosition(20, 13, 14);
					cout << "\t\tCome Back Later ";

					SetCursorToPosition(20, 20, 14);
					return;
				}
			}
		}

		// if player has reached the level's required score then go to next level
		if (event.total_score >= event.LevelScore[event.Levels]) {
			system("Color 0F");
			system("cls");

			SetCursorToPosition(30 + 5, 20, 14);
			cout << "Congratulations ! You Have Reached The Next Level very Nice";

			SetCursorToPosition(30 + 10 + 5, 20 + 1, 14);
			cout << "press Enter To continue to next level";

			event.Levels += 1;

			// user confirmation
			while (!GetAsyncKeyState(VK_RETURN));

			Action.set_grid(event);

			event.total_score = 0;
			// faster falling
			event.Difficulty -= 10;

			if (event.Difficulty <= 0)
				event.Difficulty += 10;
			system("cls");
			continue;
		}
		// set block on the grid
		go.set_pawn(event);

		while (true) {
			/*
			 in every move we should update X and Y so that every block appear/change in the movement 
			 if we don't the block ill always show in the top of the grid and stay there until it changes
			*/
			Sleep(event.Difficulty);

			// fall while time past
			if (!test.Downcollision(event)) {
				bool get_out = 0;
				go.move_down(event, test, get_out);
				if (get_out)
					break;
				X++;
				Action.set_ALL_pawns(event, X, Y);
			}
			else
				break;
			// down pressed
			if (GetAsyncKeyState(VK_DOWN)) {
				bool get_out = 0;
				go.move_down(event, test, get_out);
				if (get_out)
					break;
				X++;
				Action.set_ALL_pawns(event, X, Y);
			}
			// right pressed
			if (GetAsyncKeyState(VK_RIGHT)) {
				if (test.Rightcollision(event))
					continue;
				go.move_right(event, test);
				Y++;
				Action.set_ALL_pawns(event, X, Y);
			}
			// left pressed
			if (GetAsyncKeyState(VK_LEFT)) {
				if (test.Leftcollision(event))
					continue;
				go.move_left(event, test);
				Y--;
				Action.set_ALL_pawns(event, X, Y);
			}
			// rotate 
			if (GetAsyncKeyState(VK_UP)) {
				int prev_select = event.select;
				go.Recover_Grid(event);
				event.select += 1;

				// check if not out of bound - the block should rotate not change - !
				if (event.select > current_choose[bound_of_choosen_pawn] + 3)
					event.select = current_choose[bound_of_choosen_pawn];

				Action.ChoosePawn(event);

				// if rotation is not valid we don't apply rotation
				if (test.Rotationcollision(event)) {
					event.select = prev_select;
					Action.ChoosePawn(event);
					if (test.Downcollision(event))
						go.set_pawn(event);
					else
						continue;
				}
			}
			// if game is paused
			if (GetAsyncKeyState(VK_ESCAPE)) {
				system("Color 0F");
				ClearScreen(event);

				SetCursorToPosition(15, 40, 11);
				cout << "Game is paused";

				SetCursorToPosition(15, 42, 11);
				cout << "Press Enter To Continue The Game\n";

				SetCursorToPosition(15, 44, 11);
				cout << "Press Backspace To Return To Game Menu\n";
				while (true) {
					if (GetAsyncKeyState(VK_BACK)) {
						Menu Start;
						return;
					}
					if (GetAsyncKeyState(VK_RETURN)) {
						break;
					}
				}

				SetCursorToPosition(15, 40, 11);
				cout << "\t\t\t\t\t\t\t\t\t";

				SetCursorToPosition(15, 42, 11);
				cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n";

				SetCursorToPosition(15, 44, 11);
				cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n";
			}

			//set the block to this place - drop it here - !
			if (GetAsyncKeyState(VK_SPACE)) {
				bool get_out = 0;

				while (!test.Downcollision(event))
					go.move_down(event, test, get_out);

				if (get_out)
					break;
			}
		}
		// update score for each drop
		event.total_score += 30;
	}
}


// about game's map and positions
void SetCursorToPosition(int x, int y, int _color) {
	//PlaySound(TEXT("recycle.wav"), NULL, SND_FILENAME);

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = {x, y};
	SetConsoleCursorPosition(out, pos);
	SetConsoleTextAttribute(out, _color);
}

void gotoxy(int x, int y) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = {x, y};
	SetConsoleCursorPosition(out, pos);
}

void SetColor(int color) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(out, color);
}

void ClearScreen(Game game) {
	// if player has reached level > 5 then the game is finished
	if (game.Levels > 5) {
		system("cls");

		SetCursorToPosition(30 + 5, 20, 14);
		cout << "Congratulations ! You Have Finished The Game";
		printf_s("\t\tback To return to game menu\n");
		printf_s("\t\tEnter To quit\n");
		while (true) {
			// if user choosed to play a new game then he returns to the main menu to choose a new game
			if (GetAsyncKeyState(VK_BACK)) {
				Menu Start;
				return;
			}
			// else if user chooosed to quit the game then the game stops
			if (GetAsyncKeyState(VK_RETURN)) {
				system("cls");
				system("Color 0F");

				SetCursorToPosition(20, 10, 14);
				cout << "Thank You For Playing Da Tetris Game";

				SetCursorToPosition(20, 13, 14);
				cout << "Come Back Later For More";

				SetCursorToPosition(20, 20, 14);
				// end of game
				return;
			}
		}
		return;
	}

	// else if game is not finished then show current level and current level on the side
	SetCursorToPosition(15 + 23 + 6, 15, 14);
	cout << " Level ";
	if (game.Levels != 5)
		cout << game.Levels;
	else
		cout << game.Levels << " [Final]";

	SetCursorToPosition(15 + 23 + 6, 15 + 1, 14);
	printf_s("Score : %d out of [ %d ]", game.total_score, game.LevelScore[game.Levels] - game.total_score);

	map <pair<int, int>, bool> colored;
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1];
		colored[make_pair(I, J)] = true;
	}

	/*if the user is at level [2,3] then there will be some changes since this is the final level
		 by changes we mean the change of gravity (blocks falls side right,down or left instead of falling down)
		 the contols stays the same to make more interesting/confusing ( confused ? you will be)
		 */
	if (game.Levels == 4)
		// at level 2 the map simply falls up !
	{
		for (int i = 15, ipos = sizeof_row - 1; ipos >= 0; i++, ipos--) {
			gotoxy(15 + 1, i);
			for (int j = 15, jpos = 0; jpos < sizeof_col; j++, jpos++) {
				/*
				I use different colors for the blocks and grid's edges
				as for the blanks ( empty grid's poistions ) i color it with dark black
				*/
				if (game.grid[ipos][jpos] == char(177))
					SetColor(17);
				else if (game.grid[ipos][jpos] == char(219)) {
					if (colored[make_pair(ipos, jpos)])
						SetColor(game.Color);
					else
						SetColor(119);
				}
				else
					SetColor(0);
				cout << game.grid[ipos][jpos];
			}
			cout << endl;
		}
		return;
	}
	if (game.Levels == 5)
		// at level 3 the map simply falls right
	{
		for (int i = 15, ipos = 0; ipos < sizeof_col; i++, ipos++) {
			gotoxy(15 + 1, i);
			for (int j = 15, jpos = 0; jpos < sizeof_row; j++, jpos++) {
				/*
				 I use different colors for the blocks and grid's edges
				 as for the blanks ( empty grid's poistions ) i color it with dark black
				 */
				if (game.grid[jpos][ipos] == char(177))
					SetColor(17);
				else if (game.grid[jpos][ipos] == char(219)) {
					if (colored[make_pair(jpos, ipos)])
						SetColor(game.Color);
					else
						SetColor(119);
				}
				else
					SetColor(0);
				cout << game.grid[jpos][ipos];
			}
			cout << endl;
		}
		return;
	}
	// print the defult map
	for (int i = 15, ipos = 0; ipos < sizeof_row; i++, ipos++) {
		gotoxy(15 + 1, i);
		for (int jpos = 0; jpos < sizeof_col; jpos++) {
			/*
			I use different colors for the blocks and grid's edges
			as for the blanks ( empty grid's poistions ) i color it with dark black
			*/
			if (game.grid[ipos][jpos] == char(177))
				SetColor(17);
			else if (game.grid[ipos][jpos] == char(219)) {
				if (colored[make_pair(ipos, jpos)])
					SetColor(game.Color);
				else
					SetColor(119);
			}
			else
				SetColor(0);
			cout << game.grid[ipos][jpos];
		}
		cout << endl;
	}
}


// blocks positions, rotates, defult map
void Map::set_ALL_pawns(Game &game, int X, int Y) {
	/*
	 this is every block we have in the game represented as (i,j) coordinates on the grid
	 for every block it has 4 rotates so we have 4 different ( mostly different ) positions for each block + it's rotates
	*/

	// A
	game.block[1] = {{X, Y},
					 {X, Y + 1},
					 {X, Y + 2},
					 {X, Y + 3}};
	game.block[2] = {{X - 2, Y},
					 {X - 1, Y},
					 {X,     Y},
					 {X + 1, Y}};
	game.block[3] = {{X, Y},
					 {X, Y + 1},
					 {X, Y + 2},
					 {X, Y + 3}};
	game.block[4] = {{X - 2, Y},
					 {X - 1, Y},
					 {X,     Y},
					 {X + 1, Y}};

	// B
	game.block[5] = {{X,     Y},
					 {X,     Y + 1},
					 {X + 1, Y},
					 {X + 1, Y + 1}};
	game.block[6] = {{X,     Y},
					 {X,     Y + 1},
					 {X + 1, Y},
					 {X + 1, Y + 1}};
	game.block[7] = {{X,     Y},
					 {X,     Y + 1},
					 {X + 1, Y},
					 {X + 1, Y + 1}};
	game.block[8] = {{X,     Y},
					 {X,     Y + 1},
					 {X + 1, Y},
					 {X + 1, Y + 1}};

	// C
	game.block[9] = {{X,     Y},
					 {X,     Y + 1},
					 {X - 1, Y + 1},
					 {X - 1, Y + 2}};
	game.block[10] = {{X,     Y},
					  {X + 1, Y},
					  {X + 1, Y + 1},
					  {X + 2, Y + 1}};
	game.block[11] = {{X,     Y},
					  {X,     Y + 1},
					  {X - 1, Y + 1},
					  {X - 1, Y + 2}};
	game.block[12] = {{X,     Y},
					  {X + 1, Y},
					  {X + 1, Y + 1},
					  {X + 2, Y + 1}};

	// D
	game.block[13] = {{X,     Y},
					  {X,     Y + 1},
					  {X + 1, Y + 1},
					  {X + 1, Y + 2}};
	game.block[14] = {{X,     Y},
					  {X + 1, Y},
					  {X + 1, Y - 1},
					  {X + 2, Y - 1}};
	game.block[15] = {{X,     Y},
					  {X,     Y + 1},
					  {X + 1, Y + 1},
					  {X + 1, Y + 2}};
	game.block[16] = {{X,     Y},
					  {X + 1, Y},
					  {X + 1, Y - 1},
					  {X + 2, Y - 1}};

	// E
	game.block[17] = {{X,     Y},
					  {X,     Y - 1},
					  {X + 1, Y - 1},
					  {X + 2, Y - 1}};
	game.block[18] = {{X,     Y},
					  {X,     Y + 1},
					  {X,     Y + 2},
					  {X + 1, Y + 2}};
	game.block[19] = {{X,     Y},
					  {X + 1, Y},
					  {X + 2, Y},
					  {X + 2, Y - 1}};
	game.block[20] = {{X,     Y},
					  {X + 1, Y},
					  {X + 1, Y + 1},
					  {X + 1, Y + 2}};

	// F
	game.block[21] = {{X,     Y},
					  {X,     Y + 1},
					  {X + 1, Y + 1},
					  {X + 2, Y + 1}};
	game.block[22] = {{X,     Y},
					  {X,     Y + 1},
					  {X,     Y + 2},
					  {X - 1, Y + 2}};
	game.block[23] = {{X,     Y},
					  {X + 1, Y},
					  {X + 2, Y},
					  {X + 2, Y + 1}};
	game.block[24] = {{X,     Y},
					  {X + 1, Y},
					  {X,     Y + 1},
					  {X,     Y + 2}};

	// G
	game.block[25] = {{X,     Y},
					  {X,     Y + 1},
					  {X - 1, Y + 1},
					  {X,     Y + 2}};
	game.block[26] = {{X,     Y},
					  {X + 1, Y},
					  {X + 1, Y + 1},
					  {X + 2, Y}};
	game.block[27] = {{X,     Y},
					  {X,     Y + 1},
					  {X + 1, Y + 1},
					  {X,     Y + 2}};
	game.block[28] = {{X,     Y},
					  {X + 1, Y},
					  {X + 1, Y - 1},
					  {X + 2, Y}};

	// score to pass levels [ 5 ]
	// you must have at least score of game.LevelScore[n] to pass level n
	game.LevelScore[1] = 4000;
	game.LevelScore[2] = 6000;
	game.LevelScore[3] = 10000;
	game.LevelScore[4] = 6000;
	game.LevelScore[5] = 6000;
}

void Map::set_grid(Game &game) {
	// set our main grid ( full with blank , edges are the frame )
	memset(game.grid, ' ', sizeof(game.grid));
	rep(i, 0, sizeof_row) {
		game.grid[i][0] = char(177);
		game.grid[sizeof_row - 1][i] = char(177);
		if (i < sizeof_col)
			game.grid[0][i] = char(177);
	}
	rep(i, 0, sizeof_row) {
		game.grid[i][sizeof_col - 1] = char(177);
	}
}

void Map::ChoosePawn(Game &game) {
	// use this function to choose our block from Game::block and then Assign it to our block's Game::position[][]
	// we determine the block by the Game::select
	for (int i = 0; i < game.row; i++) {
		for (int j = 0; j < game.col; j++) {
			game.position[i][j] = game.block[game.select][i][j];
		}
	}
}

void Map::ShowNextPawn(Game game) {
	// next_shape[][] of boolean is to contain the form of our block
	bool next_shape[7][7];

	// memset the [][] boolean with false as if it's blanks
	memset(next_shape, false, sizeof(next_shape));
	rep(i, 0, game.row) {
		// only the positions which belongs to the block and set it to true on [][] boolean
		int I = game.position[i][0];
		int J = game.position[i][1];
		next_shape[I][J] = true;
	}

	// print the block after processed
	rep(i, 0, 7) {
		SetCursorToPosition(15 + 23 + 7, 5 + 15 + i, 11);
		rep(j, 0, 7) {
			if (next_shape[i][j])
				cout << char(219);
			else
				cout << ' ';
		}
		cout << endl;
	}

	SetCursorToPosition(15 + 23 + 7, 5 + 15, 14);
	cout << "Next Block : ";
}


/* 
 collision test
 here we just check for every right, left and down point for our current block's position ( only if possible ) 
 we don't want to check the position it self only it's area around it and we don't want to check any point the belongs to the
 block's position it self
*/
bool collisionTest::Downcollision(Game game) {
	map <pair<int, int>, bool> shape;
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1];
		shape[make_pair(I, J)] = true;
	}
	rep(i, 0, game.row) {
		int I = game.position[i][0] + 1;
		int J = game.position[i][1];
		if (shape[make_pair(I, J)])continue;
		if (game.grid[I][J] != ' ')
			return true;
	}
	return false;
}

bool collisionTest::Rightcollision(Game game) {
	map <pair<int, int>, bool> shape;
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1];
		shape[make_pair(I, J)] = true;
	}
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1] + 1;
		if (shape[make_pair(I, J)])
			continue;
		if (game.grid[I][J] != ' ')
			return true;
	}
	return false;
}

bool collisionTest::Leftcollision(Game game) {
	map <pair<int, int>, bool> shape;
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1];
		shape[make_pair(I, J)] = true;
	}
	rep(i, 0, game.row) {
		int I = game.position[i][0];
		int J = game.position[i][1] - 1;
		if (shape[make_pair(I, J)])continue;
		if (game.grid[I][J] != ' ')
			return true;
	}
	return false;
}

bool collisionTest::Rotationcollision(Game game) {
	return Leftcollision(game) || Rightcollision(game) || Downcollision(game);
}