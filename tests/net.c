#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

/*
	SDL_net Test
	
	This test simply resolves addresses.
*/

int main(int argc, char ** argv)
{
	if(SDL_Init(0)!=0)
	{
		fprintf(stderr,"Failed to initialise SDL\n");
		return 0;
	}
	atexit(SDL_Quit);

	if(SDLNet_Init()!=0)
	{
		fprintf(stderr, "Failed to initialise SDL_Net\n");
		return 0;
	}

	IPaddress address[2];
	if(SDLNet_ResolveHost(&address[0],"127.0.0.1", 1234)!=0)
	{
		fprintf(stderr, "Failed to resolve IP host address\n");
	}
	if(SDLNet_ResolveHost(&address[1],"localhost", 1234)!=0)
	{
		fprintf(stderr, "Failed to resolve 'localhost' dns name\n");
	}
	printf("Resolved IP 1: %s\nResolved IP 2: %s\n",SDLNet_ResolveIP(&address[0]), SDLNet_ResolveIP(&address[1]));
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}
