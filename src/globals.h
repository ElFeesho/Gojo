#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <SDL.h>

int init_surfaces;				/* Allows the user to specify whether surfaces should be handled by hardware or software */
int init_audio;					/*	Allows the user to choose whether or not audio and networking should be initialised */
int init_net;						/* Why initialise something you won't use? */
int screenw, screenh;			/* Set by the script- used for SDL window size */
int bpp;								/* Also set by the script for bits per pixel (optional Default = 16) */
int fullscreen;					/* Also set by the script (optional, Default = 0) */
int vm_timer;						/* Allow the script creator to set when the VM should be updated */
int joystick_count;				/* Amount of joysticks connected to the system */
char *cmd;							/* Working directory */

SDL_Joystick *joy[4];			/* Joystick handle */

/*
	This fatal error variable stores whether or not it is good to process
	more events. If it isn't, it will be set to 1, and then the main loop
	will merely display an error message detailing the issue.
	
	Fatal error msg, stores a string for the error that occured.
*/

int fatal_error; 
char *fatal_error_msg;
char *fatal_error_line;

SDL_Surface *screen;

#endif
