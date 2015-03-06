#include <vector>
#include <ncurses.h>


const int CLRS = 4;
const int INSTYPES = 5;

using namespace std;

class Opts{
private:
	int width,height;
	int x,y,d;

public:
	struct tile{
		int c;
		int t;
	};


	vector<vector <tile> > grid	;

	// allowed instruction count
	int aic;
	bool ins[CLRS][INSTYPES];

	//function sizes
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

	void redraw();
};

