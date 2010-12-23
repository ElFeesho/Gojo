#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

/*
	SDL_Mixer Test

	SDL_Mixer provides sound magically, without blocking
	the main flow of a program. It works by setting channels
	on which a sound can be played. 

	It has a special music channel reserved, since there should
	only ever really be one piece of music playing at once.

	This test, tests music playback a long with sound effects.
*/

static Mix_Music *music = NULL;
static Mix_Chunk *sfx = NULL;

int handle_event(SDL_Event *event)
{
	switch(event->type)
	{
		case SDL_KEYDOWN:
			if(event->key.keysym.sym == SDLK_SPACE)
			{
				Mix_PlayChannel(-1,sfx,0);
			}			
			if(event->key.keysym.sym == SDLK_p)
			{
				Mix_PlayMusic(music, 0);
			}			
			if(event->key.keysym.sym == SDLK_f)
			{
				Mix_FadeInMusic(music, 0, 3000);
			}			
			if(event->key.keysym.sym == SDLK_o)
			{
				Mix_FadeOutMusic(3000);
			}			
			if(event->key.keysym.sym == SDLK_s)
			{
				Mix_HaltMusic();
			}			
			if(event->key.keysym.sym == SDLK_q)
			{
				return 1;
			}			
			break;
		case SDL_JOYBUTTONDOWN:
			if(event->jbutton.button == 0)
			{
				Mix_PlayChannel(-1,sfx,0);
			}			
			if(event->jbutton.button == 1)
			{
				Mix_PlayMusic(music, 0);
			}			
			if(event->jbutton.button == 2)
			{
				Mix_FadeInMusic(music, 0, 3000);
			}			
			if(event->jbutton.button == 3)
			{
				Mix_FadeOutMusic(3000);
			}			
			if(event->jbutton.button == 4)
			{
				Mix_HaltMusic();
			}			
			if(event->jbutton.button == 5)
			{
				return 1;
			}			
			break;
		case SDL_QUIT:
			return 1;
			break;
	}
	return 0;
}

int main(int argc, char **argv)
{
	SDL_Surface *screen = NULL;
	SDL_Joystick *joy = NULL;
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK)!=0)
	{
		fprintf(stderr, "Failed to initialise SDL!\n");
		return 1;
	}

	if(Mix_OpenAudio(22050, AUDIO_S16, 2, 4096)!=0)
	{
		fprintf(stderr, "Failed to initialise SDL_Mix\n");
	}

	screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
	if(!screen)
	{
		fprintf(stderr, "Failed to initialise the video mode\n");
		return 1;
	}

	/* 
		This test utilises some input methods, so we'll need the joystick
		so that it will work on the GP2X as well.
	*/

	if(SDL_NumJoysticks()>0)
	{
		joy = SDL_JoystickOpen(0);
		if(!joy)
		{
			fprintf(stderr, "Warning: Couldn't open a joystick\n");
		}
	}

	music = Mix_LoadMUS("test_files/music.xm");
	if(!music)
	{
		fprintf(stderr, "Failed to load 'test_files/music.xm'\n");
		return 1;
	} 

	sfx = Mix_LoadWAV("test_files/sound.ogg");
	if(!sfx)
	{
		fprintf(stderr, "Failed to load 'test_files/sound.ogg'\n");
		return 1;
	}

	/* Remove any pending joystick events */
	SDL_Event flush;
	while(SDL_PollEvent(&flush));

	int quit = 0;
	while(!quit)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			quit = handle_event(&event);
			if(quit)
				break;
		}
	}
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}
