#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#include <slog/slog.h>

bool quite = false;

typedef char figure[21];
typedef char board[8][8];

typedef struct {
	int x, y;
	figure *fp;
} chessobj;

enum color_pairs {
	WHITE_ON_BLACK,
	BLACK_ON_WHITE,
	BLUE_ON_WHITE,
	BLUE_ON_BLACK,
	RED_ON_WHITE,
	RED_ON_BLACK
};


enum figures {
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING
};

// do not change thoses
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


int nottoint (char const *n) {
	int retval = 0;

	if ('a' <= n[0] && n[0] <= 'h') {
		if ('1' <= n[1] && n[1] <= '8') {
			retval += (n[0]-'a');
			retval += (n[1]-'1')<<3;
			retval += PAWN<<6;
		}
	} else {
		switch (n[0]) {
			case 'K':
				retval += KING<<6;
				break;
			case 'Q':
				retval += QUEEN<<6;
				break;
			case 'R':
				retval += ROOK<<6;
				break;
			case 'B':
				retval += BISHOP<<6;
				break;
			case 'N':
				retval += KNIGHT<<6;
				break;
			default:
				retval += 99<<6;
		}

		if ('a' <= n[1] && n[1] <= 'h') {
			if ('1' <= n[2] && n[2] <= '8') {
				retval += (n[1]-'a');
				retval += (n[2]-'1')<<3;
			}
		}
	}
	return retval;
}

chessobj createobj (char const *n) {
	int code = nottoint (n);
	chessobj retval;
	switch ((code>>6)%100) {
		case PAWN:
			retval.fp = &pawn;
			break;
		case KING:
			retval.fp = &king;
			break;
		case QUEEN:
			retval.fp = &queen;
			break;
		case KNIGHT:
			retval.fp = &knight;
			break;
		case BISHOP:
			retval.fp = &bishop;
			break;
		case ROOK:
			retval.fp = &rook;
			break;
		default:
			SLOG_ERROR("createobj", "wrong figure type");
	}
	retval.x = code%8;
	retval.y = (code>>3)%8;
	return retval;
}

void mvaddfig (char *n, int color) {
	chessobj obj = createobj (n);

	if ((obj.x + obj.y)%2 == 0) {
		if (COLOR_RED == color) {
			attron(COLOR_PAIR(RED_ON_BLACK));
		} else if (COLOR_BLUE == color) {
			attron(COLOR_PAIR(BLUE_ON_BLACK));
		}
	} else {
		if (COLOR_RED == color) {
			attron(COLOR_PAIR(RED_ON_WHITE));
		} else if (COLOR_BLUE == color) {
			attron(COLOR_PAIR(BLUE_ON_WHITE));
		}
	}

	for (int i = 0; i < sizeof(figure); i++) {
		mvaddch (4*(7-obj.y) + i/7 + 2, obj.x*8 + i%7 + 2, (*obj.fp)[i]);
	}

	if ((obj.x + obj.y)%2 == 0) {
		if (COLOR_RED == color) {
			attroff(COLOR_PAIR(RED_ON_BLACK));
		} else if (COLOR_BLUE == color) {
			attroff(COLOR_PAIR(BLUE_ON_BLACK));
		}
	} else {
		if (COLOR_RED == color) {
			attroff(COLOR_PAIR(RED_ON_WHITE));
		} else if (COLOR_BLUE == color) {
			attroff(COLOR_PAIR(BLUE_ON_WHITE));
		}
	}

}

int init () {
	initscr ();
	noecho ();
	raw ();
	curs_set (0);

	if (!has_colors ()) goto _bailout;

	start_color();
	init_pair (BLUE_ON_WHITE, COLOR_BLUE, COLOR_WHITE);
	init_pair (BLUE_ON_BLACK, COLOR_BLUE, COLOR_BLACK);
	init_pair (WHITE_ON_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair (BLACK_ON_WHITE, COLOR_BLACK, COLOR_WHITE);
	init_pair (RED_ON_BLACK, COLOR_RED, COLOR_BLACK);
	init_pair (RED_ON_WHITE, COLOR_RED, COLOR_WHITE);


	for (int i = 0; i < 8; i++) {
		mvaddch (0, i*8+5, 'A' + i);
		mvaddch (i*4+3, 0, '8' - i);
	}

	for (int i = 1; i < 34; i++) {
		if (i % 4 == 1)
			mvprintw (i, 1, "+-------+-------+-------+-------+-------+-------+-------+-------+");
		else {
			mvaddch (i, 1, '|');
			if ((i-1)%8 < 5) {
				for (int j = 0; j < 4; j++) {
					attron (COLOR_PAIR(BLACK_ON_WHITE));
					mvprintw (i, 2+j*16, "       ");
					attroff (COLOR_PAIR(BLACK_ON_WHITE));
					mvaddch (i, 9+j*16, '|');
					attron (COLOR_PAIR(WHITE_ON_BLACK));
					mvprintw (i, 10+j*16, "       ");
					attroff (COLOR_PAIR(WHITE_ON_BLACK));
					mvaddch (i, 17+j*16, '|');
				}
			}
			else {
				for (int j = 0; j < 4; j++) {
					attron (COLOR_PAIR(WHITE_ON_BLACK));
					mvprintw (i, 2+j*16, "       ");
					attroff (COLOR_PAIR(WHITE_ON_BLACK));
					mvaddch (i, 9+j*16, '|');
					attron (COLOR_PAIR(BLACK_ON_WHITE));
					mvprintw (i, 10+j*16, "       ");
					attroff (COLOR_PAIR(BLACK_ON_WHITE));
					mvaddch (i, 17+j*16, '|');
				}
			}
		}
	}

	mvprintw	 (1, 72, "+-----------------------------+");
	for (int i = 2; i < 33; i++) {
		mvprintw (i, 72, "|                             |");
	}
	mvprintw	(33, 72, "+-----------------------------+");

	mvaddch (32, 74, '>');

	// putting chess figures on board

	mvaddfig ("a7", COLOR_BLUE);
	mvaddfig ("b7", COLOR_BLUE);
	mvaddfig ("c7", COLOR_BLUE);
	mvaddfig ("d7", COLOR_BLUE);
	mvaddfig ("e7", COLOR_BLUE);
	mvaddfig ("f7", COLOR_BLUE);
	mvaddfig ("g7", COLOR_BLUE);
	mvaddfig ("h7", COLOR_BLUE);

	mvaddfig ("Ra8", COLOR_BLUE);
	mvaddfig ("Nb8", COLOR_BLUE);
	mvaddfig ("Bc8", COLOR_BLUE);
	mvaddfig ("Qd8", COLOR_BLUE);
	mvaddfig ("Ke8", COLOR_BLUE);
	mvaddfig ("Bf8", COLOR_BLUE);
	mvaddfig ("Ng8", COLOR_BLUE);
	mvaddfig ("Rh8", COLOR_BLUE);

	mvaddfig ("a2", COLOR_RED);
	mvaddfig ("b2", COLOR_RED);
	mvaddfig ("c2", COLOR_RED);
	mvaddfig ("d2", COLOR_RED);
	mvaddfig ("e2", COLOR_RED);
	mvaddfig ("f2", COLOR_RED);
	mvaddfig ("g2", COLOR_RED);
	mvaddfig ("h2", COLOR_RED);

	mvaddfig ("Ra1", COLOR_RED);
	mvaddfig ("Nb1", COLOR_RED);
	mvaddfig ("Bc1", COLOR_RED);
	mvaddfig ("Qd1", COLOR_RED);
	mvaddfig ("Ke1", COLOR_RED);
	mvaddfig ("Bf1", COLOR_RED);
	mvaddfig ("Ng1", COLOR_RED);
	mvaddfig ("Rh1", COLOR_RED);

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


	refresh ();
_bailout:
	move (0, 0);
	nodelay (stdscr, FALSE);
	endwin ();
	curs_set (1);
	system ("stty sane");
	return 0;
}
