#include "Opts.h"

Opts::Opts() {
	setSize(5,5	);
	x=0;
	y=0;
	d=0;
	gx=0;
	gy=0;
	ix=0;
	iy=0;

	aic=0;
	for(int c=0;c<CLRS;c++) 
		for(int c1=0;c1<INSTYPES;c1++) 
			ins[c][c1]=0;
	
}

void Opts::setSize(int w,int h) {
	if(w<0 || h<0)
		return;

	width=w;
	height=h;

	grid.resize(width);
	for(int c=0;c<width;c++) {
		grid[c].resize(height);
	}

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

void Opts::drawGrid(int x, int y) {
	for(unsigned int c=0;c<grid.size();c++) {
		for(unsigned int c1=0;c1<grid[c].size();c1++) {
			move(y + c1  , x + c *2);
			color_set(grid[c][c1].c+3,NULL);
			printw("%1.1d",grid[c][c1].t);
		}
	}
}

void Opts::drawWidHigh(int x, int y) {
	move(y,x);
	color_set(1,NULL);
	printw("Width: %2d   Height: %2d",width,height);
}

void Opts::drawIns(int x,int y) {
	for(int c=0;c<CLRS;c++) {
		color_set(3+c,NULL);
		for(int c1=0;c1<INSTYPES;c1++) {
			move( y + c , x + c1 *2 );
			printw(ins[c][c1]?" ":"X");
		}
	}
}

void Opts::drawFunct(int x,int y) {

}

void Opts::redraw(int x, int y) {
	xPos=x;
	yPos=y;

	drawWidHigh(x, y);
	drawGrid(x, y+2);
	drawIns(x,y+3+height);
	drawFunct(x,y+4+height+CLRS);
}


void Opts::movegCursor(int x, int y) {
	if(x<0)
		x=0;
	if(y<0)
		y=0;
	if(x>=width)
		x=width-1;
	if(y>=height)
		y=height-1;

	gx=x;
	gy=y;
}

void Opts::gcursorSelect(int t) {
	// t: type

	if(width<1 || height<1)
		return;

	//to correct cursor, if it is outsise	 of the grid
	movegCursor(gx,gy);

	switch(t) {
		case 0:
			grid[gx][gy].t= (grid[gx][gy].t+1)%TILES;
			break;
		case 1:
			grid[gx][gy].t= (grid[gx][gy].t-1+TILES)%TILES;
			break;
		case 2:
			grid[gx][gy].c= (grid[gx][gy].c+1)%CLRS;
			break;
		case 3:
			grid[gx][gy].c= (grid[gx][gy].c-1+CLRS)%CLRS;
			break;
	}
}

void Opts::moveiCursor(int x, int y) {
	if(x<0)
		x=0;
	if(y<0)
		y=0;
	if(x>=INSTYPES)
		x=INSTYPES-1;
	if(y>=CLRS)
		y=CLRS-1;

	ix=x;
	iy=y;
}

void Opts::icursorSelect() {
	if(INSTYPES<1 || CLRS<1)
		return;

	//to correct cursor
	moveiCursor(ix,iy);

	ins[iy][ix]=!ins[iy][ix];
}

void Opts::getCursorPos(int cs, int *cx, int *cy) {
	if(cs==0) {
		*cx = xPos + gx*2;
		*cy = yPos + gy + 2;
		return;
	}
	if(cs==1) {
		*cx = xPos + 23;
		*cy = yPos;
		return;
	}
	if(cs==2) {
		*cx = xPos + ix * 2;
		*cy = yPos + 3 + height + iy;
		return;		
	}
}