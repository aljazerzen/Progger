#include <vector>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

const int CLRS = 5;
const int INSTYPES = 3;
const char INSTYPE[]="|><";

const int TILES = 6;
const char TILE[] = ".+*#|-";

const char CURSOR[]=">/<\\";

const int NAME_LEN = 32;
const int DIS_LEN = 68;

const unsigned char LLIMIT = 32;
const unsigned char ULIMIT = 177;

using namespace std;

class Level{
private:
	int width,height;
	int cux,cuy,cud;

	void drawName(int x,int y);
	void drawDis(int x,int y);
	void drawWidHigh(int x, int y);
	void drawGrid(int x, int y);
	void drawIns(int x,int y);
	void drawFunct(int x,int y);

	void modString(char *src, int inp, int *cursor, int max);

	int xPos,yPos;

public:
	struct tile{
		int c;
		int t;
	};

	char name[NAME_LEN];
	int nc;
	char dis[DIS_LEN];
	int dc;

	void modName(int c);
	void modDis(int c);

	vector<vector <tile> > grid	;

	bool ins[CLRS][INSTYPES];

	// function sizes
	vector<int> funct;

	Level();

	void clearIns();
	void setIns();

	void toFile(FILE *file);
	bool fromFile(FILE *file);

	void setFunctSize(int s);
	void incfSize();
	void decfSize();

	void setSize(int w,int h);
	void incWidth();
	void decWidth();
	void incHeight();
	void decHeight();

	// grid cursor
	int gx,gy;
	void movegCursor(int x, int y);
	void gcursorSelect(int t);

	// instruction cursor
	int ix,iy;
	void moveiCursor(int x, int y);
	void icursorSelect();

	// function cursor
	int fx;
	void movefCursor(int x);
	void fcursorSelect(int t);

	// cursor cursor
	void movecCursor(int x, int y); //relative
	void rotatecCursor(int d); //relative

	void getCursorPos(int cs, int *cx, int *cy);

	void redraw(int x, int y);
};

