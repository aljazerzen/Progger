#include "Pack.h"

Pack::Pack() {
	mainMenu=1;
	levelId=0;

	xPos=0;
	yPos=0;

	cs=-1;
	qui=0;

	keyboardLock=1;

	strcpy(name , "Pack name");
	ncu=strlen(name);
}

void Pack::modString(char *src, int inp, int *cursor, int max) {
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

unsigned char Pack::calcCrc(FILE* file) {

	int sum = 0;
	char inp =0;

	while(inp != EOF) {
		sum = (sum*7) & 0xff;
		inp = fgetc(file);
		sum = (sum+inp) & 0xff;
	}

	return sum; 
}

void Pack::fromFile(char *path) {
	FILE *sFile=fopen(path,"r");

	if(sFile==NULL) {
		fprintf(stderr,"Error reading data from %s\n", path);
		return;
	}

	strncpy(name,path,PACK_NAME_LEN);
	*index(name,'.')='\0';

	// check crc
	int crc=calcCrc(sFile);
	
	if(crc == 0xff) {
		fseek(sFile,0,SEEK_SET);

		do {
			newLevel();
		} while (!levels.back()->fromFile(sFile));
		popLevel();

	} else {
		fprintf(stderr, "%s is not valid pack file\n", path);
	}

	fclose(sFile);
}

void Pack::toFile(char *path) {

	if(levels.size()==0) {
		return;
	}

	char *fpath = (char*) malloc(strlen(path)+7);
	strcpy(fpath,path);
	strcat(fpath,".pack");

	FILE *sFile=fopen(fpath,"w");

	if(sFile==NULL) {
		fprintf(stderr,"Error writing user data to %s\n", fpath);
		return;
	}

	for(unsigned int c=0;c<levels.size();c++) {
		levels[c]->toFile(sFile);
	}

	fclose(sFile);

	// crc calcualation
	sFile=fopen(fpath,"r");
	char crc = 0xff-calcCrc(sFile);
	fclose(sFile);

	// crc appendtaiton
	sFile=fopen(fpath,"a");
	fputc(crc,sFile);
	fclose(sFile);

	free(fpath);
}

void Pack::gotoLevel(int i) {
	if(i<0 || i>= (int)levels.size()) 
		return;

	mainMenu=0;
	cs=4;
	levelId=i;
}

void Pack::gotoMain() {
	mainMenu=1;
	cs=0;
}

void Pack::quit() {
	if(mainMenu) {
		toFile(name);
		qui=1;
	} else {
		gotoMain();
	}
}

void Pack::moveCursor(int i) {
	if(!mainMenu)
		return;
	if(cs+i<-1 || cs+i>=(int)levels.size()) 
		return;
	cs+=i;

	if(cs < 0) {
		keyboardLock = 1;
	}
}

void Pack::newLevel() {
	Level *newLevel = new Level();
	levels.push_back(newLevel);
}

bool Pack::movLevel(int i, bool d) {
	int newPos = i + d*2 - 1;
	if( newPos<0 || newPos>=(int)levels.size())
		return 0;

	Level *tmp = levels[i];
	levels[i]=levels[newPos];
	levels[newPos]=tmp;
	return 1; 
}

void Pack::popLevel() {
	if(levels.size()==0)
		return;

	delete levels[levels.size()-1];
	levels.pop_back();
}

void Pack::delLevel(int i) {
	if(i<0)
		return;

	while(movLevel(i++,1)) ;
	i--;

	popLevel();

}


bool Pack::input(int inp) {
	bool MasterInput=0;
	
	if(mainMenu) {
		inputMain(inp);
	} else {

		// if keyboard isnt locked, check if input is master input 
		//(and take actions if it is)
		if(!keyboardLock) {
			MasterInput=inputMaster(inp);
		}

		// if input isnt master, pass it to the mode 
		if(!MasterInput) {
			inputState(inp);
		}
	}

	return qui;
}

void Pack::inputMain(int inp) {
	
	if( keyboardLock) {
		if ( inp == '\n') {
			keyboardLock = 0;
			cs=0;
		} else {
			keyboardLock = 1;
			modString(name, inp, &ncu, PACK_NAME_LEN);
		}
		return;
	}


	inp = tolower(inp);

	switch(inp) {
		case 'q' :
			quit();
			return;
		case 'n' :
			newLevel();
			gotoLevel(levels.size()-1);
			break;
		case KEY_DOWN:
			moveCursor(1);
			break;
		case KEY_UP:
			moveCursor(-1);
			break;
		case '\n':
		case ' ':
			gotoLevel(cs);
	}
}

bool Pack::inputMaster(int inp) {
	inp = tolower(inp);
	switch(inp) {
		case 'q':
			quit();
			return 1;
		case '4':
			cs=0;
			return 1;
		case '3':
			cs=1;
			return 1;
		case '5':
			cs=2;
			return 1;
		case '6':
			cs=3;
			return 1;
		case '1':
			cs=4;
			return 1;
		case '2':
			cs=5;
			return 1;
		case '7':
			cs=6;
			return 1;
	}

	return 0;
}

void Pack::inputState(int inp) {
	Level* cl = levels[levelId];

	switch(cs) {
		case 0:
			switch(inp) {
				case KEY_RIGHT:
					cl->movegCursor(cl->gx+1,cl->gy);
					break;
				case KEY_LEFT:
					cl->movegCursor(cl->gx-1,cl->gy);
					break;
				case KEY_DOWN:
					cl->movegCursor(cl->gx,cl->gy+1);
					break;
				case KEY_UP:
					cl->movegCursor(cl->gx,cl->gy-1);
					break;
				case ' ':
					cl->gcursorSelect(0);
					break;
				case '\n':
					cl->gcursorSelect(2);
					break;
			}	
			break;
		case 1:	
			switch(inp) {
				case KEY_RIGHT:
					cl->incWidth();
					break;
				case KEY_LEFT:
					cl->decWidth();
					break;
				case KEY_DOWN:
					cl->incHeight();
					break;
				case KEY_UP:
					cl->decHeight();
					break;
			}
			break;
		case 2:
			switch(inp) {
				case KEY_RIGHT:
					cl->moveiCursor(cl->ix+1,cl->iy);
					break;
				case KEY_LEFT:
					cl->moveiCursor(cl->ix-1,cl->iy);
					break;
				case KEY_DOWN:
					cl->moveiCursor(cl->ix,cl->iy+1);
					break;
				case KEY_UP:
					cl->moveiCursor(cl->ix,cl->iy-1);
					break;
				case '\n':
				case ' ':
					cl->icursorSelect();
					break;
			}	
			break;
		case 3:
			switch(inp) {
				case KEY_RIGHT:
					cl->movefCursor(cl->fx+1);
					break;
				case KEY_LEFT:
					cl->movefCursor(cl->fx-1);
					break;
				case KEY_DOWN:
					cl->fcursorSelect(1);
					break;
				case KEY_UP:
					cl->fcursorSelect(0);
					break;
				case KEY_BACKSPACE:
					cl->decfSize();
					break;
				case ' ':
					cl->incfSize();
					break;
			}	
			break;
		case 4:
		case 5:
			if ( inp == '\n') {
				keyboardLock = 0;
				cs++;
				if(cs==6)
					cs=0;
			} else {
				keyboardLock = 1;
				if(cs==4)
					cl->modName(inp);
				else
					cl->modDis(inp);
			}						
			break;
		case 6:
			switch(inp) {
				case KEY_RIGHT:
					cl->movecCursor(1,0);
					break;
				case KEY_LEFT:
					cl->movecCursor(-1,0);
					break;
				case KEY_DOWN:
					cl->movecCursor(0,1);
					break;
				case KEY_UP:
					cl->movecCursor(0,-1);
					break;
				case ' ':
				case '\n':
					cl->rotatecCursor(1);
					break;
			}
			break;
	}
}

void Pack::redraw(int x, int y) {
	if(mainMenu) {
		redrawMain(x,y);
	} else {
		color_set(1,NULL);

		move(x,y);
		switch(cs) {
			case 0: 
				printw(" --[ Grid editing   ]-- ");
				break;
			case 1: 
				printw(" --[ Grid resizing  ]-- ");
				break;
			case 2: 
				printw(" --[ Instructions   ]-- ");
				break;
			case 3: 
				printw(" --[ Functions      ]-- ");
				break;
			case 4:
				printw(" --[ Name           ]-- ");
				break;
			case 5:
				printw(" --[ Description    ]-- ");
				break;
			case 6:
				printw(" --[ Setting cursor ]-- ");
				break;
		}


		levels[levelId]->redraw(x, y+2);
	}
}

void Pack::redrawMain(int x, int y) {
	xPos=x;
	yPos=y;

	move(y,x);
	printw("%s", name);

	for(unsigned int c=0;c<levels.size();c++) {
		move(y+c+2,x+2);
		printw("%s", levels[c]->name);
	}
}

void Pack::getCursorPos(int *x, int *y) {
	if(mainMenu) {
		if(keyboardLock) {
			*x = xPos + ncu;
			*y = yPos;
		} else {
			*x = xPos;
			*y = 2+ yPos + cs;
		}
	} else {
		levels[levelId]->getCursorPos(cs,x,y);
	}
}
