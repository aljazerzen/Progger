#include "Opts.h"

Opts::Opts() {
	setSize(3,3);
	x=0;
	y=0;
	d=0;

	aic=0;
	for(int c=0;c<CLRS;c++) 
		for(int c1;c1<INSTYPES;c1++) 
			ins[c][c1]=1;

}

void Opts::setSize(int w,int h) {
	if(w<0 || h<0)
		return;

	width=w;
	height=h;

	grid.resize(width);
	for(int c=0;c<width;c++)
		grid[c].resize(height);

}

void Opts::incWidth() {
	setSize(width+1,height);
}

void Opts::decWidth() {
	setSize(width-1,height);
}

void Opts::incHeight() {
	setSize(width,height+1);
}

void Opts::decHeight() {
	setSize(width,height-1);
}

void Opts::redraw() {

	move(2,2);
	color_set(1,NULL);
	printw("Width: %2d   Height: %2d",width,height);
	
	for(unsigned int c=0;c<grid.size();c++) {
		for(unsigned int c1=0;c1<grid[c].size();c1++) {
			move(4 + c1  , 2 + c *2);
			color_set(grid[c][c1].c,NULL);
			printw("%1.1d",grid[c][c1].t);
		}
	}


}
