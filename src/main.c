#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#include <slog/slog.h>

#define CURSOR_START_X 76
#define CURSOR_START_Y 32

const char EXIT_CMD[] = {'e', 'x', 'i', 't', '\n'};

typedef char figure[21];
typedef char board[8][8];

typedef struct {
	int x, y;
	const figure *fp;
} chessobj;


enum color_pairs {
	BLACK_ON_BLACK,
	WHITE_ON_WHITE,
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
	KING,
	UNKNOWN
};

// do not change thoses
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

const figure empty = {
	' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' '
};

// if your command is beeing read incorretly, maybe the reason is that you
// have a different number for enter key encoding
int compaire_commands (const char *a, const char *b) {
	for (int i = 0; i < 1000; i++) {
		if (a[i] == '\n' || b[i] == '\n' || a[i] != b[i]) {
			return a[i]-b[i];
		}
	}
	return 0;
}

int nottoint (const char *n) {
	int retval = 0;

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
			retval += PAWN<<6;
			n--;
	}

	if ('a' <= n[1] && n[1] <= 'h') {
		if ('1' <= n[2] && n[2] <= '8') {
			retval += (n[1]-'a');
			retval += (n[2]-'1')<<3;
		}
	}
	switch (n[3]) {
		case 0:
			return retval;
		case '-':
			retval += 1<<9;
			break;
		case 'x':
			retval += 2<<9;
	}

	if ('a' <= n[4] && n[4] <= 'h') {
		if ('1' <= n[5] && n[5] <= '8') {
			retval += (n[4]-'a')<<11;
			retval += (n[5]-'1')<<14;
		}
	}

	return retval;
}

chessobj createobj (int code) {
	chessobj retval;
	switch ((code>>6)%8) {
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
		case UNKNOWN:
			SLOG_ERROR ("createobj", "wrong figure type");
	}
	retval.x = code%8;
	retval.y = (code>>3)%8;
	return retval;
}

void mvaddfig (const char *n, int color) {
	int code = nottoint (n);
	chessobj from = createobj (code);
	chessobj to = createobj (code>>11);

	switch ((code>>9)%4) {
		case 2: // 'x' taken
			// cleaning future place
			if ((to.x + to.y)%2 == 0) {
				attron (COLOR_PAIR (BLACK_ON_BLACK));
			} else {
				attron (COLOR_PAIR (WHITE_ON_WHITE));
			}

			for (int i = 0; i < sizeof(figure); i++) {
				mvaddch (4*(7-to.y) + i/7 + 2, to.x*8 + i%7 + 2, empty[i]);
			}

			if ((to.x + to.y)%2 == 0) {
				attroff (COLOR_PAIR (BLACK_ON_BLACK));
			} else {
				attroff (COLOR_PAIR (WHITE_ON_WHITE));
			}

		case 1: // '-' moved
			// cleaning old place
			if ((from.x + from.y)%2 == 0) {
				attron (COLOR_PAIR (BLACK_ON_BLACK));
			} else {
				attron (COLOR_PAIR (WHITE_ON_WHITE));
			}

			for (int i = 0; i < sizeof(figure); i++) {
				mvaddch (4*(7-from.y) + i/7 + 2, from.x*8 + i%7 + 2, empty[i]);
			}

			if ((from.x + from.y)%2 == 0) {
				attroff (COLOR_PAIR (BLACK_ON_BLACK));
			} else {
				attroff (COLOR_PAIR (WHITE_ON_WHITE));
			}

			// placing figure on it's place
			if ((to.x + to.y)%2 == 0) {
				if (COLOR_RED == color) {
					attron (COLOR_PAIR (RED_ON_BLACK));
				} else if (COLOR_BLUE == color) {
					attron(COLOR_PAIR (BLUE_ON_BLACK));
				}
			} else {
				if (COLOR_RED == color) {
					attron (COLOR_PAIR (RED_ON_WHITE));
				} else if (COLOR_BLUE == color) {
					attron (COLOR_PAIR (BLUE_ON_WHITE));
				}
			}

			for (int i = 0; i < sizeof(figure); i++) {
				mvaddch (4*(7-to.y) + i/7 + 2, to.x*8 + i%7 + 2, (*from.fp)[i]);
			}

			if ((to.x + to.y)%2 == 0) {
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
			return;
	}

	if ((from.x + from.y)%2 == 0) {
		if (COLOR_RED == color) {
			attron (COLOR_PAIR(RED_ON_BLACK));
		} else if (COLOR_BLUE == color) {
			attron (COLOR_PAIR(BLUE_ON_BLACK));
		}
	} else {
		if (COLOR_RED == color) {
			attron (COLOR_PAIR(RED_ON_WHITE));
		} else if (COLOR_BLUE == color) {
			attron (COLOR_PAIR(BLUE_ON_WHITE));
		}
	}

	for (int i = 0; i < sizeof(figure); i++) {
		mvaddch (4*(7-from.y) + i/7 + 2, from.x*8 + i%7 + 2, (*from.fp)[i]);
	}

	if ((from.x + from.y)%2 == 0) {
		if (COLOR_RED == color) {
			attroff (COLOR_PAIR(RED_ON_BLACK));
		} else if (COLOR_BLUE == color) {
			attroff (COLOR_PAIR(BLUE_ON_BLACK));
		}
	} else {
		if (COLOR_RED == color) {
			attroff (COLOR_PAIR(RED_ON_WHITE));
		} else if (COLOR_BLUE == color) {
			attroff (COLOR_PAIR(BLUE_ON_WHITE));
		}
	}
}

bool processcmd (const char buf[17], int turn) {
	if (!compaire_commands (buf, (const char *)EXIT_CMD)) {
		return true;
	} else {
		if (turn%2 == 0) {
			mvaddfig ((const char *)buf, COLOR_RED);
		} else {
			mvaddfig ((const char *)buf, COLOR_BLUE);
		}
	}
	return false;
}

int main () {
	bool quite = false;
	char buf[17];
	int len = 16; // 16 is not an error
	int pos = 0;
	int turn = 0;
	char board[8][8] = {
		'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
		'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
		'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
	};


	{ // initiation
		initscr ();
		noecho ();
		raw ();
		curs_set (0);

		if (!has_colors ()) {
			goto _bailout;
		}

		start_color();
		init_pair (BLUE_ON_WHITE, COLOR_BLUE, COLOR_WHITE);
		init_pair (BLUE_ON_BLACK, COLOR_BLUE, COLOR_BLACK);
		init_pair (BLACK_ON_BLACK, COLOR_BLACK, COLOR_BLACK);
		init_pair (WHITE_ON_WHITE, COLOR_WHITE, COLOR_WHITE);
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
						attron (COLOR_PAIR(WHITE_ON_WHITE));
						mvprintw (i, 2+j*16, "       ");
						attroff (COLOR_PAIR(WHITE_ON_WHITE));
						mvaddch (i, 9+j*16, '|');
						attron (COLOR_PAIR(BLACK_ON_BLACK));
						mvprintw (i, 10+j*16, "       ");
						attroff (COLOR_PAIR(BLACK_ON_BLACK));
						mvaddch (i, 17+j*16, '|');
					}
				}
				else {
					for (int j = 0; j < 4; j++) {
						attron (COLOR_PAIR(BLACK_ON_BLACK));
						mvprintw (i, 2+j*16, "       ");
						attroff (COLOR_PAIR(BLACK_ON_BLACK));
						mvaddch (i, 9+j*16, '|');
						attron (COLOR_PAIR(WHITE_ON_WHITE));
						mvprintw (i, 10+j*16, "       ");
						attroff (COLOR_PAIR(WHITE_ON_WHITE));
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
	}

	while (!quite) {
		buf[pos] = getch();

		switch (buf[pos]) {
			case 127:
				if (pos == 0) {
					break;
				}
				pos--;
				mvaddch (CURSOR_START_Y, CURSOR_START_X+pos, ' ');
				break;
			case '\n':
				pos = 0;
				for (int i = 3; i <= CURSOR_START_Y; i++) {
					char line[19];
					mvinnstr (i, CURSOR_START_X, line, 18);
					mvprintw (i-1, CURSOR_START_X, "%18s", line);
				}
				mvprintw(CURSOR_START_Y, CURSOR_START_X, "                         ");
				quite = processcmd (buf, turn++);
				break;
			default:
				if (pos != len) {
					mvaddch (CURSOR_START_Y, CURSOR_START_X+pos, buf[pos]);
					pos++;
				}
		}
	}

	refresh ();
_bailout:
	move (0, 0);
	nodelay (stdscr, FALSE);
	endwin ();
	curs_set (1);
	system ("stty sane");
	return 0;
}
