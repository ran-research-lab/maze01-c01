

//Using SDL, SDL_image, SDL_ttf, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include "LTexture.h"
#include <string>
#include <fstream>


// iip is an integer integer pair, we use it for 
// representing a points coordinates.
typedef std::pair<int,int> iip;


//Screen dimension constants
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 480;

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 20;

const int CLOCK_PERIOD = 150;
const auto SDL_COLOR_BLACK = SDL_Color {.r= 0, .g = 0x0, .b = 0, .a = 0xff };
const auto SDL_COLOR_RED = SDL_Color {.r= 0xff, .g = 0x0, .b = 0, .a = 0xff };
const auto SDL_COLOR_GREEN = SDL_Color {.r= 0x0, .g = 0xff, .b = 0, .a = 0xff };

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFontTiny = NULL;
TTF_Font *gFont = NULL;
TTF_Font *gFontBig = NULL;


// How many pixels correspond to 1 unit in the grid
double xf, yf;


// from http://lazyfoo.net/SDL_tutorials/lesson32/index.php
// A class that keeps helps keep track of the ticks
class Timer {
private:
    //The clock time when the timer started
    int startTicks;
    bool started;

public:
    Timer()  {
    	startTicks = 0;
    	started = false;
	}

    //Start the timer and register the current clock time
    void start(){
	    started = true;
	    startTicks = SDL_GetTicks();
	}

    //Gets the timer's time
    int get_ticks(){
	    if( started == true ){
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
	    }
	    return 0;
	}
};

class maze {
private:
	std::string st;
	std::pair<int,int> origin;
	std::pair<int,int> dest;
	std::pair<int,int> pos;
	int width, height;
	std::stack<iip> S;
public:
	maze(const std::string& fileName) {
		std::ifstream inFile(fileName);
		std::string tmp;

		if (!inFile.is_open()) {
			std::cout << "Could not open: " << fileName << "." << std::endl;
			exit(1);
		}

		
		inFile >> width >> height;
		std::cout << height << std::endl;
		inFile >> origin.first >> origin.second;
		pos = origin;
		inFile >> dest.first >> dest.second;
		st = "";

		for (int i = 0; i < height; i++) {
			inFile >> tmp;
			std::cout << tmp << std::endl;
			st = tmp + st;
		}
	}

	bool isAtDest() const { return ( dest.first == pos.first and dest.second == pos.second); }
	void move() {
		int stPos = pos.first + width * pos.second;
		//std::cout << stPos << std::endl;	
		//std::cout << st << std::endl;
		//std::cout << pos.first << " " << pos.second << std::endl;
		if (isAtDest()){
			// exit(1);
			// std::cout << "YESS" << std::endl;
			while (!S.empty()) {
				iip ppos = S.top();
				st[ppos.first + width * ppos.second] = 'p';
				S.pop();
			}
			//std::cout << toString() << std::endl;
		}
		else if (st[stPos+1] != '*' && st[stPos+1] != 'v') {
			//std::cout << "right" << std::endl;
			st[stPos+1] = 'v';
			S.push(pos);
			pos.first++;
		}
		else if (st[stPos-width] != '*' && st[stPos-width] != 'v') {
			//std::cout << "up" << std::endl;
			st[stPos-width] = 'v';
			S.push(pos);
			pos.second--;
		}
		else if (st[stPos-1] != '*' && st[stPos-1] != 'v') {
			//std::cout << "left" << std::endl;
			st[stPos-1] = 'v';
			S.push(pos);
			pos.first--;
		}
		else if (st[stPos+width] != '*' && st[stPos+width] != 'v') {
			//std::cout << "down" << std::endl;
			st[stPos+width] = 'v';
			S.push(pos);
			pos.second++;
		}
		else {
			//std::cout << "popping...." << std::endl;
			pos = S.top();
			S.pop();
		}
	}


	std::string toString() const {
		int fromPos = 0;
		std::string res;
		for (int i = 0; i < height; i++) {
			res = res + st.substr(fromPos, width) + "\n";
			fromPos += width;
		}
		return res;
	}
	const std::string &getSt() const {return st; }
	int getWidth() const {return width; }
	int getHeight() const {return height; }
	const iip &getPos() const {return pos; }


};


// function init:
//    Initializes several of the structures needed by SDL.
//    Also creates the main window.
//    Nothing interesting to see or modify here.

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Snake", SDL_WINDOWPOS_UNDEFINED,
		        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL ) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else{
			//Create synced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 ) {
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

// function loadMedia:
//    Loads several fonts that we might use other projects.

bool loadMedia() {
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont( "16_true_type_fonts/DejaVuSans.ttf", 20 );
	if( gFont == NULL ) {
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}

    gFontBig = TTF_OpenFont( "16_true_type_fonts/DejaVuSans.ttf", 40 );
    if( gFontBig == NULL ) {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    gFontTiny = TTF_OpenFont( "16_true_type_fonts/DejaVuSans.ttf", 10 );
    if( gFontTiny == NULL ) {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    return success;
}

// function close:
//    Before the program ends, we must free some reserved memories and quit
//    some SDL subsystems. This function takes care of that.

void close() {
	//Free global font
	TTF_CloseFont( gFont );
    gFontTiny = gFont = gFontBig = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

// function drawPoint():
//   Given:
//     * a reference to the renderer
//     * a reference to a 2D point
//     * a reference to a RGB color
//   Draws a rectangle a point in the grid (a rectangle of width xf and height yf)

void drawPoint(SDL_Renderer* renderer, const iip &p, const SDL_Color& color){
	int x =  p.first * xf;
	int y = SCREEN_HEIGHT -  (p.second + 1) * yf;
	SDL_Rect fillRect = { x ,  y , (int)xf, (int)yf };
	SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );		
	SDL_RenderFillRect( gRenderer, &fillRect );
}

void drawMaze(SDL_Renderer* renderer, const maze &m){
	const std::string mazeSt = m.getSt();
	for(int i = 0; i < mazeSt.length(); i++) {
		if (mazeSt[i] == '*') 
			drawPoint(renderer, iip(i % m.getWidth(), i / m.getWidth()), SDL_COLOR_BLACK);
		else if (mazeSt[i] == 'p') 
			drawPoint(renderer, iip(i % m.getWidth(), i / m.getWidth()), SDL_COLOR_GREEN);
	}
	const iip pos = m.getPos();
	drawPoint(renderer, pos, SDL_COLOR_RED);

}




void drawAxis(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
    // int i = 0;
    // int delta = tickSeparation *   width / static_cast<float>(maxX);
    // int label = 0;
    // int y2 = y + height;
    // int tickHeight = .05 * height;

    for (int i = 0; i <= GRID_WIDTH; i++) {
        SDL_RenderDrawLine(renderer, i * xf, 0, i* xf, SCREEN_HEIGHT );
    }
    for (int i = 0; i <= GRID_HEIGHT; i++) {
        SDL_RenderDrawLine(renderer, 0, i * yf, SCREEN_WIDTH, i * yf );
    }


}


int main( int argc, char* args[] ) {

    maze myMaze("maze02.txt");
    std::cout << myMaze.toString() <<  std::endl;

	
    // a time object to keep track of time
	Timer delta;

	// this variable keeps track of the ticks the last time we entered
	// the game loop
	int prevTicks = 0;


    // compute the number of pixels per grid square
    // we will need these for paiting the snake and apple
    xf = SCREEN_WIDTH  / GRID_WIDTH;
    yf = SCREEN_HEIGHT / GRID_HEIGHT; 


	//Start up SDL and create window
	if( !init() ) {
		printf( "Failed to initialize!\n" );
	}
	else {
		//Load media
		if( !loadMedia() ) {
			printf( "Failed to load media!\n" );
		}
		else {	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
			delta.start();
			//While application is running
			while( !quit ) {
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 ) {
					//User requests quit
					if( e.type == SDL_QUIT ) { quit = true;}

				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                // this block decides if it is time to move. We move
                // approximately CLOCK_PERIOD ticks
                if (prevTicks / CLOCK_PERIOD < delta.get_ticks()/CLOCK_PERIOD) {
                		myMaze.move();
                }
                prevTicks  = delta.get_ticks();

				drawAxis(gRenderer);
				drawMaze(gRenderer, myMaze);
				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
