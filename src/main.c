#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#include <slog/slog.h>

#define BLACK_PAIR 1
#define WHITE_PAIR 2


bool quite = false;

typedef char figure[3][7];
typedef char board[8][8];

figure pawn = {
' ',' ',' ',' ',' ',' ',' ', 
' ',' ',' ','@',' ',' ',' ',
' ',' ','_','^','_',' ',' '
};

figure rook = {
' ','.',' ','#',' ','.',' ', 
' ','-','-','-','-','-',' ',
' ',' ','#','#','#',' ',' '
};

figure knight = {
' ',' ',' ','^','^',' ',' ', 
' ',' ','/',' ','|','\\',' ',
' ',' ','_','_','^','_',' '
};

figure bishop = {
' ',' ',' ','.',' ',' ',' ', 
' ',' ','<','#','>',' ',' ',
' ',' ','_','^','_',' ',' '
};

figure queen = {
'.',' ','.',' ','.',' ','.', 
'\\',' ','\\',' ','/',' ','/',
' ','_','_','^','_','_',' '
};

figure king = {
' ',' ',' ','+',' ',' ',' ', 
' ',' ','@',' ','@',' ',' ',
' ',' ','_','^','_',' ',' '
};



void addfigure (char *move) {
	SLOG_FN_CALL("main.c", "addfigure");
	int pos_x, pos_y;
	figure *figp;

	if (move[0] >= 'a') {
		int pos_x = move[0] - 'a';
		int pos_y = move[1] - '1';
		figp = &pawn;
	}
	else {
		int pos_x = move[1] - 'a';
		int pos_y = move[2] - '1';
		switch (move[0]) {
			case 'R':
				figp = &rook;
				break;
			case 'N':
				figp = &knight;
				break;
			case 'B':
				figp = &bishop;
				break;
			case 'Q':
				figp = &queen;
				break;
			case 'K':
				figp = &knight;
				break;
		}
	}

	pos_y = 2 + pos_y * 4;
	pos_x = 1 + pos_x * 8;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 7; j++)
			mvaddch (pos_y+i, pos_x+j, (*figp)[i][j]);

}


int init () {
	SLOG_FN_CALL("main.c", "init");
	initscr ();
	noecho ();
	raw ();
	curs_set (0);

	if (!has_colors()) {
		SLOG_ERROR ("init", "your terminal does not support colors");
		return 1;
	}

	start_color();
	init_pair (WHITE_PAIR, COLOR_BLACK, COLOR_WHITE);
	init_pair (BLACK_PAIR, COLOR_WHITE, COLOR_WHITE);

	SLOG_INFO("init", "drawing board");
	for (int i = 0; i < 9; i++) {
		// here i is used to count the vertical '|' lines
		for (int yrange = 1; yrange < 34; yrange++)
			mvaddch (yrange, 1+i*8, '|');
		// here i is used to count horizontal lines
		for (int xdash = 0; xdash < 4; xdash++)
			for (int column = 0; column < 8; column++)
				mvaddch (1+i*4, 2 + xdash*2 + column*8, '-');
	}

	SLOG_INFO("init", "naming squares");
	for (int i = 0; i < 8; i++) {
		mvaddch (0, 5 + 8*i, 'A' + i);
		mvaddch (31 - 4*i, 67, '1' + i);
	}

	SLOG_INFO("init", "putting figures on board");
// pawns
	for (int i = 0; i < 8; i++) {
		char *p1 = "a2";
		char *p2 = "a7";
		p1[0] = 'a' + i;
		p2[0] = 'a' + i;
		addfigure (p1);
		addfigure (p2);
	}

	refresh ();
	return 0;
}

void run () {
	SLOG_FN_CALL("main.c", "run");
	char ch = getch ();
	switch (ch) {
		case 'x':
			quite = true;
	}
}

int main () {
	SLOG_INFO("main", "that's start of logging\n");
	if (init ()) goto _bailout;
	SLOG_INFO("main", "initiation is completed");
	getch ();
	nodelay (stdscr, true);
	while (!quite) run ();

_bailout:
	fclose (SLOG_OUT);
	curs_set (1);
	nodelay (stdscr, false);
	endwin ();
	system ("stty sane");
	return 0;
}
