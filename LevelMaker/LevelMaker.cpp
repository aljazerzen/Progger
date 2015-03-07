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

	//setup terminal colors 
	init_pair(1,7,COLOR_BLACK); 			// default font
	init_pair(2,2,COLOR_BLACK); // not used
	
	init_pair(3,7,COLOR_BLACK); 
	init_pair(4,1,COLOR_BLACK); 
	init_pair(5,2,COLOR_BLACK); 
	init_pair(6,3,COLOR_BLACK); 
	init_pair(7,4,COLOR_BLACK); 

	wbkgd(win,COLOR_PAIR(1));

	bool quit=0;

	Opts op;
	int inp;
	int cs=1;
	// cursor state:
	/*
		0 : move on the grid
		1 : resize grid
		2 : edit allowed instructions
		3 : edit functions
	*/

	while(!quit) {

		erase();

		color_set(1,NULL);

		move(1,1);
		switch(cs) {
			case 0: 
				printw(" --[ grid editing   ]-- ");
				break;
			case 1: 
				printw(" --[ grid resizing  ]-- ");
				break;
			case 2: 
				printw(" --[ instructions   ]-- ");
				break;
			case 3: 
				printw(" --[ functions      ]-- ");
				break;
			//case 4:
			//case 5:
		}
		op.redraw(2,4);

		int cx,cy;
		op.getCursorPos(cs,&cx,&cy);
		move(cy,cx);

		inp = getch();
		switch(inp) {
			case 'Q':
			case 'q':
				quit=1;
				break;
			case 'w':
			case 'W':
				cs=0;
				break;
			case 'e':
			case 'E':
				cs=1;
				break;
			case 'r':
			case 'R':
				cs=2;
				break;
			case 't':
			case 'T':
				cs=3;
				break;
			default:
				switch(cs) {
					case 0:
						switch(inp) {
							case KEY_RIGHT:
								op.movegCursor(op.gx+1,op.gy);
								break;
							case KEY_LEFT:
								op.movegCursor(op.gx-1,op.gy);
								break;
							case KEY_DOWN:
								op.movegCursor(op.gx,op.gy+1);
								break;
							case KEY_UP:
								op.movegCursor(op.gx,op.gy-1);
								break;
							case ' ':
								op.gcursorSelect(0);
								break;
							case '\n':
								op.gcursorSelect(2);
								break;
						}	
						break;
					case 1:	
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
						}
						break;
					case 2:
						switch(inp) {
							case KEY_RIGHT:
								op.moveiCursor(op.ix+1,op.iy);
								break;
							case KEY_LEFT:
								op.moveiCursor(op.ix-1,op.iy);
								break;
							case KEY_DOWN:
								op.moveiCursor(op.ix,op.iy+1);
								break;
							case KEY_UP:
								op.moveiCursor(op.ix,op.iy-1);
								break;
							case '\n':
								op.icursorSelect();
								break;
						}	
						break;
				}			
		}
	}

	endwin();
	
	return 0;
}
