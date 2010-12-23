#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>

/*
	SDL_gfx Test
	This test, utilises the SDL_gfx's primitive drawing
	functions.

	Lines,
	Rectangles,
	Circles,
	Text
	
	Are all tested!
*/

int main(int argc, char **argv)
{
	SDL_Surface *screen = NULL;

	if(SDL_Init(SDL_INIT_VIDEO)!=0)
	{
		fprintf(stderr, "Failed to initialise SDL\n");
		return 1;
	}
	/* No init call for SDL_gfx */

	screen = SDL_SetVideoMode(320,240,16,SDL_SWSURFACE);

	if(!screen)
	{
		fprintf(stderr, "Failed to set video mode\n");
		return 1;
	}
	
	/* Wait until a key is pressed */
	int quit = 0;

	while(!quit)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					quit = 1;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
			}
		}
		SDL_FillRect(screen,NULL,0x0);
		stringRGBA(screen, 0,0,"This test should show this text, a line",255,255,255,255);
		stringRGBA(screen, 0,10,"and a rectangle",255,255,255,255);
		stringRGBA(screen, 0,220,"If you see a blue circle, the test has",255,255,255,255);
		stringRGBA(screen, 0,230,"failed... Just kidding :D",255,255,255,255);
		aalineRGBA(screen, 10,30,130,100,255,0,0,255);
		rectangleRGBA(screen, 110,30,210,130,0,255,0,255);
		aacircleRGBA(screen, 180, 160,40, 0,0,255,255);
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
