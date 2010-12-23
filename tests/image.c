#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

int main(int argc, char **argv)
{
	SDL_Surface *screen = NULL;
	SDL_Surface *test_img = NULL;
	if(SDL_Init(SDL_INIT_VIDEO)!=0)
	{
		fprintf(stderr, "Couldn't initialise SDL!\n");
		return 1;
	}
	/*
		No init for SDL_image
	*/
	screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);

	if(!screen)
	{
		fprintf(stderr, "Couldn't set video mode\n");
		return 0;
	}

	test_img = IMG_Load("test_files/image.png");

	if(!test_img)
	{
		fprintf(stderr, "Failed to load 'test_files/image.png'\n");
		return 0;
	}

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
		SDL_FillRect(screen, NULL, 0x0);
		SDL_Rect pos = { screen->w/2-test_img->w/2, screen->h/2-test_img->h/2,test_img->w, test_img->h };
		SDL_BlitSurface(test_img, NULL, screen, &pos);
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
