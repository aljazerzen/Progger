#include <vector>
#include <ncurses.h>


const int CLRS = 5;
const int INSTYPES = 5;

const int TILES = 4;

using namespace std;

class Opts{
private:
	int width,height;
	int x,y,d;

	void drawGrid(int x, int y);
	void drawWidHigh(int x, int y);
	void drawIns(int x,int y);
	void drawFunct(int x,int y);

	int xPos,yPos;

public:
	struct tile{
		int c;
		int t;
	};


	vector<vector <tile> > grid	;

	// allowed instruction count
	int aic;
	bool ins[CLRS][INSTYPES];

	// function sizes
	vector<int> funct;

	Opts();

	char* toString();
	void print();
	void toFile(char *path);

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

	void getCursorPos(int cs, int *cx, int *cy);

	void redraw(int x, int y);
};

