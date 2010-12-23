/*
	Config File
	
	I'm not l33t enough to know autoconf and the rest, but
	here's a slapdash attempt at filtering out what's needed
	for this to work, any config changes you make will need
	to be reflected in the Makefile, for example. if you remove
	SDL_MIXER support, you'd have to remove -lSDL_mixer from
	the Makefile libs var.
*/

#define SDL_MIXER
#define SDL_TTF
#define SDL_GFX
#define SDL_NET
