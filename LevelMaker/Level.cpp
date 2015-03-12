#include "Level.h"

Level::Level() {
	setSize(5,5	);
	setFunctSize(1);
	
	strcpy(name,"Name");
	strcpy(dis,"Description");
	nc=strlen(name);
	dc=strlen(dis);

	cux=0;
	cuy=0;
	cud=0;
	gx=0;
	gy=0;
	ix=0;
	iy=0;
	fx=0;

	setIns();
}

void Level::clearIns() {
	for(int c=0;c<CLRS;c++) 
		for(int c1=0;c1<INSTYPES;c1++) 
			ins[c][c1]=0;	
}

void Level::setIns() {
	for(int c=0;c<CLRS;c++) 
		for(int c1=0;c1<INSTYPES;c1++) 
			ins[c][c1]=1;	
}

void Level::toFile(FILE *file) {
 
	int alc=0;
	for(int c=0;c<CLRS;c++) 
		for(int c1=0;c1<INSTYPES;c1++) 
			alc+= ins[c][c1]==1;


	// Name + Description
	int l=strlen(name);
	fputc(l,file);
	for(int c=0;c<l;c++) {
		fputc(name[c] - LLIMIT, file);
	}

	l=strlen(dis);
	fputc(l,file);
	for(int c=0;c<l;c++) {
		fputc(dis[c] - LLIMIT, file);
	}

	// Level data 
	fputc(width,file);
	fputc(height,file);
	fputc(cux,file);
	fputc(cuy,file);
	fputc(cud,file);
	
	for(int c=0;c<height;++c) {
		for(int c1=0;c1<width;++c1) {
			fputc(grid[c1][c].c,file);
			fputc(grid[c1][c].t,file);
		}
	}

	// ----[ Allowed instructions ]----
	if(alc==CLRS*INSTYPES)
		fputc(0,file);
	else {
		fputc(alc,file);
		for(int c=0;c<CLRS;c++) 
			for(int c1=0;c1<INSTYPES;c1++) 
				if(ins[c][c1]) {
					fputc( c,file);
					fputc(c1,file);
				}
	} 
	
	// ----[ Functions sizes ]----
	fputc(funct.size(),file);
	
	for(unsigned int c=0;c<funct.size();c++) {		
		fputc(funct[c],file);
	}
}

bool Level::fromFile(FILE *file) {

	// Name + Description
	int l;
	l=fgetc(file);

	if(l==EOF) return 1;
	for(int c=0;c<l;c++) {
		name[c]=fgetc(file) + LLIMIT;
	}
	name[l]='\0';

	l=fgetc(file);
	if(l==EOF) return 1;
	for(int c=0;c<l;c++) {
		dis[c]=fgetc(file) + LLIMIT;
	}
	dis[l]='\0';

	// Level data 
	width=fgetc(file);
	if(l==EOF) return 1;
	height=fgetc(file);
	if(l==EOF) return 1;
	cux=fgetc(file);
	cuy=fgetc(file);
	cud=fgetc(file);
	
	setSize(width,height);

	for(int c=0;c<height;++c) {
		for(int c1=0;c1<width;++c1) {
			grid[c1][c].c = fgetc(file);
			grid[c1][c].t = fgetc(file);
		}
	}

	// ----[ Allowed instructions ]----
	int alc;
	alc = fgetc(file);

	if(alc==0) {
		setIns();
	} else {
		clearIns();

		for(int c=0;c<alc;c++) {
			ins[fgetc(file)][fgetc(file)]=1;
		}
	} 
	
	// ----[ Functions sizes ]----
	int fsize;
	fsize = fgetc(file);
	if(l==EOF) return 1;
	funct.resize(fsize);
	
	for(int c=0;c<fsize;c++) {		
		funct[c]=fgetc(file);
	}

	return 0;
}

void Level::setFunctSize(int s) {
	if(s<0)
		return;

	funct.resize(s);

	for(unsigned int c=funct.size();c--;) {
		if(funct[c]<1) {
			funct[c]=1;
		}
	}

	movefCursor(fx);
}

void Level::incfSize() {
	setFunctSize((int) funct.size()+1);
}
void Level::decfSize() {
	setFunctSize((int) funct.size()- (int)1);
}

void Level::setSize(int w,int h) {
	if(w<0 || h<0)
		return;

	width=w;
	height=h;

	grid.resize(width);
	for(int c=0;c<width;c++) {
		grid[c].resize(height);
	}

}

void Level::incWidth() {
	setSize(width+1,height);
}

void Level::decWidth() {
	setSize(width-1,height);
}

void Level::incHeight() {
	setSize(width,height+1);
}
	
void Level::decHeight() {
	setSize(width,height-1);
}

void Level::modString(char *src, int inp, int *cursor, int max) {
	switch(inp) {
		case KEY_BACKSPACE:
			if(*cursor > 0) {
				strcpy(src-1+*cursor, src+*cursor);
				*cursor=*cursor-1;
			}
			break;
		case KEY_LEFT:
			if(*cursor > 0) {
				*cursor=*cursor-1;
			}
			break;
		case KEY_RIGHT:
			if(*cursor < strlen(src)) {
				*cursor=*cursor+1;
			}
			break;			
		default:
			int l=strlen(src);
			if(inp >= LLIMIT  && inp < ULIMIT && l+1 < max) {
				strcpy(src + *cursor + 1,src + *cursor);
				src[*cursor]=inp;
				*cursor=*cursor+1;
			}
	}
}

void Level::modName(int c) {
	modString(name, c, &nc, NAME_LEN);
}

void Level::modDis(int c) {
	modString(dis, c, &dc, DIS_LEN);
}

void Level::drawGrid(int x, int y) {
	for(unsigned int c=0;c<grid.size();c++) {
		for(unsigned int c1=0;c1<grid[c].size();c1++) {
			move(y + c1  , x + c *2);
			color_set(grid[c][c1].c+3,NULL);
			printw("%c",TILE[grid[c][c1].t]);
		}
	}
	move(y + cuy, x + cux*2+1);
	color_set(1,NULL);
	printw("%c",CURSOR[cud]);
}

void Level::drawWidHigh(int x, int y) {
	move(y,x);
	color_set(1,NULL);
	printw("Width: %2d   Height: %2d",width,height);
}

void Level::drawIns(int x,int y) {
	for(int c=0;c<CLRS;c++) {
		color_set(3+c,NULL);
		for(int c1=0;c1<INSTYPES;c1++) {
			move( y + c , x + c1 *2 );
			printw("%c",ins[c][c1]?INSTYPE[c1]:' ');
		}
	}
}

void Level::drawFunct(int x,int y) {
	color_set(1,NULL);
	for(unsigned int c=0;c<funct.size();c++) {
		move(y, x + c * 3);
		printw("%d",funct[c]);
	}
}

void Level::drawName(int x,int y) {
	move(y,x);
	color_set(1,NULL);
	printw("%s",name);
}

void Level::drawDis(int x,int y) {
	move(y,x);
	color_set(1,NULL);
	printw("%s",dis);
}

void Level::redraw(int x, int y) {
	xPos=x;
	yPos=y;

	drawName(x, y);
	drawDis(x, y+1);
	drawWidHigh(x, y+3);
	drawGrid(x, y+5);
	drawIns(x,y+6+height);
	drawFunct(x,y+7+height+CLRS);
}


void Level::movegCursor(int x, int y) {
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

void Level::gcursorSelect(int t) {
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

void Level::moveiCursor(int x, int y) {
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

void Level::icursorSelect() {
	if(INSTYPES<1 || CLRS<1)
		return;

	//to correct cursor
	moveiCursor(ix,iy);

	ins[iy][ix]=!ins[iy][ix];
}

void Level::movefCursor(int x) {
	if(x<0)
		x=0;
	if((unsigned int)x >= funct.size())
		x=funct.size()-1;

	fx=x;
}

void Level::fcursorSelect(int t) {
	if(funct.size()==0)
		return;

	movefCursor(fx);

	if(t==0) {
		funct[fx]++;
	}
	if(t==1) {
		funct[fx]--;
	}
	if(funct[fx]<1)
		funct[fx]=1;
}

void Level::movecCursor(int x, int y) {
	if(width<0 || height <0)
		return;

	if(cux+x < 0) {
		x=-cux;
	}
	if(cuy+y < 0) {
		y=-cuy;
	}
	if(cux+x >= width) {
		x=width-cux-1;
	}
	if(cuy+y >= height) {
		y=height-cuy-1;
	}

	cux+=x;
	cuy+=y;
}

void Level::rotatecCursor(int d) {
	cud= (cud+(d%4)+4)%4;
}
	
void Level::getCursorPos(int cs, int *cx, int *cy) {
	if(cs==0) {
		*cx = xPos + gx*2;
		*cy = yPos + gy + 5;
		return;
	}
	if(cs==1) {
		*cx = xPos + 23;
		*cy = yPos + 3;
		return;
	}
	if(cs==2) {
		*cx = xPos + ix * 2;
		*cy = yPos + 6 + height + iy;
		return;		
	}
	if(cs==3) {
		*cx = xPos + fx * 3;
		*cy = yPos + 7 + height + CLRS;
		return;		
	}
	if(cs==4) {
		*cx = xPos + nc;
		*cy = yPos;
		return;		
	}
	if(cs==5) {
		*cx = xPos + dc;
		*cy = yPos +1;
		return;		
	}
}