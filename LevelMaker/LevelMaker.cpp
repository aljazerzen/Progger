#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <vector>

#include "Pack.h"

using namespace std;

int main(int argc, char **argv) {
	srand(time(NULL));

	Pack pck;

	if ( argc > 1) {
		printf("Loading package at %s\n", argv[1] );
		pck.fromFile(argv[1]);
	}

	WINDOW *win = initscr();
	start_color();

	timeout(1000);
	keypad(win, 1);

	//setup terminal colors
	init_pair(1, 7, COLOR_BLACK); 			// default font
	init_pair(2, 2, COLOR_BLACK); // not used

	init_pair(3, 7, COLOR_BLACK);
	init_pair(4, 1, COLOR_BLACK);
	init_pair(5, 2, COLOR_BLACK);
	init_pair(6, 3, COLOR_BLACK);
	init_pair(7, 4, COLOR_BLACK);

	wbkgd(win, COLOR_PAIR(1));

	bool quit = 0;

	int inp;
	// cursor state:
	/*
		0 : move on the grid
		1 : resize grid
		2 : edit allowed instructions
		3 : edit functions
		4 : edit name
		5 : edit discription
		6 : set starting cursor position
	*/

	while (!quit) {

		erase();

		pck.redraw(2, 4);

		int cx, cy;
		pck.getCursorPos(&cx, &cy);
		move(cy, cx);

		inp = getch();
		if (inp != -1)
			quit = pck.input(inp);

	}

	endwin();

	return 0;
}
