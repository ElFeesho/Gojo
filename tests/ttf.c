#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

/*
	SDL_TTF Test

	This test, tries out the SDL_ttf library, which lets
	you use .ttf fonts in your SDL application!

	This test will just print out a simple sentense with
	a preset font.
*/

int main(int argc, char **argv)
{
	SDL_Surface *screen = NULL;
	TTF_Font *font = NULL;
	if(SDL_Init(SDL_INIT_VIDEO)!=0)
	{
		fprintf(stderr, "Failed to initialise SDL\n");
		return 1;
	}
	
	screen = SDL_SetVideoMode(320,240,16,SDL_SWSURFACE);
	if(!screen)
	{
		fprintf(stderr, "Failed to set video mode!\n");
		return 1;
	}

	/* Initialise TTF NOW! */ 

	if(TTF_Init()!=0)
	{
		fprintf(stderr, "Failed to initialise TTF %s\n", TTF_GetError());
		return 1;
	}
	
	font = TTF_OpenFont("test_files/arial.ttf",16);
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
		SDL_Color col = { 255,255,255 };
		SDL_FillRect(screen, NULL, 0x0);
		SDL_Surface *text_surface = NULL;
		text_surface = TTF_RenderText_Solid(font, "This is a TTF Test!",col);
		if(!text_surface)
		{
			fprintf(stderr, "Failed to create text surface!\n");
			return 1;
		}
		SDL_BlitSurface(text_surface, NULL, screen, NULL);
		SDL_FreeSurface(text_surface);
		SDL_Flip(screen);
	}

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
