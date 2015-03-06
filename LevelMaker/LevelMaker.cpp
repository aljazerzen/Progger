#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <vector>

using namespace std;

#include "Opts.h"

int main(int argc, char **argv) {
	srand(time(NULL));


	WINDOW *win=initscr();
	start_color();
	
	timeout(1000);
	keypad(win,1);

	init_pair(1,4,0); //setup terminal colors 
	init_pair(2,2,0); //setup terminal colors 

	bool quit=0;

	Opts op;
	int inp;

	while(!quit) {

		erase();
		op.redraw();
		move(0,0);

		inp = getch();
		switch(inp) {
			case KEY_RIGHT:
				op.incWidth();
				break;
			case KEY_LEFT:
				op.decWidth();
				break;
			case KEY_DOWN:
				op.incHeight();
				break;
			case KEY_UP:
				op.decHeight();
				break;
			case 'Q':
			case 'q':
				quit=1;
				break;
//			default :
		//		printw("%d--%d",inp,KEY_RIGHT);
		}
	}

	endwin();
	
	return 0;
}
