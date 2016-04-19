#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#ifdef SDL_GFX
#include <SDL_rotozoom.h>
#include <SDL_gfxPrimitives.h>
#endif
#ifdef SDL_TTF
#include <SDL_ttf.h>
#endif
#ifdef SDL_MIXER
#include <SDL_mixer.h>
#endif
#ifdef SDL_NET
#include <SDL_net.h>
#endif
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "globals.h"

#include "binds.h"


/* Some needed variables for use with bind calls to make sure what is going down is legal */

int init_surfaces = 0;			/* Allows the user to specify whether surfaces should be handled by hardware or software */
int init_audio = 1;				/*	Allows the user to choose whether or not audio and networking should be initialised */
int init_net = 0;					/* Why initialise something you won't use? */
int screenw = 0, screenh = 0; /* Set by the script- used for SDL window size */
int bpp = 32;						/* Also set by the script for bits per pixel (optional Default = 16) */
int fullscreen = 0;				/* Also set by the script (optional, Default = 0) */
int vm_timer = 10;				/* Allow the script creator to set when the VM should be updated */
int joystick_count = 0;			/* Amount of joysticks connected to the system */
char *cmd = NULL;					/* Working directory */

SDL_Joystick *joy[4];			/* Joystick handle */

/*
	This fatal error variable stores whether or not it is good to process
	more events. If it isn't, it will be set to 1, and then the main loop
	will merely display an error message detailing the issue.
	
	Fatal error msg, stores a string for the error that occured.
*/

int fatal_error = 0; 
char *fatal_error_msg = NULL;
char *fatal_error_line = NULL;

static char *parse_cmd(char *line, struct lua_State *l)
{
	/*This is fine for everything except windows */
	char *working_dir = malloc(strlen(line)+7);
	memset(working_dir, '\0', strlen(line));
#ifndef __WINDOW__
	if(strrchr(line,'/')==NULL)
		strcpy(working_dir,"./");
	else
	{
		if(line[0]!='/')
		{
			strcpy(working_dir,"./");
			strcat(working_dir, line);
		}
		else
		{
			strcpy(working_dir,line);
		}
		strcpy(strrchr(working_dir,'/')+1,"\0");
	}
	char *lua_path = malloc(strlen(working_dir)+5);
	strcpy(lua_path,working_dir);
	strcat(lua_path,"?.gj");
	lua_getglobal(l,"package");
	lua_pushstring(l,lua_path);
	lua_setfield(l,-2,"path");
	lua_remove(l,-1);
	free(lua_path);
#else
	if(strrchr(line,'\\')==NULL)
	{
		strcpy(working_dir,".\\");
	}
	else
	{
		if(line[1]!=':') 
		{
			strcpy(working_dir,".\\");
			strcat(working_dir, line);
		}
		else
		{
			strcpy(working_dir,line);
		}
		strcpy(strrchr(working_dir,'\\')+1,"\0");
	}
	char *lua_path = strdup(working_dir);
	strcat(lua_path,"?.gj");
	lua_getglobal(l,"package");
	lua_pushstring(l,lua_path);
	lua_setfield(l,-2,"path");
	lua_remove(l,-1);
#endif
	return working_dir;
}

static void set_error(char *error_msg)
{
	fatal_error_msg = strdup(error_msg);
	fatal_error = 1;
}

static void clean_up()
{
	int i;
	for(i = 0;i<joystick_count && i<4;i++)
	{
		SDL_JoystickClose(joy[i]);
	}
	#ifdef SDL_NET
	SDLNet_Quit();
	#endif
	#ifdef SDL_MIXER
	Mix_CloseAudio();
	#endif
	#ifdef SDL_TTF
	TTF_Quit();
	#endif
}	

char *split_string(char *string, unsigned int position)
{
	if(position < strlen(string))
	{
		while(string[position] != ' ')
		{
			position--;
		}

		char *ret_string = string + position + 1;
		while(*ret_string==' ')
		{
			ret_string++;
		}
		string[position] = '\0';
		return ret_string;
	}
	else
	{
		return NULL;
	}
}

void draw_error(SDL_Surface *dst, char *error_orig)
{
	char *error = strdup(error_orig);
	int line_num = 0;
	while(error!=NULL)
	{
		char *temp = split_string(error,(screenw/8)-1);
		stringRGBA(dst, 5, 5+(8*line_num++), error, 0xff, 0, 0, 0xff);
		error = temp;
	}
	free(error);
}

int main(int argc, char **argv)
{
	if(argc==1)
	{
		fprintf(stderr, "Gojo needs a script file in order to do something useful.\n");
		return 1;
	}
	
	int err = 0;					/* Used for checking any errors from lua */
	int quit = 0;					/* Used for infinite loop */
	int last_tick = 0;				/* Last time the lua vm was updated */
	int flags = 0;
	
	char *game_name = NULL;			/* Stores the game's name, used in the window title */
	char *game_author = NULL;
	
	screen = NULL;	/* The SDL window */
	
	/* Create a lua state - stores all the interpreter jive (variables, functions, tables etc) */
	
	struct lua_State *lua_vm = luaL_newstate();
	
	/* Standard lua libs (maths, os, table and so on) */
	luaL_openlibs(lua_vm);

	/* 
		0. Set the working directory, gojo may of been called from a directory other
		than the directory containing the script files and resources!
		
		This will make games nice and portable, so you don't have to copy the gojo
		binary around!
	*/
	cmd = parse_cmd(argv[1],lua_vm);
	
	lua_newtable(lua_vm);
	lua_setglobal(lua_vm, "Game");
	lua_getglobal(lua_vm,"Game");
	lua_pushstring(lua_vm, cmd);
	lua_setfield(lua_vm,-2,"working_dir");
	
	binds_register(lua_vm);
	
	/* Finally open the script up! */
	if(luaL_loadfile(lua_vm, argv[1]))
	{
		fprintf(stderr, "%s couldn't be loaded\n",argv[1]);
		return 0;
	}
	
	/* 1. Parse the script once! (check for obvious errors I guess) */
	
	err = lua_pcall(lua_vm,0,0,0);
	if(err)
	{
		fprintf(stderr, "ERROR: Failed running %s\n",argv[1]);
		fprintf(stderr, "LUA: %s\n", lua_tostring(lua_vm,-1));
		return 1;
	}	
	
	/* 1. Continued - read in the 'Game' table data */
	
	lua_getglobal(lua_vm, "Game");
	lua_getfield(lua_vm, -1, "w");
	lua_getfield(lua_vm, -2, "h");
	lua_getfield(lua_vm,	-3, "name");
	lua_getfield(lua_vm, -4, "author");
	lua_getfield(lua_vm, -5, "bpp");
	lua_getfield(lua_vm, -6, "fullscreen");
	lua_getfield(lua_vm, -7, "update_tick");
	lua_getfield(lua_vm, -8, "audio");
	lua_getfield(lua_vm, -9, "net");
	lua_getfield(lua_vm, -10, "hw");
	lua_remove(lua_vm, -11);
	
	if(lua_isnil(lua_vm, -10) || lua_isnil(lua_vm, -9))
	{
		fprintf(stderr, "Couldn't continue running script, no screen dimensions were not set!\n");
		return 1;
	}
	screenw = lua_tointeger(lua_vm, -10);
	screenh = lua_tointeger(lua_vm, -9);
	
	if(lua_isnil(lua_vm,-8))
	{
		game_name = strdup("No-Name");
	}
	else
	{
		game_name = strdup(lua_tostring(lua_vm,-8));
	}
	
	if(lua_isnil(lua_vm,-7))
	{
		game_author = strdup("No-Name");
	}
	else
	{
		game_author = strdup(lua_tostring(lua_vm,-7));
	}
	if(!lua_isnil(lua_vm, -6))
		bpp = lua_tointeger(lua_vm, -6);
	
	if(bpp%8!=0)
	{
		fprintf(stderr, "WARNING: BPP was not divisible by 8, jumping back to 16!\n");
		bpp = 16;
	}
	
	if(!lua_isnil(lua_vm, -5))
	{
		fullscreen = lua_tointeger(lua_vm, -5);
	}

	if(!lua_isnil(lua_vm, -4))
	{
		vm_timer = lua_tointeger(lua_vm, -4);
	}

	if(!lua_isnil(lua_vm,-3))
	{
		init_audio = lua_tointeger(lua_vm, -3);
	}

	if(!lua_isnil(lua_vm,-2))
	{
		init_net = lua_tointeger(lua_vm, -2);
	}

	if(!lua_isnil(lua_vm,-1))
	{
		if(lua_tointeger(lua_vm, -1)>0)
		{
			init_surfaces = SDL_HWSURFACE;
		}
		else
		{
			init_surfaces = SDL_SWSURFACE;
		}
	}

	err = 0;
	err = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
	if(err < 0)
	{
		fprintf(stderr, "Failed to initialise SDL\n");
		return 1;
	}

	/* Check whether we can actually do hardware blitting */

	if(init_surfaces == SDL_HWSURFACE)
	{
		const SDL_VideoInfo *vid_inf = SDL_GetVideoInfo();
		if(!vid_inf->hw_available)
		{
			printf("Video hardware can't handle hardware surfaces, reverting to software surfaces\n");
			init_surfaces = SDL_SWSURFACE;
		}
	}
		
	atexit(SDL_Quit);
	
	/* Set the SDL window's width and height to what we got from the script */
	
	if(fullscreen == 1)
	{
		flags |= SDL_FULLSCREEN;
	}
	
	screen = SDL_SetVideoMode(screenw, screenh, bpp, flags|init_surfaces);
	
	if(!screen)
	{
		fprintf(stderr, "SDL: Failed to initialise the screen!\n");
		return 1;
	}
	
	/* Check for joystick */
	joystick_count = SDL_NumJoysticks();
	if(joystick_count > 0)
	{
		/* Open joystick */
		int i;
		for(i = 0;i<joystick_count && i<4;i++)
			joy[i] = SDL_JoystickOpen(i);
	}
	
	SDL_WM_SetCaption(game_name, NULL);
	
	int init_errors = 0;
	
	if(TTF_Init()<0)
	{
		fprintf(stderr,"Failed to initialise TTF: %s\n",TTF_GetError());
		set_error("ERROR: Failed to initialise TTF!");
		init_errors = 1;
	}
	
	if(init_net)
	{
		if(SDLNet_Init()!=0)
		{
			fprintf(stderr, "Unable to initialise SDL_Net\n");
			set_error("ERROR: Couldn't initialise Net lib!");
			init_errors = 1;
		}
	}
	
	if(init_audio)
	{
		if(Mix_OpenAudio(44100, AUDIO_S16, 2, 1024))
		{
			fprintf(stderr,"Unable to open audio\n");
			/* This isn't too fatal, so instead just put init_audio to 0 */
			init_audio = 0;
		}
	}
	

	/* Before the main loop, call on_init in the script */
	
	if(!init_errors)
	{
		lua_getglobal(lua_vm,"on_init");
		err = lua_pcall(lua_vm,0,0,0);
		if(err)
		{
			set_error((char*)lua_tostring(lua_vm, -1));
		}	
	}
	
	SDL_EventState(SDL_ACTIVEEVENT,SDL_IGNORE);
	SDL_EventState(SDL_KEYDOWN,SDL_IGNORE);
	SDL_EventState(SDL_KEYUP,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_JOYAXISMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYBALLMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_JOYHATMOTION,SDL_IGNORE);
	SDL_EventState(SDL_VIDEORESIZE,SDL_IGNORE);
	SDL_EventState(SDL_VIDEOEXPOSE,SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT,SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT,SDL_IGNORE);
	
	//SDL_FlushEvents(SDL_JOYAXISMOTION, SDL_JOYDEVICEREMOVED);

	do
	{
		SDL_Event event;
		/* Loop until there are no events left on the queue */
		while(SDL_PollEvent(&event)) 
		{  
			/* Process the appropriate event type */
			if(!fatal_error)
			{
				switch(event.type) 
				{ 
					case SDL_KEYDOWN:  /* Handle a KEYDOWN event */
						lua_getglobal(lua_vm,"on_key_down");
						lua_pushinteger(lua_vm,event.key.keysym.sym);
						lua_pcall(lua_vm,1,0,0);
						break;
					case SDL_KEYUP:
						lua_getglobal(lua_vm,"on_key_up");
						lua_pushinteger(lua_vm,event.key.keysym.sym);
						lua_pcall(lua_vm,1,0,0);
						break;
					case SDL_MOUSEBUTTONDOWN:
						lua_getglobal(lua_vm,"on_mouse_down");
						lua_pushinteger(lua_vm,event.button.button);
						lua_pushinteger(lua_vm,event.button.x);
						lua_pushinteger(lua_vm,event.button.y);
						lua_pcall(lua_vm,3,0,0);
						break;
					case SDL_MOUSEMOTION:
						lua_getglobal(lua_vm,"on_mouse_move");
						lua_pushinteger(lua_vm,event.button.state);
						lua_pushinteger(lua_vm,event.motion.x);
						lua_pushinteger(lua_vm,event.motion.y);
						lua_pcall(lua_vm,3,0,0);
						break;
					case SDL_MOUSEBUTTONUP:
						lua_getglobal(lua_vm,"on_mouse_up");
						lua_pushinteger(lua_vm,event.button.button);
						lua_pushinteger(lua_vm,event.button.x);
						lua_pushinteger(lua_vm,event.button.y);
						lua_pcall(lua_vm,3,0,0);
						break;
					case SDL_JOYBUTTONDOWN:
						lua_getglobal(lua_vm,"on_joy_button_down");
						lua_pushinteger(lua_vm,event.jbutton.button);
						lua_pcall(lua_vm,1,0,0);
						break;
					case SDL_JOYBUTTONUP:
						lua_getglobal(lua_vm,"on_joy_button_up");
						lua_pushinteger(lua_vm,event.jbutton.button);
						lua_pcall(lua_vm,1,0,0);
						break;
					case SDL_JOYAXISMOTION:
						lua_getglobal(lua_vm,"on_joy_axis");
						lua_pushinteger(lua_vm,event.jaxis.axis);
						lua_pushinteger(lua_vm,event.jaxis.value);
						lua_pcall(lua_vm,2,0,0);
						break;
					case SDL_QUIT:
						quit = 1;
						break;
				}
			}
			else
			{
				switch(event.type) 
				{ 
					case SDL_KEYUP:
						quit = 1;
						break;
					case SDL_JOYBUTTONUP:
						quit = 1;
						break;
					case SDL_QUIT:
						quit = 1;
						break;
				}
			}
		}

		if(!quit)
		{
			if(!fatal_error) /* Game speed should be able to set from lua script? */
			{		
				int ticks = SDL_GetTicks();
				lua_getglobal(lua_vm,"on_update");
				lua_pushinteger(lua_vm,ticks);
				err = lua_pcall(lua_vm,1,0,0);
				if(err)
				{
					set_error((char*)lua_tostring(lua_vm, -1));
					continue;
				}

				last_tick = SDL_GetTicks();
				SDL_Flip(screen);
				if(vm_timer-(last_tick-ticks)>0) 
				{
					SDL_Delay(vm_timer-(last_tick-ticks));
				}
			}
			else if(fatal_error)
			{
				SDL_EventState(SDL_KEYUP,SDL_ENABLE);
				SDL_EventState(SDL_JOYBUTTONUP,SDL_ENABLE);
				if(fatal_error_msg != NULL)
				{
					SDL_Rect pos;
					pos.x = 0;
					pos.y = 0;
					pos.w = screenw;
					pos.h = screenh;
					
					SDL_FillRect(screen, &pos, SDL_MapRGBA(screen->format, 0x00,0x00,0x00,0x00));
					
					draw_error(screen, fatal_error_msg);

					if(fatal_error_line != NULL)
					{
						draw_error(screen, fatal_error_line);
					}

					SDL_Flip(screen);
				}
			}
		}
	} while(!quit);
	
	if(err)
	{
		fprintf(stderr, "ERROR: Failed running %s\n",argv[1]);
		fprintf(stderr, "LUA ERROR: %s\n", lua_tostring(lua_vm,-1));
		return 1;
	}

	lua_getglobal(lua_vm,"on_quit");
	err = lua_pcall(lua_vm,0,0,0);

	lua_close(lua_vm);
	
	free(game_name);
	free(game_author);
	free(cmd);
	
	clean_up();
	
	SDL_Quit();
	
	return 0;
}
