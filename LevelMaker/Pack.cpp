#include "Pack.h"

char keyBinds[KEY_BINDS][2][32] = {
	{"q", "Quit"},
	{"n", "New Level"},
	{"Del", "Remove Level"},
	{"Enter", "Enter level"},
	{"Space", "Enter level"},
	{"+", "Move level up"},
	{"-", "Move level down"}
};

char cursorStates[CURSOR_STATES][20] = {
	"Grid editing",
	"Grid resizing",
	"Instructions",
	"Functions",
	"Name",
	"Description",
	"Setting cursor"
};

Pack::Pack() {
	mainMenu = 1;
	levelId = 0;

	xPos = 0;
	yPos = 0;

	cs = -2;
	qui = 0;

	keyboardLock = 0;

	strcpy(name , "Pack name");
	ncu = strlen(name);
	strcpy(dis , "Pack description");
	dcu = strlen(dis);

	packId = 0;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("./")) != NULL) {

		while ((ent = readdir (dir)) != NULL) {

			char *dot = rindex(ent->d_name, '.');
			char tdis[] = "";

			if (dot != NULL && strcmp(dot, ".pack") == 0) {
				int l = 0;
				sscanf(ent->d_name, "%d.pack", &l);
				if ( l > packId)
					packId = l;
			}
		}

		packId++;
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("Pack directory");
	}

}

void Pack::modString(char *src, int inp, int *cursor, int max) {
	switch (inp) {
	case KEY_BACKSPACE:
		if (*cursor > 0) {
			strcpy(src - 1 + *cursor, src + *cursor);
			*cursor = *cursor - 1;
		}
		break;
	case KEY_LEFT:
		if (*cursor > 0) {
			*cursor = *cursor - 1;
		}
		break;
	case KEY_RIGHT:
		if (*cursor < strlen(src)) {
			*cursor = *cursor + 1;
		}
		break;
	default:
		int l = strlen(src);
		if (inp >= LLIMIT  && inp < ULIMIT && l + 1 < max) {
			strcpy(src + *cursor + 1, src + *cursor);
			src[*cursor] = inp;
			*cursor = *cursor + 1;
		}
	}
}

unsigned char Pack::calcCrc(FILE* file) {

	int sum = 0;
	char inp = 0;

	while (inp != EOF) {
		sum = (sum * 7) & 0xff;
		inp = fgetc(file);
		sum = (sum + inp) & 0xff;
	}

	return sum;
}

void Pack::fromFile(char *path) {
	FILE *sFile = fopen(path, "r");

	if (sFile == NULL) {
		fprintf(stderr, "Error reading data from %s\n", path);
		return;
	}

	sscanf(path, "%d.pack", &packId);

	// check crc
	int crc = calcCrc(sFile);

	if (crc == 0xff) {
		rewind(sFile);

		int l;
		l = fgetc(sFile);
		ncu = l;
		for (int c = 0; c < ncu && l != EOF; c++) {
			l = fgetc(sFile);
			name[c] = l + LLIMIT;
		}
		name[ncu] = '\0';
		ncu = strlen(name);

		l = fgetc(sFile);
		dcu = l;
		for (int c = 0; c < dcu && l != EOF; c++) {
			l = fgetc(sFile);
			dis[c] = l + LLIMIT;
		}
		dis[dcu] = '\0';
		dcu = strlen(dis);

		do {
			//printf("ok\n");
			newLevel();
		} while (!levels.back()->fromFile(sFile));

		//printf("---\n");
		popLevel();
		//printf("&&&\n");
	} else {
		fprintf(stderr, "%s is not valid pack file\n", path);
	}

	fclose(sFile);
}

void Pack::toFile() {

	if (levels.size() == 0) {
		return;
	}

	char *fpath = (char*) malloc(8);
	sprintf(fpath, "%02d.pack", packId);

	FILE *sFile = fopen(fpath, "w");

	if (sFile == NULL) {
		fprintf(stderr, "Error writing user data to %s\n", fpath);
		return;
	}

	// Name + Description
	int l = strlen(name);
	fputc(l, sFile);
	for (int c = 0; c < l; c++) {
		fputc(name[c] - LLIMIT, sFile);
	}

	l = strlen(dis);
	fputc(l, sFile);
	for (int c = 0; c < l; c++) {
		fputc(dis[c] - LLIMIT, sFile);
	}

	for (unsigned int c = 0; c < levels.size(); c++) {
		levels[c]->toFile(sFile);
	}

	fclose(sFile);

	// crc calcualation
	sFile = fopen(fpath, "r");
	char crc = 0xff - calcCrc(sFile);
	fclose(sFile);

	// crc appendtaiton
	sFile = fopen(fpath, "a");
	fputc(crc, sFile);
	fclose(sFile);

	free(fpath);
}

void Pack::gotoLevel(int i) {
	if (i < 0 || i >= (int)levels.size())
		return;

	mainMenu = 0;
	cs = 4;
	levelId = i;
}

void Pack::gotoMain() {
	mainMenu = 1;
	cs = levelId;
}

void Pack::quit() {
	if (mainMenu) {
		toFile();
		qui = 1;
	} else {
		gotoMain();
	}
}

void Pack::moveCursor(int i) {
	if (!mainMenu)
		return;
	if (cs + i < -2) {
		cs = -2;
		return;
	}
	if (cs + i == (int)levels.size()) {
		cs = (int)levels.size() - 1;
		return;
	}
	cs += i;
}

void Pack::newLevel() {
	Level *newLevel = new Level();

	levels.push_back(newLevel);
}

bool Pack::movLevel(int i, bool d) {
	int newPos = i + d * 2 - 1;
	if ( newPos < 0 || newPos >= (int)levels.size())
		return 0;

	Level *tmp = levels[i];
	levels[i] = levels[newPos];
	levels[newPos] = tmp;
	return 1;
}

void Pack::popLevel() {
	if (levels.size() == 0)
		return;

	delete levels[levels.size() - 1];

	levels.pop_back();
}

void Pack::delLevel(int i) {
	if (i < 0)
		return;

	while (movLevel(i++, 1)) ;
	i--;

	popLevel();

}


bool Pack::input(int inp) {
	bool MasterInput = 0;

	if (mainMenu) {
		inputMain(inp);
	} else {

		// if keyboard isnt locked, check if input is master input
		//(and take actions if it is)
		if (!keyboardLock) {
			MasterInput = inputMaster(inp);
		}

		// if input isnt master, pass it to the mode
		if (!MasterInput) {
			inputState(inp);
		}
	}

	return qui;
}

void Pack::inputMain(int inp) {
	bool type = keyboardLock;

	if (!keyboardLock) {
		int i = tolower(inp);

		switch (i) {
		case 'q' :
			quit();
			return;
		case 'n' :
			newLevel();
			gotoLevel(levels.size() - 1);
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
			break;
		case KEY_DC:
		case KEY_BACKSPACE:
			if (cs > 0) {
				delLevel(cs);
				moveCursor(0);
			} else
				type = 1;
			break;
		case '+':
			movLevel(cs, 1);
			moveCursor(1);
			break;
		case '-':
			movLevel(cs, 0);
			moveCursor(-1);
			break;
		default:
			type = 1;
		}
	}

	if (type) {
		switch (cs) {
		case -2:
			if ( inp == '\n') {
				keyboardLock = 0;
				cs = -1;
			} else {
				keyboardLock = 1;
				modString(name, inp, &ncu, PACK_NAME_LEN);
			}
			break;
		case -1:
			if ( inp == '\n') {
				keyboardLock = 0;
				cs = 0;
			} else {
				keyboardLock = 1;
				modString(dis, inp, &dcu, PACK_DIS_LEN);
			}
			break;
		}
	}
}

bool Pack::inputMaster(int inp) {
	inp = tolower(inp);
	switch (inp) {
	case 'q':
		quit();
		return 1;
	case '4':
		cs = 0;
		return 1;
	case '3':
		cs = 1;
		return 1;
	case '5':
		cs = 2;
		return 1;
	case '6':
		cs = 3;
		return 1;
	case '1':
		cs = 4;
		return 1;
	case '2':
		cs = 5;
		return 1;
	case '7':
		cs = 6;
		return 1;
	}

	return 0;
}

void Pack::inputState(int inp) {
	Level* cl = levels[levelId];

	switch (cs) {
	case 0:
		switch (inp) {
		case KEY_RIGHT:
			cl->movegCursor(cl->gx + 1, cl->gy);
			break;
		case KEY_LEFT:
			cl->movegCursor(cl->gx - 1, cl->gy);
			break;
		case KEY_DOWN:
			cl->movegCursor(cl->gx, cl->gy + 1);
			break;
		case KEY_UP:
			cl->movegCursor(cl->gx, cl->gy - 1);
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
		switch (inp) {
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
		switch (inp) {
		case KEY_RIGHT:
			cl->moveiCursor(cl->ix + 1, cl->iy);
			break;
		case KEY_LEFT:
			cl->moveiCursor(cl->ix - 1, cl->iy);
			break;
		case KEY_DOWN:
			cl->moveiCursor(cl->ix, cl->iy + 1);
			break;
		case KEY_UP:
			cl->moveiCursor(cl->ix, cl->iy - 1);
			break;
		case '\n':
		case ' ':
			cl->icursorSelect();
			break;
		}
		break;
	case 3:
		switch (inp) {
		case KEY_RIGHT:
			cl->movefCursor(cl->fx + 1);
			break;
		case KEY_LEFT:
			cl->movefCursor(cl->fx - 1);
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
			if (cs == 6)
				cs = 0;
		} else {
			keyboardLock = 1;
			if (cs == 4)
				cl->modName(inp);
			else
				cl->modDis(inp);
		}
		break;
	case 6:
		switch (inp) {
		case KEY_RIGHT:
			cl->movecCursor(1, 0);
			break;
		case KEY_LEFT:
			cl->movecCursor(-1, 0);
			break;
		case KEY_DOWN:
			cl->movecCursor(0, 1);
			break;
		case KEY_UP:
			cl->movecCursor(0, -1);
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
	if (mainMenu) {
		redrawMain(x, y);
	} else {
		color_set(1, NULL);

		move(x, y);
		printw(" --[ %-15.15s ]-- ", cursorStates[cs]);

		levels[levelId]->redraw(x, y + 2);
	}
}

void Pack::redrawMain(int x, int y) {
	xPos = x;
	yPos = y;

	move(y, x);
	printw("%s", name);
	move(y + 1, x);
	printw("%s", dis);

	for (unsigned int c = 0; c < levels.size(); c++) {
		move(y + c + 3, x + 2);
		printw("%s", levels[c]->name);
	}

	move(levels.size() + 3 + y, x);
	printw("------------------");

	for (unsigned int c = 0; c < KEY_BINDS; c++) {
		move(levels.size() + 5 + y + c, x);
		printw("[%5.5s] %s", keyBinds[c][0], keyBinds[c][1]);
	}
}

void Pack::getCursorPos(int *x, int *y) {
	if (mainMenu) {
		switch (cs) {
		case -2:
			*x = xPos + ncu;
			*y = yPos;
			break;
		case -1:
			*x = xPos + dcu;
			*y = yPos + 1;
			break;
		default :
			*x = xPos;
			*y = 3 + yPos + cs;
		}
	} else {
		levels[levelId]->getCursorPos(cs, x, y);
	}
}
