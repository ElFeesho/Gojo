#include <stdio.h>
#include <SDL/SDL.h>

int main(int argc, char **argv)
{
	if(SDL_Init(SDL_INIT_VIDEO)!=0)
	{
		fprintf(stderr, "Failed to initialise SDL\n");
	}

	SDL_Rect **modes;

	/* Get available fullscreen/hardware modes */
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_SWSURFACE);

	/* Check if there are any modes available */
	if(modes == (SDL_Rect **)0)
	{
		printf("No modes available!\n");
		exit(-1);
	}

	/* Check if our resolution is restricted */
	if(modes == (SDL_Rect **)-1)
	{
		printf("All resolutions available.\n");
	}
	else
	{
		/* Print valid modes */
		printf("Available Modes\n");
		int i = 0;
		for(i=0;modes[i];++i)
			printf("  %d x %d\n", modes[i]->w, modes[i]->h);
	}
}
