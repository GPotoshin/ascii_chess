#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#define BLACK_PAIR 1
#define WHITE_PAIR 2

bool quite = false;

typedef char figure[21];
typedef char board[8][8];

const figure pawn = {
' ',' ',' ',' ',' ',' ',' ', 
' ',' ',' ','@',' ',' ',' ',
' ',' ','_','^','_',' ',' '
};

const figure rook = {
' ','.',' ','#',' ','.',' ', 
' ','-','-','-','-','-',' ',
' ',' ','#','#','#',' ',' '
};

const figure knight = {
' ',' ',' ','^','^',' ',' ', 
' ',' ','/',' ','|','\\',' ',
' ',' ','_','_','^','_',' '
};

const figure bishop = {
' ',' ',' ','.',' ',' ',' ', 
' ',' ','<','#','>',' ',' ',
' ',' ','_','^','_',' ',' '
};

const figure queen = {
'.',' ','.',' ','.',' ','.', 
'\\',' ','\\',' ','/',' ','/',
' ','_','_','^','_','_',' '
};

const figure king = {
' ',' ',' ','+',' ',' ',' ', 
' ',' ','@',' ','@',' ',' ',
' ',' ','_','^','_',' ',' '
};


void mvaddfig (int y, int x, const figure fig) {
	for (int i = 0; i < sizeof(figure); i++) {
		mvaddch (y + i/7, x + i%7, fig[i]);
	}
}

int init () {
	initscr ();
	noecho ();
	raw ();
	curs_set (0);

	if (!has_colors ()) goto _bailout;

	start_color();
	init_pair (WHITE_PAIR, COLOR_BLACK, COLOR_WHITE);
	init_pair (BLACK_PAIR, COLOR_WHITE, COLOR_BLACK);


	for (int i = 0; i < 8; i++) {
		mvaddch (0, i*8+5, 'A' + i);
	}

	for (int i = 1; i < 34; i++) {
		if (i % 4 == 1)
			mvprintw (i, 1, "+-------+-------+-------+-------+-------+-------+-------+-------+");
		else
			mvprintw (i, 1, "|       |       |       |       |       |       |       |       |");
	}



	return 0;
_bailout:
	endwin ();
	return 1;
}

void run () {

}

int main () {
	if (init ()) goto _bailout;
	getch ();
//	while (!quite) run ();


	mvaddfig (0, 0, king);
	refresh ();
_bailout:
	move (0, 0);
	nodelay (stdscr, FALSE);
	endwin ();
	curs_set (1);
	system ("stty sane");
	return 0;
}
