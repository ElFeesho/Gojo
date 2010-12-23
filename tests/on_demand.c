#include <SDL/SDL.h>

int event_filter(const SDL_Event *event)
{
	switch(event->type)
	{
		case SDL_KEYDOWN:
			printf("Key down\n");
			break;
	}
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Surface *screen = SDL_SetVideoMode(320,240,16,SDL_SWSURFACE);

	SDL_EventState(SDL_MOUSEBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_KEYDOWN,SDL_IGNORE);
	
	atexit(SDL_Quit);

	int button = 0;
	SDL_SetEventFilter(event_filter);
	for(;;)
	{
		SDL_PumpEvents();
		if(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1))
		{
			printf("Mouse button is down boyo!\n");
		}
		else if(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(2))
		{
			break;
		}
	}
	return 0;
}
