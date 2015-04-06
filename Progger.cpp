// Author: Aljaz Erzen, 22.9.2014-?
// SDL code base made by Foo Productions

// Colors
// red: 	#CF3535
// green: 	#99C232
// yellow: 	#E6E34E
// blue:	#524CE3

//The headers
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <dirent.h>

using namespace std;

// ----[ DEFINED PARAMETERS ]----

// config path
const char CONFIG_PATH[] = "./Config/";

// pack structure
const int NAME_LENGTH=32;
const int DIS_LENGTH=68;
const int CON_SIZE=1024;
const int PROG_SIZE=1024;

const unsigned char LLIMIT = 32;
const unsigned char ULIMIT = 177;

const char *menuFile="menus";

const int MENU_CURSOR_PADDING=5;

/*Instruction id:
	- Color(mask 0x0000FF00)
	- Type (mask 0x000000FF): */
const int INS_BLANK 	= 0;
const int INS_FORWARD	= 1;
const int INS_LEFT		= 2;
const int INS_RIGHT		= 3;
//		  INS_SETCOLOR_N= 4+N;
//		  INS_FUNCT_N	= 20+N;
const int INS_TYPE_COUNT= 4; //without fuctions and colors
const int COLOR_NUMBER 	= 5;

const int INS_SPRITE_TYPES=15;
const int INS_SPRITES = INS_SPRITE_TYPES*COLOR_NUMBER;
int INS_SPRITE_SIZE = 0;

const int MAX_FUNCTION_LENGTH=20;
const int MAX_FUNCTION_COUNT=10;

/*Grid tiles:
	- Color mask(0x0000FF00)
	- Type  mask(0x000000FF): */
const int TIL_BLANK		= 0;
const int TIL_TILE		= 1;
const int TIL_STAR		= 2;
const int TIL_BLOCK		= 3;
const int TIL_VERTICAL 	= 4;
const int TIL_HORIZONTAL= 5;

const int MAX_GRID_WIDTH=20;
const int MAX_GRID_HEIGHT=20;

const int GRID_SPRITES=30;

const int CTRL_SPRITES=6;

const int SCREEN_BPP = 32;

const int RESOLUTION_COUNT=5;
//sorted increasingly
const int SUPPORTED_RESOLUTIONS[RESOLUTION_COUNT][3]={
	{640,480,2},
	{1280,720,4},
	{1280,920,4},
	{1600,900,4},
	{1920,1080,4}
};
const char GRAPHICS_PATH[][32]={
	"640x480/",
	"1280x960/",
	"1280x960/",
	"1280x960/",
	"1280x960/"
}; 
// normal, small, tiny
const int FONT_SIZES[]={10,7,3,30};

// defined sizes giving structure to Level (changing with resolution )
const int DEF_PROGRAM_QT_SIZE=12;
const int DEF_PROGRAM_FU_SIZE=25;
const int DEF_PROGRAM_CB_SIZE=30;

const int DEF_PROGRAM_IB_MARGIN=5;
const int DEF_PROGRAM_CB_MARGIN=5;
const int DEF_GRID_MARGIN=3;

// scaled to match screen (no need to change when changing resolution)
const int PROGRAM_QT_PADDING=30;
const int PROGRAM_FU_PADDING=4; 
const int PROGRAM_IB_PADDING=4; 
const int PROGRAM_CB_PADDING=4;

//The frame rate
const int FRAMES_PER_SECOND = 120;

const SDL_Rect CB_POSITIONS[]= {
	{0,0+	0	,50,50},
	{0,50+	4	,50,50},
	{0,100+	8	,50,50},
	{0,150+	12	,50,25},
	{0,222+	0	,50,25},
	{0,247+	4	,50,50}
};
const int DEF_CB_WIDTH=58;
const int DEF_CB_HEIGHT=316;

int DEF_POP_TTC = 5000;
int DEF_POP_PAD = 5;
int DEF_POP_LINE_SPACING = 3;
int DEF_POP_X = 10;
int DEF_POP_Y = 10;
bool DEF_POP_COC = 1;

char stopMessages[][64] = {
	"",
	"I have fallen out of the grid. More luck next time",
	"Oops. I have no more tiles to be on...\0",
	"... What should I do next? I ran out of instructions!"
};

char winMessage[] = "Yeah! Level complete! Progressing to new one...";
char packCompleteMessage[] = "Well that was the last one. Pack's done!";

// ----[ AUTO-ADJUSTABLE PARAMETERS]----

char PACKSTRUCT_PATH[]="./Packs/";

int GRD_SPRITE_SIZE=0;

int CTRL_SPRITE_SIZE=0;

int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;

int MENU_SCALE_FACTOR = 0;
int FONT_SIZE_NORMAL = 0;
int FONT_SIZE_SMALL = 0;
int FONT_SIZE_TINY = 0;
int FONT_SIZE_BIG = 0;

int PROGRAM_QT_SIZE=0;
int PROGRAM_FU_SIZE=0;
int PROGRAM_CB_SIZE=0;

int PROGRAM_IB_MARGIN=0;
int PROGRAM_CB_MARGIN=5;
int GRID_MARGIN=10;

// ----[ OPTIONS ]----

bool SCREEN_FULLSCREEN = 1;

int CURR_RESOLUTION=-1;

// POS: 0 Program is on the top of the screen, 1 Program is at the bottom, -1: not shown
int PROGRAM_POS=1;

int QUEUE=0;

bool ENABLE_TRANSITIONS=1;

bool WAIT_ONLY_ON_MOVE=1;

int SPEED=3;

class cTexture {
	private:
		int tH, tW;
		SDL_Texture *sTexture;

	public:
		cTexture();
		~cTexture();

		bool loadFromFile(const char *path, Uint8 kred = 0, Uint8 kgreen = 0, Uint8 kblue = 0);
		bool loadFromRenderedText( const char *text, SDL_Color textColor , TTF_Font *font);
		void loadFromSurface(SDL_Surface *src, SDL_Rect *clip);
		void free();

		void setColor(Uint8 red, Uint8 green, Uint8 blue);
		void setBlendMode(SDL_BlendMode mode);
		void setAlpha(Uint8 alpha);

		void render(int x, int y, SDL_Rect *clip=NULL, double angle=0.0f, SDL_Point *center=NULL, SDL_RendererFlip flip=SDL_FLIP_NONE);

		int getW();
		int getH();
		bool isLoaded();
};

class Timer {
	private:
		//The clock time when the timer started
		int startTicks;

		//The ticks stored when the timer was paused
		int pausedTicks;

		//The timer status
		bool paused;
		bool started;

	public:
		//Initializes variables
		Timer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		int get_ticks();

		//Checks the status of the timer
		bool is_started();
		bool is_paused();
};

class Level {
	private:
		// ----[ logical ]----
		bool loaded;

		int levelId;

		// content
		char *con;

		// user written program encoded
		char *prog;

		int starsToGet;

		struct insEle{
			int ins;
			insEle *n;
		};
		insEle *nextIns;
		int insCount;

		int insBar[COLOR_NUMBER][INS_TYPE_COUNT+MAX_FUNCTION_COUNT];
		int insBarHeight, insBarWidth;

		//current ticksto step, number of ticks waiting for next instruction (step delay)
		int cTicks,ticksAStep;
		int speed;
		bool started,paused;

		int fCnt;
		int funct[MAX_FUNCTION_COUNT][MAX_FUNCTION_LENGTH];
		int functL[MAX_FUNCTION_COUNT];
		//function cursor: function, index
		int fCf,fCi;

		int gxSize, gySize;
		int grd[MAX_GRID_WIDTH][MAX_GRID_HEIGHT];
		//cusror's x position, y position and direction (0 right, 1 down, 2 left, 3 up)
		int cX, cY, cD;

		//instruction bar cursor type and color
		int ibCt,ibCc;

		//control bar cursor 
		int cbC;

		void loadFunction(insEle **e, int f);
		int getNextInstruction();

		int addFunction(int l);
		int removeFunction(int index);	
		int setInstruction(int funct,int index,int ins);

		void selectFuInstruction();
		void moveFuCursorTo(int funct, int index);

		void selectIbInstruction();
		void moveIbCursorTo(int type, int color);
		void closeIb();

		void unfocusProgram();
		void unfocusCb();

		void selectCbCursor();
		void moveCbCursor(int ele);

		void incSpeed();
		void decSpeed();
		
		// ----[ visual ]----

		//program height, program position- 0: top 	1: bottom -1:none 
		int programH,programW,programX,programY,programPos;
		bool recalculateNeeded;

		//function's height, width, x position
		int fuS, fuW, fuH, fuX, fuY, fuMLen;
		cTexture fuTexture;

		//queue transtion state - 0:hidden 	32:visible  (linear)
		//queue transition modifier - 0:hidden 	1024:visible  (sqaured)
		//width, height, x position
		int quTState,quTMod,quW,quH,quX,quY,quAlpha;
		cTexture quTexture;

		//holding program background
		cTexture bgTexture;

		//grid
		int gridS, gridX, gridY, gridW, gridH;
		cTexture gTexture;

		// offset for moving animations 
		// *LM = last move amount
		// (when moved lm offset is set to some value offset is then being adjusted)
		// cDoffset's unit is deg ( 90 - 0 or -90 to 0)
		int cXoffset, cYoffset, cDoffset; 
		int cXoffsetLM, cYoffsetLM, cDoffsetLM; 

		// instruction sidebar & background
		// state 0:hidden or showing 	1: shown or hiding
		// target state
		// instruction sidebar's position, size, alpha
		int ibState,ibTState,ibX,ibY,ibH,ibW,ibS,ibAlpha;
		cTexture ibTexture;
		cTexture ibBTexture;

		// control bar & background
		// cbVP & cbHP: vertical && horizontal padding
		int cbS,cbW,cbH,cbX,cbY,cbVP,cbHP;
		cTexture cbTexture;
		cTexture cbBTexture;

		SDL_Rect calcPositionsGrid(int px, int py, int ax, int ay);
		SDL_Rect calcPositionsQueue(int px, int py, int ax, int ay);
		SDL_Rect calcPositionsFunctions(int px, int py, int ax, int ay);
		SDL_Rect calcPositionsInstructionBar(int px, int py, int ax, int ay);
		SDL_Rect calcPositionsControlBar(int px, int py, int ax, int ay);

		void calcPositions();
		void rebuildQuTexture();
		void rebuildFuTexture();
		void rebuildBgTexture();
		void rebuildGTexture();
		void rebuildIbTexture();
		void rebuildCbTexture();

	public:
		// ----[ logical ]----
		Level(int id, char *co, char *prog, int pP);
		void close();
		void save();
		void win();

		int getLevelId();

		bool isLoaded();
		int gridReset(bool resetFunctions);
		
		void start();
		void stop(int stopCode);
		void togglePause();
		void tick();

		void moveRobo(int x, int y);
		void rotateRobo(int d);

		void onClick(SDL_Event *e);
		void onMouseMove(SDL_Event *e);
		void onKeyPress(SDL_Event *e);

		void setSpeed(int s);

		// ----[ visual ]----
		void show(int alpha);
};

class Menu {
	private:
	struct Element{
		char name[NAME_LENGTH];
		SDL_Rect box;  	// position on the screen
		SDL_Rect clip;  // clip of menu surface
		SDL_Rect clip2; // additional clip, for elements with diffrent states 
		cTexture *texture;
		int event;
		int type;
		int value;
		/* value: additional information
			type:
				1 )	0x00FF : pack
					0xFF00 : puzzle
		*/	
	};

	vector <Element> ele;
	cTexture cursorTexture;
	SDL_Rect cursorBox;

	int cursor;
	int cState;		//transition's state  		0 -  32 (linear)
	int cAmpf; 		//transition's amplifier	0 - 256 (square)
	
	int scroll;
	int tScroll;

	// height of the whole menu
	int height;

	public:
	Menu();
	void addElement(const char *n, int x, int y, int w, int h, int ev, int px, int py, int t, long a);
	void makeTexture(int eleIndex);

	void onClick(SDL_Event *e);
	void onMouseEnter(SDL_Event *e);
	void onKeyPress(SDL_Event *e);
	void onScroll(SDL_Event *e);

	void openMenu(int menuIndex);
	void closeMenu();

	void tick();
	void moveCursorTo(int index);
	void cursorSelect();

	void rebuildCursorTexture();
	void show(int alpha, SDL_Rect *box);
};

class Structure {
	private:
		class Package {
			private:
				class Puzzle{
					private:
						char name[NAME_LENGTH];
						char dis[DIS_LENGTH];
						char con[CON_SIZE];
						char prog[PROG_SIZE];
						bool done;

					public:
						Puzzle(char *n, char *d, char *co) {
							name[0]='\0';
							dis[0]='\0';
							con[0]='\0';
							strcat(name,n);	
							strcat(dis,d);	
							strcat(con,co);	
							prog[0]='\0';
							done=0;
						}
						char* getName() {return name;}
						char* getDis() {return dis;}
						char* getCon() {return con;}
						char* getProg() {return prog;}
						bool getDone() {return done;}
				
						void setProg(char *p){
							prog[0]='\0';
							strncat(prog,p,PROG_SIZE);
						}
						void setDone(bool d){done=d;}
				};

				char name[NAME_LENGTH];
				char dis[DIS_LENGTH];
				bool loaded;
				bool done;

				const char *path;

				vector<Puzzle*> puzzles;
			public:
				Package(const char *p) {
					name[0]='\0';	
					dis[0]='\0';	
					path=p;
					loaded=0;
					done=0;
				}

				char* getName() {return name;}
				char* getDis() {return dis;}
				bool isLoaded() {return loaded;}
				bool getDone();
				int getPuzzleCount() {return puzzles.size();}
					void addPuzzle(char *n, char *d, char *co) {
					Puzzle *newPuzz = new Puzzle(n, d, co);
					puzzles.push_back(newPuzz);
				}		

				unsigned char calcCrc(FILE* file);

				void load();
		
				char* getPuzzleName(unsigned int puzzle);
				char* getPuzzleDis(unsigned int puzzle);
				char* getPuzzleCon(unsigned int puzzle);
				char* getPuzzleProg(unsigned int puzzle);
				bool getPuzzleDone(unsigned int puzzle);

				void setPuzzleProg(unsigned int puzzle, char *p);
				void setPuzzleDone(unsigned int puzzle, bool d);
		};

		vector<Package*> packages;
		const char *path;

		void addPackage(char *filename);
	public:
		Structure(const char *pa);
		void load();
		void clear();

		void loadUserData();
		void saveUserData();

		int getPackageCount();
		int getPuzzleCount(unsigned int pack);

		char* getPackageName(unsigned int pack);
		char* getPuzzleName(unsigned int pack, unsigned int puzzle);
		
		char* getPackageDis(unsigned int pack);
		char* getPuzzleDis(unsigned int pack, unsigned int puzzle);

		char* getPuzzleCon(unsigned int pack, unsigned int puzzle);
		
		char* getPuzzleProg(unsigned int pack, unsigned int puzzle);
		
		bool getPuzzleDone(unsigned int pack, unsigned int puzzle);

		void setPuzzleProg(unsigned int pack, unsigned int puzzle, char *p);
		void setPuzzleDone(unsigned int pack, unsigned int puzzle, bool d);
};

class Popup{
	private:
		SDL_Rect box;
		cTexture texture;

		int ticksToClose;

		bool active;
		bool closeOnClick;

		char lines[256][256]; 
		int linesCount;

	public:

		int PADDING;
		int LINE_SPACING;

		TTF_Font *font;
		int fontSize;
		SDL_Color textColor, backgroundColor;
		bool backgroundColorIsFromTime;
		bool centerx;

		Popup(int x, int y, int stc);

		Popup(int x, int y, SDL_Surface *src, int stc);
		Popup(int x, int y, char *text, int maxW, TTF_Font *fnt, int fntSize, SDL_Color tClr, SDL_Color bClr, int stc);
		void init(int x, int y, int stc);

		void loadLines(char *text, int maxW);
		void makeFromLines();

		void loadTexture(SDL_Surface *src);

		void close();

		void onClick(SDL_Event *e);
		void show();
		void tick();

		bool isActive();
		SDL_Rect getBox();

		void moveTo(SDL_Rect *pos);
		void setCloseOnClick();
		void setCloseOnClick(bool coc);
};

class PopupHandler {
	private:
		vector<Popup*> popups;

	public:

		int defTTC;

		int DEF_X;
		int DEF_Y;

		int DEF_PADDING;
		int DEF_LINE_SPACING;

		TTF_Font *defFont;
		int defFontSize;
		SDL_Color defTextColor, defBackgroundColor;
		bool defBackgroundColorIsFromTime;
		bool defCenterx;

		bool DEF_COC;

		PopupHandler();

		void addPopup(int x, int y);
		void addPopup(int x, int y, SDL_Surface *src);
		void addPopup(char *text);

		Popup* getPopup(int i);
		Popup* getLastPopup();

		void show();
		void tick();

		void onClick(SDL_Event *e);
};

void applyTextToSurface(int x, int y, SDL_Surface* dst, const char *text, TTF_Font* font, SDL_Color *color,int alpha=255);
void applySurface(SDL_Surface* src, SDL_Rect* clip, SDL_Surface* dst, SDL_Rect* offset);

void loadSettings();
void saveSettings();

int loadMenus();
void unloadMenus();

void changeResolution(int resolution);
void changeFullscreen(int fullscreen);

void replace(char *target, char s1, char s2);

int getColor();

//The surfaces
cTexture backGround;
cTexture robo;
SDL_Surface *screen = NULL;
SDL_Surface *menu = NULL;
SDL_Surface *grdTiles = NULL;
SDL_Surface *insTiles = NULL;
SDL_Surface *ctrlTiles = NULL;

SDL_Rect gridClips[GRID_SPRITES];
SDL_Rect insClips[INS_SPRITES];
SDL_Rect ctrlClips[CTRL_SPRITES];

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;	

TTF_Font *fontNormal = NULL;
TTF_Font *fontSmall = NULL;
TTF_Font *fontTiny = NULL;
TTF_Font *fontBig = NULL;

//The event structure
SDL_Event event;

int state=0,tState=0,stateTP=0;
/*
- 0x0F 	State type:	
	0 Blank(background)
	1 Menus
	2 In-game screen
- 0xF0 	Sub-type
	(menu id or level id)

target state of transition
state transition progress, 0 - none, 16 - Black, 32 - Done */

Structure packStructure(PACKSTRUCT_PATH);
PopupHandler *popupHandler;

vector <Menu*> menus;

int packMenuIndex=0;

bool quit = false;

int insToSprite(int ins) {
	int type=ins & 0xff;
	int color=(ins & 0xff00)>>8;
	//if(ins< INS_TYPE_COUNT+COLOR_NUMBER) {
	if(type<INS_TYPE_COUNT) {
		return type+color*INS_SPRITE_TYPES;
	}
	if(type>=20) {
		//			change that when adding instructions
		//              / 
		return type-20+ INS_TYPE_COUNT + color*INS_SPRITE_TYPES;
	}
	return 0;
}

int spriteToIns(int type,int color) {
	if(type<0 || type>INS_SPRITE_TYPES || color<0 || color>= COLOR_NUMBER) {
		return 0;
	}
	color<<=8;

	if(type<INS_TYPE_COUNT) 
		return type | color;
	else
		return (type+20-INS_TYPE_COUNT) | color;

}

cTexture::cTexture() {
	sTexture = NULL;
	tH=0;
	tW=0;
}

cTexture::~cTexture() {
	free();
}

bool cTexture::loadFromFile(const char *path, Uint8 kred, Uint8 kgreen , Uint8 kblue) {
	free();

	SDL_Surface *fromFile = IMG_Load( path );
	if(fromFile == NULL) return 1;
	
	SDL_SetColorKey(fromFile, SDL_TRUE, SDL_MapRGB(fromFile->format, kred, kgreen, kblue));
	
	SDL_Texture *newTexture = SDL_CreateTextureFromSurface( gRenderer, fromFile);
	if(newTexture != NULL) {
		tW = fromFile->w;
		tH = fromFile->h;
	}

	SDL_FreeSurface(fromFile);
	
	sTexture=newTexture;
	return newTexture==NULL;
}

bool cTexture::loadFromRenderedText( const char *text, SDL_Color textColor , TTF_Font *font) {
	free();

	SDL_Surface *fromFont = TTF_RenderText_Solid( font, text, textColor);
	if(fromFont == NULL) return 1;
	
	SDL_Texture *newTexture = SDL_CreateTextureFromSurface( gRenderer, fromFont);
	if(newTexture != NULL) {
		tW = fromFont->w;
		tH = fromFont->h;
	}

	SDL_FreeSurface(fromFont);

	sTexture=newTexture;
	return newTexture==NULL;
}

void cTexture::loadFromSurface(SDL_Surface *src, SDL_Rect *clip=NULL) {
	free();

	SDL_Surface *srcClipped=NULL;

	if(clip != NULL ) {
		srcClipped= SDL_CreateRGBSurface(0, clip->w, clip->h, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
		SDL_BlitSurface( src, clip, srcClipped, NULL);
	} else {
		srcClipped=src;
	}

	sTexture = SDL_CreateTextureFromSurface( gRenderer, srcClipped);
	if(sTexture != NULL) {
		tW = src->w;
		tH = src->h;
	}

	if(clip != NULL) {
		SDL_FreeSurface(srcClipped);
	}
}

void cTexture::free() {
	if(sTexture!=NULL) {
		SDL_DestroyTexture(sTexture);
		sTexture=NULL;
	}
	tW=0;
	tH=0;
}

void cTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	SDL_SetTextureColorMod(sTexture, red, green, blue);
}

void cTexture::setBlendMode(SDL_BlendMode mode) {
	SDL_SetTextureBlendMode(sTexture, mode);
}

void cTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod( sTexture, alpha);
}

void cTexture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) {
	if(sTexture==NULL) return;

	SDL_Rect dstRect = { x, y, tW, tH };
	
	if( clip != NULL ) {
		dstRect.w = clip->w;
		dstRect.h = clip->h;
	}
	SDL_RenderCopyEx( gRenderer, sTexture, NULL, &dstRect, angle, center, flip );
}

int cTexture::getH() {
	return tH;
}

int cTexture::getW() {
	return tW;
}

bool cTexture::isLoaded() {
	return sTexture!=NULL;
}

Timer::Timer() {
	//Initialize the variables
	startTicks = 0;
	pausedTicks = 0;
	paused = false;
	started = false;
}

void Timer::start() {
	//Start the timer
	started = true;

	//Unpause the timer
	paused = false;

	//Get the current clock time
	startTicks = SDL_GetTicks();
}

void Timer::stop() {
	//Stop the timer
	started = false;

	//Unpause the timer
	paused = false;
}

void Timer::pause() {
	//If the timer is running and isn't already paused
	if( ( started == true ) && ( paused == false ) )
	{
		//Pause the timer
		paused = true;

		//Calculate the paused ticks
		pausedTicks = SDL_GetTicks() - startTicks;
	}
}

void Timer::unpause() {
	//If the timer is paused
	if( paused == true ) {
		//Unpause the timer
		paused = false;

		//Reset the starting ticks
		startTicks = SDL_GetTicks() - pausedTicks;

		//Reset the paused ticks
		pausedTicks = 0;
	}
}

int Timer::get_ticks() {
	//If the timer is running
	if( started == true ) {
		//If the timer is paused
		if( paused == true ) {
			//Return the number of ticks when the timer was paused
			return pausedTicks;
		} else {
			//Return the current time minus the start time
			return SDL_GetTicks() - startTicks;
		}
	}

	//If the timer isn't running
	return 0;
}

bool Timer::is_started() {
	return started;
}

bool Timer::is_paused() {
	return paused;
}

Level::Level(int id, char *co, char *p, int pP) {
	levelId=id;
	con=co;
	prog=p;

	started=0;
	quTState=0;
	quTMod=0;
	paused=0;
	fCnt=0;
	nextIns=NULL;
	insCount=0;
	ibState=0;
	ibTState=0;
	programPos=pP;
	recalculateNeeded=1;
	fCf=0;
	fCi=0;
	ibCt=0;
	ibCc=0;
	cbC=-1;

	memset(functL,0,MAX_FUNCTION_COUNT*sizeof(int));

	setSpeed(SPEED);
	loaded=0;
	if(gridReset(1)==0) {
		loaded=1;
	}
}

void Level::loadFunction(insEle **e, int f) {
	insEle *pr=(*e);
	for(int c=functL[f]-1;c>=0;c--) {
		if(funct[f][c]!=0) { 
			*e=(insEle*) malloc(sizeof(insEle));
			(*e)->ins=funct[f][c];
			(*e)->n=pr;
			pr=*e;

			insCount++;
		}
	}
}

int Level::getNextInstruction() {
	if(nextIns==NULL) return -1;

	int r=nextIns->ins;
	insEle *ne=nextIns->n;
	free(nextIns);
	nextIns=ne;

	insCount--;
	recalculateNeeded=1;
	quTexture.free();

	return r;
}

int Level::addFunction(int l) {
	if(fCnt>=MAX_FUNCTION_COUNT) 
		return -1;
	if(l>MAX_FUNCTION_LENGTH) {
		l=MAX_FUNCTION_LENGTH;
	}
	if(l<0) {
		l=0;
	}
	
	//prepares function
	functL[fCnt]=l;
	memset(funct[fCnt],0x0,l*sizeof(int));

	recalculateNeeded=1;
	fuTexture.free();
	ibTexture.free();

	return ++fCnt;
}
	
int Level::removeFunction(int index=-1) {
	if(index<0) {
		index=fCnt+index;
	}

	if(fCnt<=0) {
		return -1;
	}
	if(index>=fCnt) {
		return -2;
	}
	
	for(int f=index;f+1<fCnt;f++) {
		functL[f]=functL[f+1];
		for(int c=0;c<functL[f];f++) {
			funct[f][c]=funct[f+1][c];
		}
	}
	fCnt--;
	functL[fCnt]=0;

	recalculateNeeded=1;
	fuTexture.free();
	ibTexture.free();

	return 0;
}

int Level::setInstruction(int fnc,int index,int ins) {
	if(functL[fnc]<=index) {
		return 1;
	}
	if(ins<0) {
		return 2;
	}
	
	funct[fnc][index]=ins;
	return 0;
}

void Level::selectFuInstruction() {
	if(started==0 && fCf>=0 && fCi>=0) {
		ibTState=8;
		ibTexture.free();
	}
}

void Level::moveFuCursorTo(int funct, int index) {
	if(funct>=fCnt) return; 
	if(funct<0) return;
	if(index>=functL[funct]) return; 
	if(index<0) {
		cbC=0;
		unfocusProgram();
		return;
	}
	fCf=funct;
	fCi=index;

	fuTexture.free();
}

void Level::selectIbInstruction() {
	if(ibCc<0 && ibCt<0) return;

	funct[fCf][fCi]= insBar[ibCc][ibCt];
	ibTState=0;
	ibTexture.free();
	fuTexture.free();
}

void Level::moveIbCursorTo(int type, int color) {
	if(type<0) {
		cbC=0;
		unfocusProgram();
		return;
	}
	if(type>=insBarWidth) return;

	if(color<0) return;
	if(color>=insBarHeight) return;

	ibCt=type;
	ibCc=color;

	ibTexture.free();
}

void Level::closeIb() {
	ibTState=0;
	ibTexture.free();
	fuTexture.free();
}

void Level::unfocusProgram() {
	fCi=-1;
	fCf=-1;
	closeIb();
	cbTexture.free();
}

void Level::unfocusCb() {
	cbC=-1;
	cbTexture.free();
	fuTexture.free();
}

void Level::selectCbCursor() {
	if(cbC<0) return;

	switch(cbC) {
		case 0:
			start();
			break;
		case 1:
			togglePause();
			break;
		case 2:
			stop(0);
			break;
		case 3:
			setSpeed(speed+1);
			break;
		case 4:
			setSpeed(speed-1);
			break;
		case 5:
			close();
			break;
	}
}

void Level::moveCbCursor(int ele) {
	unfocusProgram();
	if(ele<0) return;
	if(ele>6) return;

	cbC=ele;

	cbTexture.free();
}

void Level::incSpeed() {
	ticksAStep-=FRAMES_PER_SECOND>>1;
	if(ticksAStep<0) ticksAStep=0;
}

void Level::decSpeed() {
	ticksAStep+=FRAMES_PER_SECOND>>1;
}

// px,py: place availibale	ax,ay : coordinates of 0,0 in available place absolute to window
// returns place allocated
SDL_Rect Level::calcPositionsGrid(int px, int py, int ax, int ay) {
	if(gxSize==0 || gySize==0) {
		SDL_Rect ret={ ax, ay, px, py };
		return ret;
	}

	int tSizex = (px - 2 * GRID_MARGIN)/gxSize;
	int tSizey = (py - 2 * GRID_MARGIN)/gySize;

	if(tSizex<tSizey) 
		gridS=tSizex;
	else
		gridS=tSizey;

	gridW=gridS*gxSize;
	gridH=gridS*gySize;
	
	// center align (all place allocated)
	gridX=((px - gridW)>>1) + ax;
	gridY=((py - gridH)>>1) + ay;

	SDL_Rect ret = {gridX - GRID_MARGIN, gridY - GRID_MARGIN, gridW + 2*GRID_MARGIN, gridH + 2*GRID_MARGIN};
	return ret;
}

SDL_Rect Level::calcPositionsQueue(int px, int py, int ax, int ay) 	{
	if(!quTState) {
		SDL_Rect ret={ ax, ay, px, 0 };
		return ret;
	}

	quW=insCount*PROGRAM_QT_SIZE;
	if(quW<px) {
		quX=(px-quW)>>1;
	} else {
		quX=0;
	}
	quX+=ax;
	quY=ay;

	quH=PROGRAM_QT_SIZE; 

	//height and alpha change due to transition (quTMod==0 > height=0, quTMod==1024 > height=TILE_SIZE+PADDING, linear)
	quAlpha=quTMod*0xFF/1024;

	bgTexture.free();

	SDL_Rect ret = {quX, quY, quW, quTMod*PROGRAM_QT_SIZE/1024};
	return ret;
}

SDL_Rect Level::calcPositionsFunctions(int px, int py, int ax, int ay) 	{
	if(fCnt<=0) {
		SDL_Rect ret={ ax, ay, px, 0 };
		return ret;
	}

	fuMLen=0;
	for(int f=0;f<fCnt;f++) 
		if(fuMLen<functL[f]) fuMLen=functL[f];

	int ys = py / fCnt;
	int xs = px / fuMLen;
	int ls = PROGRAM_FU_SIZE;

	if(xs<ys) 
		fuS=xs;
	else
		fuS=ys;

	if(fuS>ls)
		fuS=ls;

	fuW = fuS * fuMLen;
	fuH = fuS * fCnt;
	
	fuX = (px-fuW)>>1;
	if(fuX<0) fuX=0;
	fuX += ax;
	fuY = ay;

	SDL_Rect ret = {fuX, fuY, fuW , fuH};
	return ret;
}

SDL_Rect Level::calcPositionsInstructionBar(int px, int py, int ax, int ay) {
	if(!ibState || !insBarHeight || !insBarWidth) {
		SDL_Rect ret={ ax, ay, px, py };
		return ret;
	}

	int xs= (px - 2*PROGRAM_IB_MARGIN) / insBarWidth;
	int ys= (py - 2*PROGRAM_IB_MARGIN) / insBarHeight;

	if(xs<ys) 
		ibS=xs;
	else
		ibS=ys;

	ibW = ibS * insBarWidth;
	ibH = ibS * insBarHeight;
	ibX = ((px-ibW)>>1) + ax;
	ibY = ((py-ibH)>>1) + ay;
	ibAlpha= ibState*255/8;

//	ibTexture.free();

	SDL_Rect ret = {ibY - PROGRAM_IB_MARGIN, ibX - PROGRAM_IB_MARGIN, ibW + 2 * DEF_PROGRAM_IB_MARGIN, ibH + 2 * DEF_PROGRAM_IB_MARGIN};
	return ret;
}

SDL_Rect Level::calcPositionsControlBar(int px, int py, int ax, int ay) {
	int xs = px - 2*PROGRAM_CB_MARGIN;
	int ys = (py - 2*PROGRAM_CB_MARGIN) / 6;
	
	if(xs < ys) 
		cbS=xs;
	else
		cbS=ys;

	if(cbS > PROGRAM_CB_SIZE)
		cbS=PROGRAM_CB_SIZE;

	cbH = cbS * 6;
	cbW = cbS;
	cbX = PROGRAM_CB_MARGIN + ax;
	cbY = (py - cbH)/2 + ay;

	SDL_Rect ret = {ax , ay, cbW + 2 * PROGRAM_CB_MARGIN,py};
	return ret;
}

void Level::calcPositions() {
	int pX= SCREEN_WIDTH, pY=SCREEN_HEIGHT;

	SDL_Rect r;

	// ----[ Control bar ]----

	r=calcPositionsControlBar(pX, pY, 0, 0);
	pX-=r.w;
	int cbEnd= r.x + r.w;


	// ----[ Function, queue and grid ]----

	if( programPos==-1 ) {
		r=calcPositionsGrid(pX, pY, cbEnd, 0);
	}
	if( programPos== 0 ) {
		programX=cbEnd;
		programY=0;
		programH=0;
		programW=pX;

		r=calcPositionsFunctions(pX,pY,cbEnd, 0);
		programH+=r.h;
		pY-=r.h;

		r=calcPositionsQueue(pX, pY, cbEnd, r.h);
		programH+=r.h;
		pY-=r.h;

		r=calcPositionsGrid(pX, pY, cbEnd, programH);
		r=calcPositionsInstructionBar(pX,pY,cbEnd,programH);
	}
	if(programPos == 1) {
		programX=cbEnd;
		programY=0;
		programH=0;
		programW=pX;

		// to get information on space used
		r=calcPositionsFunctions(pX,pY,cbEnd, 0);
		programH+=r.h;
		r=calcPositionsQueue(pX, pY, cbEnd, 0);
		programH+=r.h;

		programY=pY-programH;

		r=calcPositionsFunctions(pX,programH,cbEnd, programY);
		r=calcPositionsQueue(pX, pY, cbEnd, programY+r.h);
		pY-=programH;

		r=calcPositionsGrid(pX, pY, cbEnd, 0);
		r=calcPositionsInstructionBar(pX,pY,cbEnd,0);
	}

	recalculateNeeded=0;
}

void Level::rebuildQuTexture() {
	SDL_Surface *canvas = SDL_CreateRGBSurface(0, (INS_SPRITE_SIZE+2*PROGRAM_QT_PADDING)*insCount, (INS_SPRITE_SIZE+2*PROGRAM_QT_PADDING), SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	insEle *e=nextIns;
	int c=0;
	SDL_Rect offset = {0,PROGRAM_QT_PADDING,INS_SPRITE_SIZE,INS_SPRITE_SIZE};
	
	while (e!=NULL && c<insCount) {
		offset.x= c*(INS_SPRITE_SIZE+2*PROGRAM_QT_PADDING)+PROGRAM_QT_PADDING;
		SDL_BlitSurface( insTiles, &insClips[insToSprite(e->ins)], canvas, &offset);		
				
		e=e->n;
		c++;
	}

	//converting to texture
	quTexture.loadFromSurface(canvas);
	quTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	
	SDL_FreeSurface(canvas);
}

void Level::rebuildFuTexture() {
	SDL_Surface *canvas = SDL_CreateRGBSurface(0, (INS_SPRITE_SIZE+2*PROGRAM_FU_PADDING)*fuMLen, (INS_SPRITE_SIZE+2*PROGRAM_FU_PADDING)*fCnt, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	//SDL_Rect offset={0,0,0,0};

	SDL_Rect offset={fCi*(INS_SPRITE_SIZE+PROGRAM_FU_PADDING*2)+PROGRAM_FU_PADDING,fCf*(INS_SPRITE_SIZE+PROGRAM_FU_PADDING*2)+PROGRAM_FU_PADDING,INS_SPRITE_SIZE,INS_SPRITE_SIZE};
	SDL_FillRect(canvas, &offset, SDL_MapRGBA(canvas->format, 0xAF, 0xAF, 0xAF, 0xFF));

	for(int f=0;f<fCnt;f++) {
		offset.y=f*(INS_SPRITE_SIZE+PROGRAM_FU_PADDING*2)+PROGRAM_FU_PADDING;
		for(int c=0;c<functL[f];c++) {
			offset.x=c*(INS_SPRITE_SIZE+PROGRAM_FU_PADDING*2)+PROGRAM_FU_PADDING;
			SDL_BlitSurface( insTiles, &insClips[ insToSprite(funct[f][c]) ], canvas, &offset);		
		}		
	}

	//converting to texture
	fuTexture.loadFromSurface(canvas);
	fuTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	
	SDL_FreeSurface(canvas);
}

void Level::rebuildBgTexture() {
	SDL_Surface *canvas = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_Rect program={programX,programY,programW,programH};
	SDL_FillRect(canvas, &program, SDL_MapRGBA(canvas->format, 0xAF, 0xAF, 0xAF, 0x6F));

	SDL_Rect control={0,0,SCREEN_WIDTH-programW,SCREEN_HEIGHT};
	SDL_FillRect(canvas, &control, SDL_MapRGBA(canvas->format, 0xCF, 0xCF, 0xCF, 0x6F));

	//converting to texture
	bgTexture.loadFromSurface(canvas);
	bgTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	
	SDL_FreeSurface(canvas);
}

void Level::rebuildGTexture() {
	SDL_Surface *canvas = SDL_CreateRGBSurface(0, GRD_SPRITE_SIZE*gxSize, GRD_SPRITE_SIZE*gySize, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	for(int x=0;x<gxSize;++x) {
		for(int y=0;y<gySize;++y) {
			int sprite=5*(grd[x][y] & 0x000000FF);
			sprite+=(grd[x][y] & 0x0000FF00)>>8 ;

			SDL_Rect offset={x*GRD_SPRITE_SIZE,y*GRD_SPRITE_SIZE,GRD_SPRITE_SIZE,GRD_SPRITE_SIZE};
			SDL_BlitSurface( grdTiles, &gridClips[sprite], canvas, &offset);
		}
	}

	gTexture.loadFromSurface(canvas);
	gTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(canvas);
}

void Level::rebuildIbTexture() {
	SDL_Surface *canvas = SDL_CreateRGBSurface(0, (INS_SPRITE_SIZE+2*PROGRAM_IB_PADDING)*insBarWidth, (INS_SPRITE_SIZE+2*PROGRAM_IB_PADDING)*insBarHeight, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_Rect offset={0,0,INS_SPRITE_SIZE,INS_SPRITE_SIZE};

	for(int c=0;c<insBarWidth;c++) {
		offset.x=c*(INS_SPRITE_SIZE+2*PROGRAM_IB_PADDING)+PROGRAM_IB_PADDING;
		for(int c1=0;c1<insBarHeight;c1++) {
			offset.y=c1*(INS_SPRITE_SIZE+2*PROGRAM_IB_PADDING)+PROGRAM_IB_PADDING;
			SDL_BlitSurface( insTiles, &insClips[insToSprite(insBar[c1][c])], canvas, &offset);
		}
	}

	ibTexture.loadFromSurface(canvas);
	ibTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	

	SDL_FillRect(canvas, NULL, SDL_MapRGBA(canvas->format,0xAF,0xAF,0xAF,0x6F));
	
	if(ibCc>=0 && ibCt>=0) {
		offset.x=ibCt*(INS_SPRITE_SIZE+2*PROGRAM_IB_PADDING)+PROGRAM_IB_PADDING;
		offset.y=ibCc*(INS_SPRITE_SIZE+2*PROGRAM_IB_PADDING)+PROGRAM_IB_PADDING;
		SDL_FillRect(canvas, &offset, SDL_MapRGBA(canvas->format,0xAF,0xAF,0xAF,0xFF));
	}
	ibBTexture.loadFromSurface(canvas);
	ibBTexture.setBlendMode(SDL_BLENDMODE_BLEND);

	SDL_FreeSurface(canvas);
}

void Level::rebuildCbTexture() {
	//int sf=SUPPORTED_RESOLUTIONS[CURR_RESOLUTION][2];

	SDL_Surface *canvas = SDL_CreateRGBSurface(0, CTRL_SPRITE_SIZE+2*PROGRAM_CB_PADDING, (CTRL_SPRITE_SIZE+2*PROGRAM_CB_PADDING)*6, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_Rect offset={PROGRAM_CB_PADDING,0,CTRL_SPRITE_SIZE,CTRL_SPRITE_SIZE};
	SDL_Color textColor= {0,0,0,0};

	for(int c1=0;c1<CTRL_SPRITES;c1++) {
		offset.y=c1*(CTRL_SPRITE_SIZE+2*PROGRAM_CB_PADDING)+PROGRAM_CB_PADDING;

		/*offset.x=CB_POSITIONS[c1].x*sf;
		offset.y=CB_POSITIONS[c1].y*sf;
		offset.w=CB_POSITIONS[c1].w*sf;
		offset.h=CB_POSITIONS[c1].h*sf;*/
		SDL_BlitSurface( ctrlTiles, &ctrlClips[c1], canvas, &offset);
	}

	char speedString[4];
	sprintf(speedString,"%d",speed);
	applyTextToSurface((CTRL_SPRITE_SIZE-FONT_SIZE_BIG/2)/2, CTRL_SPRITE_SIZE*4 - FONT_SIZE_BIG/2, canvas, speedString, fontBig, &textColor);

	
	cbTexture.loadFromSurface(canvas);
	cbTexture.setBlendMode(SDL_BLENDMODE_BLEND);

	SDL_FillRect(canvas, NULL, SDL_MapRGBA(canvas->format,0x00,0x00,0x00,0x00));
	if(cbC>=0) {
		offset.w=CTRL_SPRITE_SIZE;
		offset.h=CTRL_SPRITE_SIZE;
		offset.y=cbC*(CTRL_SPRITE_SIZE+2*PROGRAM_CB_PADDING)+PROGRAM_CB_PADDING;
		SDL_FillRect(canvas, &offset, SDL_MapRGBA(canvas->format,0xCF,0xCF,0xCF,0xFF));
	}
	cbBTexture.loadFromSurface(canvas);
	cbBTexture.setBlendMode(SDL_BLENDMODE_BLEND);

	SDL_FreeSurface(canvas);
}

void Level::close() {
	stop(0);
	save();
	tState=1 | ( ((levelId & 0x00FF)+1+packMenuIndex) <<4);
}

void Level::save() {
	char p[PROG_SIZE];
	int cu=0;
	for(int c=0;c<fCnt;++c) {		
		for(int c1=0;c1<functL[c];c1++) {
			sprintf(p+cu,"%04x",funct[c][c1]);
			cu+=4;
		}
	}

	packStructure.setPuzzleProg(levelId & 0xFF, levelId >> 8, p);
	packStructure.saveUserData();
}

void Level::win() {
	packStructure.setPuzzleDone(levelId & 0xFF, levelId >> 8, 1);
	menus[packMenuIndex + (levelId & 0xff) + 1]->makeTexture((levelId >> 8)+2);

//	stop(0);
	save();

	tState += (1<<12);
	if( packStructure.getPuzzleCount( (tState & 0xFF0) >> 4 ) <= tState>>12) {
		close();
		popupHandler->addPopup(packCompleteMessage);
	} else {
		popupHandler->addPopup(winMessage);
	}

}

int Level::getLevelId() {
	return levelId;
}

bool Level::isLoaded() {
	return loaded;
}

int Level::gridReset(bool resetFunctions=1) {
	//erase grid colors and count
	for(int c=0;c<COLOR_NUMBER;c++) {
		memset(insBar[c],0x0,sizeof(insBar[0]));
	}

	starsToGet=0;

	// ----[ Grid itself ]----

	cXoffset=0;
	cYoffset=0;
	cDoffset=0;
	cXoffsetLM=0;
	cYoffsetLM=0;
	cDoffsetLM=0;

	int color,cu=0;
	
	gxSize= con[cu++]-1;
	gySize= con[cu++]-1;
	cX= con[cu++]-1;
	cY= con[cu++]-1;
	cD= con[cu++]-1;

	for(int c=0;c<gySize;++c) {
		for(int c1=0;c1<gxSize;++c1) {
			grd[c1][c] = (con[cu++]-1) << 8;
			grd[c1][c] |= con[cu++]-1;

			if((grd[c1][c] & 0x000000FF) == TIL_STAR) {
				starsToGet++;
			}

			color=(grd[c1][c] & 0x0000FF00)>>8;
			if(!insBar[color][0]) {
				insBar[color][0]=1;
			}
		}
	}

	// ----[ Allowed instructions ]----

	int inp=0, allowedInsCnt=0;

	insBarHeight=0;
	insBarWidth=0;

	allowedInsCnt=con[cu++]-1;

	if(allowedInsCnt!=0){
		for(int c=0;c<COLOR_NUMBER;c++) {
			memset(insBar[c],0x0,sizeof(insBar[0]));
		}
	
		for(int c=0;c<allowedInsCnt;++c) {		
			inp =(con[cu++]-1 )<< 8;
			inp|=con[cu++]-1;
			insBar[(inp & 0xFF00) >> 8][inp & 0xFF]=inp;
		}

		//remove empty rows and columns
		bool emptyColumns[INS_TYPE_COUNT],emptyRow;
		memset(emptyColumns,0x1,sizeof(emptyColumns));
		for(int c=0;c<COLOR_NUMBER;c++) {
			emptyRow=1;
			for(int c1=0;c1<INS_TYPE_COUNT;c1++) {
				emptyRow=emptyRow && !insBar[insBarHeight][c1];
				emptyColumns[c1]=emptyColumns[c1] && !insBar[insBarHeight][c1];
			}

			//remove empty rows
			if(emptyRow) {
				for(int c1=0;c1<INS_TYPE_COUNT;c1++) {
					for(int c2=insBarHeight;c2<COLOR_NUMBER-1-c+insBarHeight;c2++) {
						insBar[c2][c1]=insBar[c2+1][c1];
					}
				}
				//not nesessary (to clear junk from the bottom of insBar)
				//memset(insBar[COLOR_NUMBER-1-c+insBarHeight],0x0,sizeof(insBar[0]));
			} else {
				insBarHeight++;
			}
		}

		//remove empty columns
		insBarWidth=1;
		for(int c=1;c<INS_TYPE_COUNT;c++) {
			if(emptyColumns[c]) {
				for(int c1=0;c1<insBarHeight;c1++) {
					for(int c2=c;c2<INS_TYPE_COUNT-1-c+insBarWidth;c2++) {
						insBar[c1][c2]=insBar[c1][c2+1];
					}
				}
			} else {
				insBarWidth++;
			}
		}

		//setup of blanks at the start
		for(int c=1,c1;c<insBarHeight;c++) {
			c1=0;
			while(insBar[c][c1]==0) c1++;
			insBar[c][0]=insBar[c][c1] & 0xFF00;
		}
	} else {
		for(int c=0;c<COLOR_NUMBER;c++) {
			if(insBar[c][0]) {
				for(int c2=0;c2<INS_TYPE_COUNT;c2++) {
					insBar[insBarHeight][c2]=spriteToIns(c2,c);
				}
				insBarHeight++;
			}
		}
		insBarWidth=INS_TYPE_COUNT;
	}

	// ----[ Functions sizes ]----
	if(resetFunctions) {
		int functionCnt=0;

		while(removeFunction()!=-1) ;

		functionCnt=con[cu++]-1;
		
		for(int c=0;c<functionCnt;++c) {		
			addFunction(con[cu++]-1);
		}

		int cu2=0,ret2;
		for(int c=0;c<fCnt && ret2!=EOF;++c) {		
			for(int c1=0;c1<functL[c] && ret2!=EOF;c1++) {
				ret2=sscanf(prog+cu2,"%4x",&funct[c][c1]);
				cu2+=4;
			}
		}
	}

	// add function calls to insBar
	for(int c=0;c<fCnt;c++) {
		for(int c1=0;c1<insBarHeight;c1++) {
			insBar[c1][insBarWidth]=spriteToIns(c+INS_TYPE_COUNT,insBar[c1][0] >> 8);
		}
		insBarWidth++;
	}

	recalculateNeeded=1;
	gTexture.free();

	return 0;
}

void Level::start() {
	if(started==0) {
		gridReset(0);

		while(getNextInstruction()!=-1) ;

		loadFunction(&nextIns, 0);

		if(insCount>0) {
			started=1
;			paused=0;

			cTicks=ticksAStep;
		}
		save();
	}
}

void Level::stop(int stopCode) {
	started=0;
	paused=0;
	cTicks=0;

	if(stopCode > 0) {
		gridReset(0);

		popupHandler->addPopup(stopMessages[stopCode]);
	}
}

void Level::togglePause() {
	if(!paused && started) {
		paused=1;
	} else {
		paused=0;
	}
}
	
void Level::tick() {
	if(started && !paused) {
		cTicks--;
		if(cTicks<=0) {
			cXoffsetLM=0;
			cYoffsetLM=0;
			cDoffsetLM=0;

			bool ok=1;

			while(ok){
				int ins=getNextInstruction();
				if(ins==-1) {
					stop(3);
					ok=0;
				}
				
				if(!(ins&0x0000FF00) | ((ins&0x0000FF00) == (grd[cX][cY]&0x0000FF00))) {
					ins=ins&0x000000FF;
					
					switch(ins) {
						case INS_FORWARD:
							moveRobo((cD%2==0)*(cD-1)*(-1), (cD%2)*(cD-2)*(-1));
							ok=0;
							break;
						case INS_RIGHT:
							rotateRobo(-1);
							ok=0;
							break;
						case INS_LEFT:
							rotateRobo(+1);
							ok=0;
							break;
						default:
							if(ins >= 20) {
								if(ins-20>=0 && ins-20<fCnt) {
									loadFunction(&nextIns, ins-20);
								}
							}		
					}
				}

				if(WAIT_ONLY_ON_MOVE==0)
					ok=0;
			}
			cTicks=ticksAStep;
		}
	}

	if(ticksAStep<cTicks)
		cTicks=ticksAStep;

	if(programH) {
		//alter queue transition state
		if(QUEUE) {
			if((!started && quTState>0) || (started && quTState<32)) {
				if(ENABLE_TRANSITIONS) {
					quTState-=((!started * 2)-1)*120/FRAMES_PER_SECOND;
					if(quTState<0) quTState=0;
					if(quTState>32) quTState=32;
				} else {
					quTState=(started==1)*32;
				}
				quTMod=quTState*quTState;

				recalculateNeeded=1;
			}
		}

		//alter instruction bar transition state
		if(ibTState<0) ibTState=0;
		if(ibTState>8) ibTState=8;
		if(ENABLE_TRANSITIONS) {
			if(ibState<ibTState) {
				ibState++;
				recalculateNeeded=1;
			}
			if(ibState>ibTState) {
				ibState--;
				recalculateNeeded=1;
			}
		} else {
			if(ibState!=ibTState) {
				ibState=ibTState;
				recalculateNeeded=1;
			}
		}
	}

	if(cXoffsetLM || cYoffsetLM || cDoffsetLM) {
		if( ticksAStep==1 || cTicks==1) {
			cXoffset = 0;
			cYoffset = 0;
			cDoffset = 0;
		} else {
			float t= ticksAStep - cTicks;
			cXoffset = (float)cXoffsetLM * pow( (float)t / ticksAStep - 1.0f , 2);
			cYoffset = (float)cYoffsetLM * pow( (float)t / ticksAStep - 1.0f , 2);
			cDoffset = (float)cDoffsetLM * pow( (float)t / ticksAStep - 1.0f , 2);
		}
	}
}

void Level::moveRobo(int xMove, int yMove) {
	if(cX+xMove>=gxSize || cX+xMove<0 || cY+yMove>=gySize || cY+yMove<0) {
		stop(1);
	}

	bool block=false;
	if( xMove != 0 && ( (grd[ cX+xMove][cY+yMove] & 0x00ff) == TIL_VERTICAL || (grd[ cX][cY] & 0x00ff) == TIL_VERTICAL ))
		block=true;
	if( yMove != 0 && ( (grd[ cX+xMove][cY+yMove] & 0x00ff) == TIL_HORIZONTAL || (grd[ cX][cY] & 0x00ff) == TIL_HORIZONTAL )) 
		block=true;
	if( (grd[ cX+xMove][cY+yMove] & 0x00ff) == TIL_BLOCK)
		block=true;

	if(!block) {
		cX+=xMove;
		cY+=yMove;
	}

	if(grd[cX][cY]==TIL_BLANK) {
		stop(2);
	}

	if((grd[cX][cY] & 0x00FF) == TIL_STAR) {
		grd[cX][cY] = (grd[cX][cY] & 0xFF00) | 1; 
		
		starsToGet--;
		if(starsToGet==0) {
			win();
		}
	}

	if(!block) {
		cXoffset=(-1) * gridS * xMove;
		cYoffset=(-1) * gridS * yMove;
		cXoffsetLM=cXoffset;
		cYoffsetLM=cYoffset;
		gTexture.free();
	}
}

void Level::rotateRobo(int dMove) {
	cD=(cD+dMove+4)%4;
	cDoffset=90*dMove*(-1);
	cDoffsetLM=cDoffset;
}

void Level::onClick(SDL_Event *e) {
	int x=e->button.x;
	int y=e->button.y;
	
	if(x<cbW) {
		// control bar

		if(x>cbX && x<cbX+cbW && y>cbY && y<cbY+cbH) {
			
			// alter coordinated relative to cb
			y = y - cbY;
			e->button.y=y;

			x = x - cbX;
			e->button.x=x;

			int ele= y/cbS;
			moveCbCursor(ele);
			selectCbCursor();
		}

	} else {
		if(y>programY && y<programY+programH) {
			// program 
	
			//alter y relative to program div
			y = y - programY;
			e->button.y=y;
	
			//program
			if(ibTState) {
				closeIb();
			}

			if(y<fuH) {
				//functions
				int row=(y- PROGRAM_FU_PADDING )/fuS;
				quH++;

				//get x relative to function's div
				x -= fuX;
				int column=x/fuS;
				if(row>=0 && row<fCnt && column>=0 && functL[row]>column) {
					moveFuCursorTo(row,column);
					selectFuInstruction();
				} 
			} else {
				//queue
				
				//x -= quX;
				//int column=x/(PROGRAM_QT_SIZE+PROGRAM_QT_PADDING);
				//printf("Someone clicked on queue instruction %d\n",column);
			}
		} else {
			if(ibTState) {
				if(x>=ibX && x<ibX+ibW && y>=ibY && y<ibY+ibH) {
					int type=	(x-ibX)/ibS;
					int color= 	(y-ibY)/ibS;
					moveIbCursorTo(type,color);
					selectIbInstruction();
				}
			} else {
				//grid
				//printf("Someone clicked on grid!\n");
			}	
		}
	}
}

void Level::onMouseMove(SDL_Event *e) {
	int x=e->button.x;
	int y=e->button.y;
	
	if(x<cbW) {
		// control bar
		if(x>cbX && x<cbX+cbW && y>cbY && y<cbY+cbH) {
			// alter coordinated relative to cb
			y = y - cbY;
			x = x - cbX;

			int ele= y/cbS;
			moveCbCursor(ele);
		}
	} else {
		if(y>programY && y<programY+programH) {
			// program 
	
			//alter y relative to program div
			y = y - programY;
	
			if(y<fuH) {
				//functions
				int row=(y- PROGRAM_FU_PADDING )/fuS;
				quH++;

				//get x relative to function's div
				x -= fuX;
				int column=x/fuS;
				if(row>=0 && row<fCnt && column>=0 && functL[row]>column) {
					if(!ibTState)
						moveFuCursorTo(row,column);
				} 
			} else {
				//queue
				
				//x -= quX;
				//int column=x/(PROGRAM_QT_SIZE+PROGRAM_QT_PADDING);
				//printf("Someone clicked on queue instruction %d\n",column);
			}
		} else {
			if(ibTState) {
				if(x>=ibX && x<ibX+ibW && y>=ibY && y<ibY+ibH) {
					int type=	(x-ibX)/ibS;
					int color= 	(y-ibY)/ibS;
					moveIbCursorTo(type,color);
				}
			} else {
				//grid
				//printf("Someone clicked on grid!\n");
			}	
		}
	}
}

void Level::onKeyPress(SDL_Event *e) {
	switch(e->key.keysym.sym) {
		case SDLK_a:
			start();
			break;
		case SDLK_s:
			togglePause();
			break;
		case SDLK_d:
			stop(0);
			break;
		case SDLK_q:
			close();
			break;
		case SDLK_w:
			PROGRAM_POS=!PROGRAM_POS;
			programPos=PROGRAM_POS;
			recalculateNeeded=1;
			fuTexture.free();
			quTexture.free();
			bgTexture.free();
			break;
		case SDLK_p:
			setSpeed(speed+1);
			break;
		case SDLK_o:
			setSpeed(speed-1);
			break;
		case SDLK_UP:
			if(cbC<0 && fCi>=0 && fCf>=0) {
				if(ibTState)
					moveIbCursorTo(ibCt,ibCc-1);
				else 
					moveFuCursorTo(fCf-1,fCi);
			} else {
				moveCbCursor(cbC-1);
			}
			break;
		case SDLK_DOWN:
			if(cbC<0 && fCi>=0 && fCf>=0) {
				if(ibTState)
					moveIbCursorTo(ibCt,ibCc+1);
				else 
					moveFuCursorTo(fCf+1,fCi);
			} else {
				moveCbCursor(cbC+1);
			}
			break;
		case SDLK_LEFT:
			if(cbC<0 && fCi>=0 && fCf>=0) {
				if(ibTState)
					moveIbCursorTo(ibCt-1,ibCc);
				else 
					moveFuCursorTo(fCf,fCi-1);
			} else {
				//nothing
			}
			break;
		case SDLK_RIGHT:
			if(cbC<0 && fCi>=0 && fCf>=0) {
				if(ibTState)
					moveIbCursorTo(ibCt+1,ibCc);
				else 
					moveFuCursorTo(fCf,fCi+1);
			} else {
				fCi=0;
				fCf=0;
				unfocusCb();
			}
			break;
		case SDLK_RETURN:
			if(cbC<0 && fCi>=0 && fCf>=0) {
				if(ibTState)
					selectIbInstruction();
				else 
					selectFuInstruction();
			} else {
				selectCbCursor();
			}
			break;
	}
}

void Level::setSpeed(int s) {
	if(s<1) 
		s=1;
	if(s*s>=FRAMES_PER_SECOND) {
		s=sqrt(FRAMES_PER_SECOND);
	}
	speed=s;
	SPEED=speed;
	saveSettings();

	ticksAStep=1+(int)FRAMES_PER_SECOND/s/s;

	cbTexture.free();
}

void Level::show(int alpha) {
	if(loaded==0)
		return;
	SDL_Rect scale = {0,0,0,0};

	if(recalculateNeeded) {
		calcPositions();
	}

	if(!bgTexture.isLoaded()) {
		rebuildBgTexture();
	}
	bgTexture.render(0,0);


	// ----[ control bar rendering ] ----
	if(!cbTexture.isLoaded()) {
		rebuildCbTexture();
	}
	scale.w=cbW;
	scale.h=cbH;
	cbBTexture.render(cbX,cbY,&scale);
	cbTexture.render(cbX,cbY,&scale);

	// ----[ grid rendering ] ----
	if(!gTexture.isLoaded()) {
		rebuildGTexture();
	}
	gTexture.setAlpha(alpha);
	scale.w=gridW;
	scale.h=gridH;
	gTexture.render(gridX,gridY,&scale);

	// ----[ robo rendering ] ----
	scale.w=gridS;
	scale.h=gridS;
	robo.setAlpha(alpha);
	int x=gridX + cX*gridS;
	int y=gridY + cY*gridS;
	robo.render(x + cXoffset,y + cYoffset ,&scale, 90*(cD+1) + cDoffset);

	if((programPos==0 || programPos==1) && programH>0) {
		// ----[ program rendering ] ----

		if(quTState) {
			if(!quTexture.isLoaded()) {
				rebuildQuTexture();
			}
			quTexture.setAlpha((alpha<quAlpha)?alpha:quAlpha);

			scale.w=quW;
			scale.h=quH;
			quTexture.render(quX,quY ,&scale);
		}

		if(fCnt>0) {
			if(!fuTexture.isLoaded()) {
				rebuildFuTexture();
			}
			fuTexture.setAlpha(alpha);

			scale.w=fuW;
			scale.h=fuH;
			fuTexture.render(fuX,fuY,&scale);
		}

		if(ibState && insBarHeight && insBarWidth) {
			if(!ibTexture.isLoaded()) {
				rebuildIbTexture();
			}
			ibTexture.setAlpha(ibAlpha);
			ibBTexture.setAlpha(ibAlpha);

			scale.w=ibW;
			scale.h=ibH;
			ibBTexture.render(ibX,ibY,&scale);
			ibTexture.render(ibX,ibY,&scale);

		}
	}
}

Menu::Menu() {
	cursor=0;
	cState=0;
	scroll=0;
	tScroll=0;
	height=SCREEN_HEIGHT;
}

void Menu::addElement(const char *n, int x, int y, int w, int h, int ev, int px, int py, int t=0, long a=0) {
	Element newEle;

	SDL_Rect newBox = {x*MENU_SCALE_FACTOR,y*MENU_SCALE_FACTOR,w*MENU_SCALE_FACTOR,h*MENU_SCALE_FACTOR};
	SDL_Rect newClip = {px*MENU_SCALE_FACTOR,py*MENU_SCALE_FACTOR,w*MENU_SCALE_FACTOR,h*MENU_SCALE_FACTOR};

	newEle.box = newBox;
	newEle.clip = newClip;
	newEle.clip2 = newClip;
	newEle.event = ev;
	newEle.type = t;
	newEle.value = a;

	// for clip2
	switch(ev) {
		case 4:
		case 9:
		case 10:
		case 11:
		case 12:
			newEle.clip2.y+=20*MENU_SCALE_FACTOR;
	}

	newEle.name[0]='\0';
	strncat(newEle.name,n,NAME_LENGTH);

	if(newEle.box.y+newEle.box.h > height)
		height = newEle.box.y+newEle.box.h+ MENU_CURSOR_PADDING;

	ele.push_back(newEle);
	makeTexture(ele.size()-1);
}

void Menu::makeTexture(int eleIndex) {
	SDL_Rect clip = ele[eleIndex].clip;

	switch(ele[eleIndex].event) {
		case 4:
			if( packStructure.getPuzzleDone( ele[eleIndex].value & 0xFF , ele[eleIndex].value>>8) )
				clip=ele[eleIndex].clip2;
			break;
		case 9:
			if(!SCREEN_FULLSCREEN) 
				clip=ele[eleIndex].clip2;
			break;
		case 10:
			if(!ENABLE_TRANSITIONS) 
				clip=ele[eleIndex].clip2;
			break;
		case 11:
			if(!WAIT_ONLY_ON_MOVE) 
				clip=ele[eleIndex].clip2;
			break;
		case 12:
			if(QUEUE) 
				clip=ele[eleIndex].clip2;
			break;
	}

	cTexture *texture;

	int a=ele[eleIndex].value;

	// graphical creation of textures
	SDL_Color textColor= {0,0,0,0};
	char *name,*dis;
	SDL_Surface *canvas = NULL;

	texture = new cTexture();

	switch(ele[eleIndex].type) {
		case 0:
			texture->loadFromSurface( menu, &clip);
			break;
		case 1: //
			name=packStructure.getPackageName(a & 0xFF);
			dis=packStructure.getPackageDis(a & 0xFF);
			
			canvas = SDL_CreateRGBSurface(0, clip.w,clip.h, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

			applySurface(menu, &clip, canvas, NULL);
			applyTextToSurface(FONT_SIZE_NORMAL, 0, canvas, name, fontNormal, &textColor);
			applyTextToSurface(FONT_SIZE_NORMAL, FONT_SIZE_NORMAL , canvas, dis, fontSmall, &textColor);

			texture->loadFromSurface(canvas);
			SDL_FreeSurface(canvas);

			break;
		case 2:
			name=packStructure.getPuzzleName(a & 0xff, a>>8);
			dis=packStructure.getPuzzleDis(a & 0xff, a>>8);

			canvas = SDL_CreateRGBSurface(0, clip.w,clip.h, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

			applySurface(menu, &clip, canvas, NULL);
			applyTextToSurface(FONT_SIZE_NORMAL, 0, canvas, name, fontNormal, &textColor);
			applyTextToSurface(FONT_SIZE_NORMAL, FONT_SIZE_NORMAL, canvas, dis, fontSmall, &textColor);

			texture->loadFromSurface(canvas);
			SDL_FreeSurface(canvas);

			break;
	}

	texture->setBlendMode(SDL_BLENDMODE_BLEND);	
	ele[eleIndex].texture = texture;
}


void Menu::onClick(SDL_Event *e) {
	int x=e->button.x;
	int y=e->button.y + scroll;
	
	for(unsigned int c=0;c<ele.size();c++) {
		if(	ele[c].box.x<= x  &&
			ele[c].box.y<= y  &&  
			ele[c].box.x+ele[c].box.w > x  &&  
			ele[c].box.y+ele[c].box.h > y 
			) {
			
			moveCursorTo(c);
			cursorSelect();
			break;			
		}
	}
}

void Menu::onMouseEnter(SDL_Event *e) {
	int x=e->button.x;
	int y=e->button.y + scroll;
	
	for(unsigned int c=0;c<ele.size();c++) {
		if(	ele[c].box.x<= x  &&
			ele[c].box.y<= y  &&  
			ele[c].box.x+ele[c].box.w > x  &&  
			ele[c].box.y+ele[c].box.h > y 
			) {
			
			moveCursorTo(c);
			break;
		}
	}
}

void Menu::onKeyPress(SDL_Event *e) {
	switch(e->key.keysym.sym) {
		case SDLK_q:
			closeMenu();
			break;
		case SDLK_RETURN:
			cursorSelect();
			break;
		case SDLK_UP:
			moveCursorTo(cursor-1);
			break;
		case SDLK_DOWN:
			moveCursorTo(cursor+1);
			break;

		case SDLK_n:
			changeResolution(CURR_RESOLUTION+1);
			break;
		case SDLK_m:
			changeResolution(CURR_RESOLUTION-1);
			break;
		case SDLK_f:
			changeFullscreen(!SCREEN_FULLSCREEN);
			break;
		case SDLK_r:
			packStructure.load();
			unloadMenus();
			loadMenus();
			break;

	}
}

void Menu::onScroll(SDL_Event *e) {
	tScroll-=e->wheel.y * 50;
	if(tScroll < 0)
		tScroll=0;
	if(tScroll > height - SCREEN_HEIGHT)
		tScroll=height - SCREEN_HEIGHT;
}

void Menu::openMenu(int menuIndex) {
	tState=1 | (menuIndex << 4);
}

void Menu::closeMenu() {
	if(state>>4 > packMenuIndex) {
		tState=1 | (packMenuIndex<<4);
	} else {
		switch( state>>4 ) {
			case 2:
				tState=1 | (1<<4);
				break;
			default :
				tState=1 | (0<<4);
		}
	}
}

void Menu::tick() {
	if(cState < 32) {
		if(ENABLE_TRANSITIONS) {
			cState+= 128/FRAMES_PER_SECOND ;
		} else {
			cState=32;
		}
		 
		cursorTexture.free();
		cAmpf=(cState*cState) >> 2 ;
	}

	if(tScroll!=scroll) {
		scroll+=(tScroll - scroll) /10 + (tScroll-scroll < 0?-1:+1);

		cursorTexture.free();
	}
}

void Menu::moveCursorTo(int index) {
	if(index<0) return;
	if(index>=(signed int) ele.size()) return;
	
	if(cursor!=index)
		cState=0;
	cursor=index;

	if(ele[cursor].box.y - MENU_CURSOR_PADDING - tScroll<0) {
		tScroll=ele[cursor].box.y - MENU_CURSOR_PADDING - 20;
	}
	if(ele[cursor].box.y + ele[cursor].box.h + MENU_CURSOR_PADDING - tScroll > SCREEN_HEIGHT) {
		tScroll=ele[cursor].box.y + ele[cursor].box.h + MENU_CURSOR_PADDING - SCREEN_HEIGHT;
	}
}

void Menu::cursorSelect() {
	switch(ele[cursor].event) {
		case 0:	
			break;
		case 1:
			//exit button
			quit=true;
			break;
		case 2:
			//play button
			openMenu(packMenuIndex);
			break;
		case 3:
			//packs
			tState = (tState & 0x0F) | ((packMenuIndex + 1 + (ele[cursor].value & 0xFF))<<4);
			break;
		case 4:
			//levels
			tState = 2 | (ele[cursor].value<<4);
			break;
		case 5:
			//close menu
			closeMenu();
			break;
		case 6:
			//open options
			openMenu(1);
			break;
		case 7:
			//open resolutions
			openMenu(2);
			break;
		case 8:
			//set resolution
			int res;
			sscanf(ele[cursor].name,"%d",&res);
			changeResolution(res);
			saveSettings();
			break;
		case 9:
			//toggle fullscreen
			changeFullscreen(!SCREEN_FULLSCREEN);
			saveSettings();
			break;
		case 10:
			//toggle transitions
			ENABLE_TRANSITIONS = !ENABLE_TRANSITIONS;
			makeTexture(cursor);
			saveSettings();
			break;
		case 11:
			//toggle wait at
			WAIT_ONLY_ON_MOVE = !WAIT_ONLY_ON_MOVE;
			makeTexture(cursor);
			saveSettings();
			break;
		case 12:
			//toggle wait at
			QUEUE = !QUEUE;
			makeTexture(cursor);
			saveSettings();
			break;
	}
}


void Menu::rebuildCursorTexture() {
	if( ele[cursor].event != 0 ) {
		int	lineWidth= MENU_SCALE_FACTOR * 0.5; 
		int fullLength=ele[cursor].box.w+ele[cursor].box.h+(lineWidth+MENU_CURSOR_PADDING)*4;
		int length = fullLength * cAmpf / 256;

		cursorBox.w = ele[cursor].box.w + (MENU_CURSOR_PADDING+lineWidth)*2;
		cursorBox.h = ele[cursor].box.h + (MENU_CURSOR_PADDING+lineWidth)*2;
		cursorBox.x = ele[cursor].box.x - lineWidth - MENU_CURSOR_PADDING;
		cursorBox.y = ele[cursor].box.y - lineWidth - MENU_CURSOR_PADDING;

		SDL_Surface *canvas = SDL_CreateRGBSurface(0, cursorBox.w, cursorBox.h, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

		/*
		1 : line from center of left edge to left corner
		2 : line from left corner to right corner
		3 : line from right corner to center of right edge
		*/
		SDL_Rect l1 = {0,0,lineWidth,0};
		SDL_Rect l2 = {0,0,0,lineWidth};
		SDL_Rect l3 = { cursorBox.w-lineWidth,0,lineWidth,0};

		if( length < (cursorBox.h-lineWidth)/2 ) {
			l1.h=length;
		} else{
			l1.h=(cursorBox.h-lineWidth)/2;
			length -= l1.h;

			if( length < cursorBox.w-lineWidth ) {
				l2.w = length;
			} else {
				l2.w = cursorBox.w-lineWidth;
				length -= l2.w;

				l3.h = length;
			}
		}

		l1.y = cursorBox.h/2 - l1.h;

		SDL_FillRect(canvas, &l1, SDL_MapRGBA(canvas->format, 0xF0, 0xF0, 0xF0, 0xFF));
		SDL_FillRect(canvas, &l2, SDL_MapRGBA(canvas->format, 0xF0, 0xF0, 0xF0, 0xFF));
		SDL_FillRect(canvas, &l3, SDL_MapRGBA(canvas->format, 0xF0, 0xF0, 0xF0, 0xFF));

		l1.y = cursorBox.h/2;
		l2.y = cursorBox.h - lineWidth;
		l3.y = cursorBox.h - l3.h;

		SDL_FillRect(canvas, &l1, SDL_MapRGBA(canvas->format, 0xF0, 0xF0, 0xF0, 0xFF));
		SDL_FillRect(canvas, &l2, SDL_MapRGBA(canvas->format, 0xF0, 0xF0, 0xF0, 0xFF));
		SDL_FillRect(canvas, &l3, SDL_MapRGBA(canvas->format, 0xF0, 0xF0, 0xF0, 0xFF));

		cursorTexture.loadFromSurface(canvas);
		cursorTexture.setBlendMode(SDL_BLENDMODE_BLEND);

		SDL_FreeSurface(canvas);
	}
}

void Menu::show(int alpha, SDL_Rect *box) {
	if(!cursorTexture.isLoaded()) {
		rebuildCursorTexture();
	}

	cursorTexture.setAlpha(alpha);
	cursorTexture.render( cursorBox.x, cursorBox.y - scroll, &cursorBox);

	for(unsigned int c=0;c< ele.size();c++) {
		SDL_Rect offset=ele[c].box;
		offset.y-=scroll;

		if(offset.y < 0) {
			offset.h += offset.y;
			offset.y = 0;
		}

		if(offset.y+offset.h > SCREEN_HEIGHT) {
			offset.h = SCREEN_HEIGHT - offset.y;
		}

		if( offset.h > 0 ) {
			ele[c].texture->setAlpha(alpha);
			ele[c].texture->render(offset.x,offset.y,&offset);
		}
	}
}

unsigned char Structure::Package::calcCrc(FILE* file) {

	int sum = 0;
	char inp =0;

	while(inp != EOF) {
		sum = (sum*7) & 0xff;
		inp = fgetc(file);
		sum = (sum+inp) & 0xff;
	}

	return sum;
}

void Structure::Package::load() {
	FILE *sFile=fopen(path,"r");

	if(sFile==NULL) {
		fprintf(stderr,"Pack at '%s' cannot be found\n", path);
		return;
	}

	int crc = calcCrc(sFile);

	if(crc != 0xff && 0) {
		fprintf(stderr,"Pack at '%s' is not a valid Progger pack\n", path);
		return;
	}

	fseek(sFile,0,SEEK_SET);

	int l,i;
	l=fgetc(sFile);
	i=l;
	for(int c=0;c<i && l!=EOF;c++) {
		l=fgetc(sFile);
		name[c]=l + LLIMIT;
	}
	name[i]='\0';

	l=fgetc(sFile);
	i=l;
	for(int c=0;c<i && l!=EOF;c++) {
		l=fgetc(sFile);
		dis[c]=l + LLIMIT;
	}
	dis[i]='\0';

	l=fgetc(sFile);

	while(l!=EOF) {

		char tnam[NAME_LENGTH],tdis[DIS_LENGTH],tcon[CON_SIZE];
		int cu=0;

		for(int c=0;c<l;c++) {
			tnam[c]=fgetc(sFile) + LLIMIT;
		}
		tnam[l]='\0';

		l=fgetc(sFile);

		if(l==EOF)
			break;

		for(int c=0;c<l;c++) {
			tdis[c]=fgetc(sFile) + LLIMIT;
		}
		tdis[l]='\0';

		// Level data 
		tcon[cu++]=1+fgetc(sFile); //width
		tcon[cu++]=1+fgetc(sFile); //height
		tcon[cu++]=1+fgetc(sFile); //cux
		tcon[cu++]=1+fgetc(sFile); //cuy
		tcon[cu++]=1+fgetc(sFile); //cud

		for(int c=(int)(tcon[0]-1)*(int)(tcon[1]-1)*2;c--;) {
			tcon[cu++]= 1+fgetc(sFile);
		}

		// ----[ Allowed instructions ]----
		tcon[cu++]=1 + fgetc(sFile);

		for(int c=(tcon[cu-1]-1)*2;c--;) {
			tcon[cu++]=1 + fgetc(sFile);
		} 
		
		// ----[ Functions sizes ]----
		tcon[cu++]=1 + fgetc(sFile);

		for(int c=tcon[cu-1]-1;c--;) {
			tcon[cu++]=1+fgetc(sFile);
		}

		tcon[cu]='\0';

		addPuzzle(tnam,tdis,tcon);
	
		// name lenght for next level(end if EOF)
		l=fgetc(sFile);
	}

	fclose(sFile);

	loaded=1;
}

bool Structure::Package::getDone() {
	return done;
}

char* Structure::Package::getPuzzleName(unsigned int puzzle) {
	if(loaded == 0)
		return NULL;
	if(puzzle<0 || puzzle>=puzzles.size()) puzzle=0;
	return puzzles[puzzle]->getName();
}

char* Structure::Package::getPuzzleDis(unsigned int puzzle) {
	if(loaded == 0)
		return NULL;
	if(puzzle<0 || puzzle>=puzzles.size()) puzzle=0;
	return puzzles[puzzle]->getDis();
}

char* Structure::Package::getPuzzleCon(unsigned int puzzle) {
	if(loaded == 0)
		return NULL;
	if(puzzle<0 || puzzle>=puzzles.size()) puzzle=0;
	return puzzles[puzzle]->getCon();
}

char* Structure::Package::getPuzzleProg(unsigned int puzzle) {
	if(loaded == 0)
		return NULL;
	if(puzzle<0 || puzzle>=puzzles.size()) puzzle=0;
	return puzzles[puzzle]->getProg();
}

bool Structure::Package::getPuzzleDone(unsigned int puzzle) {
	if(loaded == 0)
		return 0;
	if(puzzle<0 || puzzle>=puzzles.size()) puzzle=0;
	return puzzles[puzzle]->getDone();
}

void Structure::Package::setPuzzleProg(unsigned int puzzle, char *p) {
	if(loaded == 0)
		return;
	if(puzzle<0 || puzzle>=puzzles.size()) puzzle=0;
	puzzles[puzzle]->setProg(p);
}

void Structure::Package::setPuzzleDone(unsigned int puzzle, bool d) {
	if(loaded == 0)
		return;
	if(puzzle<0 || puzzle>=puzzles.size()) puzzle=0;
	puzzles[puzzle]->setDone(d);

	done=1;
	for(unsigned int i=0;done && i<puzzles.size();i++) {
		done=puzzles[i]->getDone();
//		printf("%d %d\n", i,done);
	}	
}

Structure::Structure(const char *pa) {
	path=pa;
	load();
}

void Structure::load() {
	clear();

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (path)) != NULL) {

		while ((ent = readdir (dir)) != NULL) {

			char *dot=rindex(ent->d_name, '.');

			if(dot!=NULL && strcmp(dot,".pack")==0) {
				char *filename = (char*) malloc(sizeof(char)*(strlen(path)+strlen(ent->d_name)+2));
				strcpy(filename,path);
				strcat(filename,ent->d_name);

				addPackage(filename);

			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("Pack directory");
	}

	for(unsigned int c=0;c<packages.size();c++) {
		if(! packages[c]->isLoaded() ) {
			packages[c]->load();
		}
	}

	loadUserData();
}

void Structure::clear() {
	while(packages.size()>0) {
		delete packages.at(packages.size()-1);
		packages.pop_back();
	}	
}

void Structure::loadUserData() {
	char fileName[32];
	fileName[0]='\0';
	strcat(fileName,CONFIG_PATH);
	strcat(fileName,"userData.lock");

	FILE *sFile=fopen(fileName,"r");

	if(sFile==NULL) {
		printf("No user data found at %s\n", fileName);
		return;
	}

	int ret;
	char tpack,tpuzz,tdone;
	char tprog[PROG_SIZE];
	
	ret=fscanf(sFile,"%c%c%c%s\n", &tpack, &tpuzz, &tdone, tprog);
	while(ret!=EOF) {
		setPuzzleDone(tpack, tpuzz, tdone==1);
		setPuzzleProg(tpack, tpuzz, tprog);
		ret=fscanf(sFile,"%c%c%c%s\n", &tpack, &tpuzz, &tdone, tprog);
	}
	
	fclose(sFile);
}

void Structure::saveUserData() {
	char fileName[32];
	fileName[0]='\0';
	strcat(fileName,CONFIG_PATH);
	strcat(fileName,"userData.lock");

	FILE *sFile=fopen(fileName,"w");

	if(sFile==NULL) {
		printf("Error writing user data to %s\n", fileName);
		return;
	}

	bool d;
	char *p;

	for(unsigned int c=0;c<packages.size();c++) {
		for(int c1=0;c1 < packages[c]->getPuzzleCount();c1++) {
			d=getPuzzleDone(c,c1);
			p=getPuzzleProg(c,c1);
			if(d || p[0]!='\0') {
				//fprintf(sFile,"%04x%01x0000\n", (c & 0xFF) | (c1<<8) , d);
				fprintf(sFile,"%c%c%c%s\n", (char)c, (char)c1, d ,p);
			}
		}
	}
	
	fclose(sFile);
}

void Structure::addPackage(char *filename) {
	Package *newPack=new Package(filename);
	packages.push_back(newPack);
}

int Structure::getPackageCount() {
	return packages.size();
}

int Structure::getPuzzleCount(unsigned int pack) {
	if(pack>=packages.size() || pack < 0) return 0;
	return packages[pack]->getPuzzleCount();
}

char* Structure::getPackageName(unsigned int pack) {
	if(pack>=packages.size() || pack < 0) pack=0;
	return packages[pack]->getName();
}

char* Structure::getPuzzleName(unsigned int pack, unsigned int puzzle) {
	if(pack>=packages.size() || pack < 0) pack=0;
	return packages[pack]->getPuzzleName(puzzle);
}

char* Structure::getPackageDis(unsigned int pack) {
	if(pack>=packages.size() || pack < 0) pack=0;
	return packages[pack]->getDis();
}

char* Structure::getPuzzleDis(unsigned int pack, unsigned int puzzle) {
	if(pack>=packages.size() || pack < 0) pack=0;
	return packages[pack]->getPuzzleDis(puzzle);
}

char* Structure::getPuzzleCon(unsigned int pack, unsigned int puzzle) {
	if(pack>=packages.size() || pack < 0) pack=0;
	return packages[pack]->getPuzzleCon(puzzle);
}

char* Structure::getPuzzleProg(unsigned int pack, unsigned int puzzle) {
	if(pack>=packages.size() || pack < 0) pack=0;
	return packages[pack]->getPuzzleProg(puzzle);
}

bool Structure::getPuzzleDone(unsigned int pack, unsigned int puzzle) {
	if(pack>=packages.size() || pack < 0) pack=0;
	return packages[pack]->getPuzzleDone(puzzle);
}

void Structure::setPuzzleProg(unsigned int pack, unsigned int puzzle, char *p) {
	if(pack<0 || pack>=packages.size()) pack=0;
	packages[pack]->setPuzzleProg(puzzle,p);
}

void Structure::setPuzzleDone(unsigned int pack, unsigned int puzzle, bool d) {
	if(pack<0 || pack>=packages.size()) pack=0;
	packages[pack]->setPuzzleDone(puzzle,d);
}


Popup::Popup(int x, int y, int stc) {
	init(x,y,stc);
}

Popup::Popup(int x, int y, SDL_Surface *src, int stc) {
	init(x,y,stc);
	loadTexture(src);
}

Popup::Popup(int x, int y, char *text, int maxW, TTF_Font *fnt, int fntSize, SDL_Color tClr, SDL_Color bClr, int stc) {
	textColor = tClr;
	backgroundColor = bClr;
	font = fnt;
	fontSize = fntSize;

	init(x,y,stc);


	loadLines(text,maxW);
	makeFromLines();
}

void Popup::init(int x, int y, int stc) {
	box.x=x;
	box.y=y;
	PADDING=5;
	LINE_SPACING=3;
	ticksToClose=FRAMES_PER_SECOND * stc / 1000;
	active=1;
	closeOnClick=0;
}

void Popup::loadLines(char *text, int maxW) {

	char word[32];
	
	int cursorX=0,cursorY=0,charsRead;
	int lineSpace = maxW - PADDING * 2;
	int ret=0;
	int wordWidth,wordHeight;

	ret=sscanf(text,"%32s%n",word, &charsRead);

	while(ret!=EOF) {
		text += charsRead;

		int len = strlen(word) +1;
		word[len-1]=' ';
		word[len]='\0';

		TTF_SizeText(font, word, &wordWidth, &wordHeight);

		if(lineSpace - wordWidth < 0) {
			cursorY++;
			cursorX=0;
			lineSpace = maxW - PADDING * 2;
		}
		lineSpace -= wordWidth;

		strcpy(&lines[cursorY][cursorX] , word);
		cursorX += len ;

		ret=sscanf(text,"%32s%n",word, &charsRead);
	}

	linesCount=	cursorY+1;

	if( linesCount > 1) {
		box.w = maxW;
	} else {
		box.w = maxW - lineSpace;
	}
	box.h = (wordHeight+LINE_SPACING)*linesCount - LINE_SPACING + 2 * PADDING;

}

void Popup::makeFromLines() {
	SDL_Surface *surface =  SDL_CreateRGBSurface(0, box.w, box.h, SCREEN_BPP, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a));

	for(int c=0;c<linesCount;c++) {
		applyTextToSurface( PADDING + 10, PADDING + c*fontSize, surface, lines[c], font, &textColor, 0xFF);
	}

	loadTexture(surface);

	SDL_FreeSurface(surface);
}

void Popup::loadTexture(SDL_Surface *src) {
	box.w=src->w;
	box.h=src->h;
	texture.loadFromSurface(src);
}

void Popup::close() {
	texture.free();
	active=0;
}

void Popup::onClick(SDL_Event *e) {
	int x=e->button.x;
	int y=e->button.y;

	if(x > box.x && x < box.x+box.w && y > box.y && y < box.y+box.h ) {
		// is clicked

		if( closeOnClick ) {
			close();
			e->button.x=0;
			e->button.y=0;
		}
	}
}

void Popup::show() {
	if(active && texture.isLoaded()) {
		texture.render(box.x,box.y,&box);
	}
}

void Popup::tick() {
	if(!active)
		return;

	if(texture.isLoaded() && backgroundColorIsFromTime) {
		int color=getColor();
		backgroundColor.r=(color>>16) & 0xff;
		backgroundColor.g=(color>>8) & 0xff;
		backgroundColor.b=color & 0xff;
		makeFromLines();
	}

	if(centerx) {
		box.x = (SCREEN_WIDTH-box.w) /2;
	}

	ticksToClose--;
	if(ticksToClose<64) {
		texture.setAlpha(ticksToClose*4);
	}
	if(ticksToClose<0) {
		close();
	}
}

bool Popup::isActive() {
	return active;
}

SDL_Rect Popup::getBox() {
	return box;
}

void Popup::moveTo(SDL_Rect *pos) {
	box.x=pos->x;
	box.y=pos->y;
}

void Popup::setCloseOnClick() {
	closeOnClick=true;
}

void Popup::setCloseOnClick(bool coc) {
	closeOnClick=coc;
}

PopupHandler::PopupHandler() {
	defTTC = DEF_POP_TTC;

	DEF_COC = DEF_POP_COC;

	DEF_X = DEF_POP_X;
	DEF_Y = DEF_POP_Y;

	DEF_PADDING = DEF_POP_PAD;
	DEF_LINE_SPACING = DEF_POP_LINE_SPACING;

	defFont = fontSmall;
	defFontSize = FONT_SIZE_SMALL;
	
	defTextColor.r=0x10;
	defTextColor.g=0x10;
	defTextColor.b=0x10;
	defTextColor.a=0xff;
	defBackgroundColor.r=0xdf;
	defBackgroundColor.g=0xdf;
	defBackgroundColor.b=0xdf;
	defBackgroundColor.a=0xff;
	
	defBackgroundColorIsFromTime = 1;
	defCenterx = 1;
}

void PopupHandler::addPopup(int x, int y) {
	Popup *newPopup=new Popup(x, y, defTTC);
	newPopup->backgroundColorIsFromTime=defBackgroundColorIsFromTime;
	newPopup->setCloseOnClick(DEF_COC);
	popups.push_back(newPopup);
}

void PopupHandler::addPopup(int x, int y, SDL_Surface *src) {
	Popup *newPopup=new Popup(x, y, src, defTTC);
	newPopup->setCloseOnClick(DEF_COC);
	popups.push_back(newPopup);
}

void PopupHandler::addPopup(char *text) {
	Popup *newPopup=new Popup(DEF_X, DEF_Y, text, SCREEN_WIDTH - DEF_PADDING *2, defFont, defFontSize, defTextColor, defBackgroundColor , defTTC);
	newPopup->backgroundColorIsFromTime=defBackgroundColorIsFromTime;
	newPopup->setCloseOnClick(DEF_COC);
	newPopup->centerx=defCenterx;
	popups.push_back(newPopup);
}

Popup* PopupHandler::getPopup(int i) {
	if(popups.size() <= (unsigned int)i || i<0)
		return NULL;
	return popups[i];
}

Popup* PopupHandler::getLastPopup() {
	if(popups.size() < 1)
		return NULL;
	return popups.back();
}

void PopupHandler::show() {
	for(unsigned int c=0; c<popups.size();c++) {
		popups[c]->show();
	}
}

void PopupHandler::tick() {
	for(unsigned int c=0; c<popups.size();c++) {
		popups[c]->tick();
	}
	while(popups.size()>0 && !popups.back()->isActive() ) {
		delete popups.back();
		popups.pop_back();
	}
}

void PopupHandler::onClick(SDL_Event *e) {
	for(int c = popups.size() - 1;c>=0;c--) {
		if(popups[c]->isActive()) {
			popups[c]->onClick(e);
		}
	}
}

void loadSettings() {
	char fileName[32]="\0";
	strcat(fileName,CONFIG_PATH);
	strcat(fileName,"settings.lock");

	FILE *sFile=fopen(fileName,"r");

	if(sFile==NULL) {
		printf("Can't open settings at %s\n", fileName);
		return;
	}

	//SCREEN_FULLSCREEN, CURR_RESOLUTION, PROGRAM_POS, ENABLE_TRANSITIONS, WAIT_ONLY_ON_MOVE, SPEED

	char crc;
	char inp_SCR,inp_CUR,inp_PRO,inp_ENA,inp_WAI,inp_SPE,inp_QUE;

	fscanf(sFile,"%c", &crc);
	
	fscanf(sFile,"%c", &inp_SCR);
	fscanf(sFile,"%c", &inp_CUR);
	fscanf(sFile,"%c", &inp_PRO);
	fscanf(sFile,"%c", &inp_ENA);
	fscanf(sFile,"%c", &inp_WAI);
	fscanf(sFile,"%c", &inp_SPE);
	fscanf(sFile,"%c", &inp_QUE);

	if(	(inp_SCR+inp_CUR+inp_PRO+inp_ENA+inp_WAI+inp_SPE+crc) % 3 == 0) {
		SCREEN_FULLSCREEN= inp_SCR;
		CURR_RESOLUTION= inp_CUR;
		PROGRAM_POS= inp_PRO;
		ENABLE_TRANSITIONS= inp_ENA;
		WAIT_ONLY_ON_MOVE= inp_WAI;
		SPEED= inp_SPE;
		QUEUE= inp_QUE;
	}
	
	fclose(sFile);
}

void saveSettings() {
	char fileName[32]="\0";
	strcat(fileName,CONFIG_PATH);
	strcat(fileName,"settings.lock");

	FILE *sFile=fopen(fileName,"w");

	if(sFile==NULL) {
		printf("Error writing user data to %s\n", fileName);
		return;
	}

	//SCREEN_FULLSCREEN, CURR_RESOLUTION, PROGRAM_POS, ENABLE_TRANSITIONS, WAIT_ONLY_ON_MOVE, SPEED

	char crc = (SCREEN_FULLSCREEN + CURR_RESOLUTION + PROGRAM_POS + ENABLE_TRANSITIONS + WAIT_ONLY_ON_MOVE + SPEED) % 3;

	fprintf(sFile,"%c",3-crc);

	fprintf(sFile,"%c",(char) SCREEN_FULLSCREEN);
	fprintf(sFile,"%c",(char) CURR_RESOLUTION);
	fprintf(sFile,"%c",(char) PROGRAM_POS);
	fprintf(sFile,"%c",(char) ENABLE_TRANSITIONS);
	fprintf(sFile,"%c",(char) WAIT_ONLY_ON_MOVE);
	fprintf(sFile,"%c",(char) SPEED);
	fprintf(sFile,"%c",(char) QUEUE);
	fprintf(sFile,"\n");
	
	fclose(sFile);
}

void setResolution(int index=-1) {
	CURR_RESOLUTION=0;

	if(index<0) {
		SDL_DisplayMode mode;
		if(SDL_GetCurrentDisplayMode(0, &mode) == 0 ) {
			CURR_RESOLUTION=RESOLUTION_COUNT-1;
			while( (SUPPORTED_RESOLUTIONS[CURR_RESOLUTION][0]>mode.w || SUPPORTED_RESOLUTIONS[CURR_RESOLUTION][1]>mode.h) && CURR_RESOLUTION>0) {
				--CURR_RESOLUTION;
			}
		}
	} else {
		if(index<RESOLUTION_COUNT)
			CURR_RESOLUTION=index;
		else
			CURR_RESOLUTION=RESOLUTION_COUNT-1;
	}

	SCREEN_WIDTH=SUPPORTED_RESOLUTIONS[CURR_RESOLUTION][0];
	SCREEN_HEIGHT=SUPPORTED_RESOLUTIONS[CURR_RESOLUTION][1];
	MENU_SCALE_FACTOR=SUPPORTED_RESOLUTIONS[CURR_RESOLUTION][2];

	int sf=SUPPORTED_RESOLUTIONS[CURR_RESOLUTION][2];
	
	FONT_SIZE_NORMAL=sf*FONT_SIZES[0];
	FONT_SIZE_SMALL=sf*FONT_SIZES[1];
	FONT_SIZE_TINY=4*sf;
	FONT_SIZE_BIG=sf*FONT_SIZES[3];

	INS_SPRITE_SIZE=sf*75/2;
	GRD_SPRITE_SIZE=sf*75/2;
	CTRL_SPRITE_SIZE=sf*50; 

	PROGRAM_QT_SIZE = DEF_PROGRAM_QT_SIZE * sf;
	PROGRAM_FU_SIZE = DEF_PROGRAM_FU_SIZE * sf;
	PROGRAM_CB_SIZE = DEF_PROGRAM_CB_SIZE * sf;

	PROGRAM_IB_MARGIN = DEF_PROGRAM_IB_MARGIN * sf;
	PROGRAM_CB_MARGIN = DEF_PROGRAM_CB_MARGIN * sf;
	GRID_MARGIN = DEF_GRID_MARGIN * sf;
}

int initWindow() {
	//Create window
	if(SCREEN_FULLSCREEN)
		gWindow = SDL_CreateWindow( "roboRun", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN ); 
	else
		gWindow = SDL_CreateWindow( "roboRun", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN ); 
	if( gWindow == NULL ) return 1;
	
	//Create renderer for window
	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
	if( gRenderer == NULL ) return 1;
	
	//Initialize renderer color
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	return 0;
} 

int init() {
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO )) return 1;
	
	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
		printf( "Warning: Linear texture filtering not enabled!" );
	}

	setResolution(CURR_RESOLUTION);

	if(initWindow()) return 1;

	//Initialize PNG and JPG loading
	int flags= IMG_INIT_PNG | IMG_INIT_JPG;
	if((IMG_Init(flags) & flags)!=flags) return 1;

	//Initialize SDL_ttf
	if(TTF_Init()) return 1;

	return 0;
}

int loadMenus() {
	
	//loading from menu file
	FILE *mFile=fopen(menuFile,"r");
	if(mFile==NULL) return 2;

	int ret,menuCnt,menuL;
	int tx,ty,tw,th,te,tpx,tpy;
	char namp[32];
	
	ret=fscanf(mFile,"%d", &menuCnt);
	for(int c=0;ret!=EOF && c<menuCnt;++c) {
		menus.push_back(new Menu());
		ret=fscanf(mFile,"%d",&menuL);
		
		for(int c1=0;ret!=EOF && c1<menuL;++c1) {
			ret=fscanf(mFile,"%32s %d %d %d %d %d %d %d",namp,&tx,&ty,&tw,&th,&te,&tpx,&tpy);
			menus.back()->addElement(namp,tx,ty,tw,th,te,tpx,tpy);
		}
	}
	fclose(mFile);
	if(ret==EOF) return 1;

	//dynamicly loading from packStructure
	packMenuIndex=menus.size();

	menus.push_back(new Menu());
	menus.back()->addElement("Packages",110,10,100,21 ,0,0,90);
	menus.back()->addElement("Back",50,10,25,21 ,5,100,90);
	for(int c=0; c<packStructure.getPackageCount(); c++) {
		menus.at(packMenuIndex)->addElement("Pack",30,50+c*30,260,20,3,0,132,1,c);

		menus.push_back(new Menu());
		menus.back()->addElement("Levels",110,10,100,21,0,0,111);
		menus.back()->addElement("Back",50,10,25,21 ,5,100,90);
		for(int c1=0;c1 < packStructure.getPuzzleCount(c);c1++) {
			menus.back()->addElement("Level",30,50+c1*30,260,20,4,0,132,2,c|(c1<<8));
		}
	}

	return 0;
}

void unloadMenus() {
	for(unsigned int c=0;c<menus.size();c++) {
		delete menus.at(c);
	}
	menus.resize(0);
}

int getColor() {
	//srand(time(NULL));
	//int randColor=rand();
	unsigned char h=time(NULL) & 0xFF;
	unsigned char s=140;		
	/*unsigned char s=(randColor & 0xFF00  )>>8;		
	if(s<128) 	s=128;								// no non-saturated colors 
	if(s>200) 	s=200;								// no over-saturated colors
	*/
	unsigned char v=255; 	

	unsigned char region, remainder, p, q, t;

    if(s==0) {
        return ((int)v<<16) | ((int)v<<8) | (int)v;
    }

    region = h / 43; // 255 / 6 = 43
    remainder = (h - (region * 43)) * 6; 

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:
            return (int)p | ((int)t<<8) | ((int)v<<16);
        case 1:
            return (int)p | ((int)v<<8) | ((int)q<<16);
        case 2:
            return (int)t | ((int)v<<8) | ((int)p<<16);
        case 3:
            return (int)v | ((int)q<<8) | ((int)p<<16);
        case 4:
            return (int)v | ((int)p<<8) | ((int)t<<16);
        case 5:
            return (int)q | ((int)p<<8) | ((int)v<<16);
    }

    return 0;
}

void setClip(SDL_Rect *clip, int x, int y, int w, int h) {
	clip->x=x;
	clip->y=y;
	clip->w=w;
	clip->h=h;
}

void setClips() {
	for(int c=0;c<GRID_SPRITES;++c) {
		setClip(gridClips+c,GRD_SPRITE_SIZE*(c%5),GRD_SPRITE_SIZE*(int)(c/5),GRD_SPRITE_SIZE,GRD_SPRITE_SIZE);
	}
	for(int c=0;c<INS_SPRITES;++c) {
		setClip(insClips+c,INS_SPRITE_SIZE*(c%15),INS_SPRITE_SIZE*(int)(c/15),INS_SPRITE_SIZE,INS_SPRITE_SIZE);
	}
	for(int c=0;c<CTRL_SPRITES;++c) {
		setClip(&ctrlClips[c],0,c*CTRL_SPRITE_SIZE,CTRL_SPRITE_SIZE,CTRL_SPRITE_SIZE); 
	}
}

SDL_Surface *loadImage(const char *path) {
	SDL_Surface* fromFile = IMG_Load( path );
	if( fromFile == NULL ) return NULL;

	//SDL_SetColorKey(fromFile,SDL_TRUE,SDL_MapRGB(fromFile->format,kred,kgreen,kblue));

	return fromFile;
}

void applySurface(SDL_Surface* src, SDL_Rect* clip, SDL_Surface* dst, SDL_Rect* offset) {
	SDL_Rect c={clip->x,clip->y,clip->w,clip->h};

	SDL_Rect o={0,0,0,0};
	if( offset!=NULL ) {
		o.x=offset->x;
		o.y=offset->y;
	}
	
	/*if(o.y<0) {
		c.y+=o.y;
		c.h-=o.y;
		o.y=0;
	}

	if(o.y+c.h > dst->h) {
		c.h=dst->h - o.y; 
	}*/

	SDL_BlitSurface( src, &c, dst, &o );
}

void applyTextToSurface(int x, int y, SDL_Surface* dst, const char *text, TTF_Font* font, SDL_Color *color, int alpha) {
	SDL_Rect offset={x,y,0,0};

	SDL_Surface *message = TTF_RenderText_Solid( font, text, *color );
	if( message == NULL ) return;


	SDL_Surface *optimized = SDL_ConvertSurface(message, dst->format,0);
	if(optimized!=NULL && alpha!=255) {
		SDL_SetSurfaceBlendMode(optimized, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(optimized, alpha);

		SDL_BlitSurface(optimized, NULL, dst, &offset);
		SDL_FreeSurface(optimized);
	} else
		SDL_BlitSurface(message, NULL, dst, &offset);

	SDL_FreeSurface(message);
}

void replace(char *target, char s1, char s2) {
	char *next=strchr( target, (int) s1 );
	while(next != NULL) {
		*next=s2;
		next=strchr( target, (int) s1 );
	}
}

void setPath(const char *file, char *path) {
	path[0]='\0';
	strcat(path,GRAPHICS_PATH[CURR_RESOLUTION]);
	strcat(path,file);
}

int loadMedia() {
	char path[64]; 

	setPath("background.jpg",path);
	if(backGround.loadFromFile(path,0xff,0x00,0x00)) return 1;

	setPath("robo.png",path);
	if(robo.loadFromFile(path,0xff,0x00,0x00)) return 1;

	setPath("menu.png",path);
	menu=loadImage(path);
	if( menu==NULL) return 1;
	
	setPath("grid.png",path);
	grdTiles=loadImage(path);
	if( grdTiles==NULL) return 1;

	setPath("ins.png",path);
	insTiles=loadImage(path);
	if( insTiles==NULL) return 1;
	
	setPath("control.png",path);
	ctrlTiles=loadImage(path);
	if( ctrlTiles==NULL) return 1;
	
	setClips();

	fontNormal = TTF_OpenFont( "urw-gothic.ttf" ,FONT_SIZE_NORMAL);
	if( fontNormal==NULL) return 1;

	fontSmall = TTF_OpenFont( "urw-gothic.ttf" ,FONT_SIZE_SMALL);
	if( fontSmall==NULL) return 1;

	fontTiny = TTF_OpenFont( "urw-gothic.ttf" ,FONT_SIZE_TINY);
	if( fontTiny==NULL) return 1;

	fontBig = TTF_OpenFont( "urw-gothic.ttf" ,FONT_SIZE_BIG);
	if( fontBig==NULL) return 1;

	return 0;
}

void unloadMedia() {
	//Free loaded images
	backGround.free();
	robo.free();
	SDL_FreeSurface(menu);
	SDL_FreeSurface(grdTiles);
	SDL_FreeSurface(insTiles);
	SDL_FreeSurface(ctrlTiles);

	//seg fault
	/*
	TTF_CloseFont(fontNormal);
	TTF_CloseFont(fontSmall);
	TTF_CloseFont(fontTiny);
	TTF_CloseFont(fontBig);
*/	
}

void closeWindow() {
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	SDL_DestroyRenderer( gRenderer );
	gRenderer = NULL;
}

void clean_up() {
	//Destroy window	
	closeWindow();

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void changeResolution(int resolution) {
	unloadMedia();
	closeWindow();
	unloadMenus();
	setResolution(resolution);
	initWindow();
	loadMedia();
	loadMenus();
}

void changeFullscreen(int fullscreen) {
	unloadMedia();
	closeWindow();
	unloadMenus();
	SCREEN_FULLSCREEN= fullscreen!=0;
	initWindow();
	loadMedia();
	loadMenus();
}

int main( int argc, char* args[] ) {

	//The frame rate regulator
	Timer fps;
	int lastFPS=0;

	loadSettings();

	//Initialize
	if(init()) return 1;
	if(loadMedia()) return 1;
	if(loadMenus()) return 1;

	int alpha;

	//setting up background
	int randColor=getColor();
	SDL_Rect backgroundBox = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	backGround.setColor((randColor>>16) & 0xff, (randColor>>8) & 0xff, randColor & 0xff);

	stateTP=16;
	state=0;
	tState= 1 | (0 << 4);

	//debug data
	char debugText[32];
	cTexture debugDisplay;
	SDL_Color debugColor={0,0,0,255};

	Level *currLevel=NULL;

	popupHandler = new PopupHandler();

	//While the user hasn't quit
	while( quit == false ) {
		//Start the frame timer
		fps.start();

		//While there's events to handle
		while( !quit && SDL_PollEvent( &event )) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch(state & 0xF) {
						case 1:
							menus[(state & 0xF0) >> 4]->onKeyPress(&event);
							break;
						case 2:
							currLevel->onKeyPress(&event);
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					popupHandler->onClick(&event);
					switch(state & 0xF) {
						case 1:
							menus[(state & 0xF0) >> 4]->onClick(&event);
							break;
						case 2:
							currLevel->onClick(&event);
								break;
					}
					break;
				case SDL_MOUSEWHEEL:
					switch(state & 0xF) {
						case 1:
							menus[(state & 0xF0) >> 4]->onScroll(&event);
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					switch(state & 0xF) {
						case 1:
							menus[(state & 0xF0) >> 4]->onMouseEnter(&event);
							break;
						case 2:
							currLevel->onMouseMove(&event);
							break;
					}
					break;
				case SDL_QUIT:
					//Quit the program
					quit = true;
					break;
			}
		}

		// state transitions
		if(tState!=state && stateTP>=16) {
			state=tState;
		}
		if(tState!=state || stateTP) {
			if(ENABLE_TRANSITIONS) {
				stateTP=stateTP+140/FRAMES_PER_SECOND;
				if(stateTP>=32) stateTP=0;
			} else {
				stateTP= (stateTP==16?0:16);
			}
			alpha=stateTP*stateTP-0x1F*stateTP+0xFF;
			//alpha=0xff/16*abs(stateTP-16);

		}

		// handling unloaded menus
		if((tState & 0xF)==1 && tState>>4 >= (signed int) menus.size()) {
			tState=state;
		}

		// unloading level if state is not in-game or levelId is not matching the one in state
 		if(currLevel!=NULL && (((state & 0xF)!=2) || (currLevel->getLevelId() != state>>4))) {
			delete currLevel;
			currLevel=NULL;
		}

		if((state & 0xF)==2) {
			// loading level
			if(currLevel==NULL) {
				currLevel= new Level(state>>4 , packStructure.getPuzzleCon((state & 0xFF0) >> 4,state >> 12) , packStructure.getPuzzleProg((state & 0xFF0) >> 4,state >> 12) ,PROGRAM_POS);
			} 

			// handling unloaded level
			if(currLevel->isLoaded() == 0) {
				currLevel->close();
			}
		}

		//drawing on the screen
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear( gRenderer );

		backgroundBox.w=SCREEN_WIDTH;
		backgroundBox.h=SCREEN_HEIGHT;
		randColor=getColor();
		backGround.setColor((randColor>>16) & 0xff, (randColor>>8) & 0xff, randColor & 0xff);
		backGround.render(0,0,&backgroundBox);

		switch(state & 0xF) {
			case 1:
				menus[state >> 4]->tick();
				menus[state >> 4]->show(alpha,&backgroundBox);
				break;
			case 2:
				currLevel->tick();
			
				currLevel->show(alpha);
				break;
		}

		//debug rendering
		sprintf(debugText,"FPS: %5d",lastFPS);
		debugDisplay.loadFromRenderedText(debugText,debugColor,fontTiny);
		debugDisplay.render(10,10);

		//popup handling
		popupHandler->tick();
		popupHandler->show();

		//Update the screen
		SDL_RenderPresent( gRenderer );
			
		//Cap the frame rate
		if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND ) {
			SDL_Delay(( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks());
		}

		//debug information
		lastFPS=1000 / (fps.get_ticks()+1);
	}

	//Clean up
	if(currLevel!=NULL) {
		delete currLevel;
		currLevel=NULL;
	}

	unloadMedia();
	clean_up();

	return 0;
}
