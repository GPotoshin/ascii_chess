#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#include <slog/slog.h>

#define BLACK_PAIR 1
#define WHITE_PAIR 2

bool quite = false;

typedef char figure[21];
typedef char board[8][8];

typedef struct {
	int x, y;
	figure *fp;
} chessobj;

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

void mvaddfig (char *n) {
   chessobj obj = createobj (n);

   for (int i = 0; i < sizeof(figure); i++) {
	   mvaddch (obj.y + i/7, obj.x + i%7, (*obj.fp)[i]);
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
		mvaddch (i*4+3, 0, '8' - i);
	}

	for (int i = 1; i < 34; i++) {
		if (i % 4 == 1)
			mvprintw (i, 1, "+-------+-------+-------+-------+-------+-------+-------+-------+");
		else {
			mvaddch (i, 1, '|');
			if ((i-1)%8 < 5) {
				for (int j = 0; j < 4; j++) {
					attron (COLOR_PAIR(WHITE_PAIR));
					mvprintw (i, 2+j*16, "       ");
					attroff (COLOR_PAIR(WHITE_PAIR));
					mvaddch (i, 9+j*16, '|');
					attron (COLOR_PAIR(BLACK_PAIR));
					mvprintw (i, 10+j*16, "       ");
					attroff (COLOR_PAIR(BLACK_PAIR));
					mvaddch (i, 17+j*16, '|');
				}
			}
			else {
				for (int j = 0; j < 4; j++) {
					attron (COLOR_PAIR(BLACK_PAIR));
					mvprintw (i, 2+j*16, "       ");
					attroff (COLOR_PAIR(BLACK_PAIR));
					mvaddch (i, 9+j*16, '|');
					attron (COLOR_PAIR(WHITE_PAIR));
					mvprintw (i, 10+j*16, "       ");
					attroff (COLOR_PAIR(WHITE_PAIR));
					mvaddch (i, 17+j*16, '|');
				}
			}
		}
	}

	// putting chess figures on board

	mvaddfig ("Re4");

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
