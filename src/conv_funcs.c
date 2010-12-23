#include <stdlib.h>
#include <string.h>
#include <lua.h>

#include "errors.h"
#include "globals.h"

/*
	File name parsing 	
	Since it's not possible to know where the user will be running a script from, we can't assume that
	relative names will point to the right file!, so all filenames are parsed with this function.
*/
char *parse_filename(char *filename)
{
	char *parsed = NULL;
	if(filename!=NULL)
	{
		parsed = malloc(strlen(filename)+strlen(cmd)+1);
		memset(parsed, '\0', strlen(filename)+strlen(cmd)+1);
		strcpy(parsed, cmd);
		strcat(parsed, filename);
	}
	return parsed;
}

Uint32 *colour_parse(struct lua_State *l, char *colour)
{
	/* I think reading in a hexvalue would be nice for colour, rather than 
		a big bunch of rgb arguements everytime you want a colour!
	*/
	
	/* Create an integer array off 4   SDL COLOR CODE - R, G, B */	
	Uint32 *return_val = malloc(sizeof(Uint32)*4);
	memset(return_val,'\0',sizeof(Uint32)*4);
	
	if(strcasecmp(colour,"red")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,255,0,0);
		return_val[1] = 255;
		return_val[2] = 0;
		return_val[3] = 0;
		return return_val;
	}
	if(strcasecmp(colour,"green")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,0,255,0);
		return_val[1] = 0;
		return_val[2] = 255;
		return_val[3] = 0;
		return return_val;
	}
	if(strcasecmp(colour,"blue")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,0,0,255);
		return_val[1] = 0;
		return_val[2] = 0;
		return_val[3] = 255;
		return return_val;
	}
	if(strcasecmp(colour,"yellow")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,255,255,0);
		return_val[1] = 255;
		return_val[2] = 255;
		return_val[3] = 0;
		return return_val;
	}
	if(strcasecmp(colour,"cyan")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,0,255,255);
		return_val[1] = 0;
		return_val[2] = 255;
		return_val[3] = 255;
		return return_val;
	}
	if(strcasecmp(colour,"purple")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,255,0,255);
		return_val[1] = 255;
		return_val[2] = 0;
		return_val[3] = 255;
		return return_val;
	}
	if(strcasecmp(colour,"white")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,255,255,255);
		return_val[1] = 255;
		return_val[2] = 255;
		return_val[3] = 255;
		return return_val;
	}
	if(strcasecmp(colour,"black")==0)
	{
		return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,0,0,0);
		return_val[1] = 0;
		return_val[2] = 0;
		return_val[3] = 0;
		return return_val;
	}
	
	/* Finally if it doesn't match any of the given colours, check if it's a hex code */
	
	if(colour[0]!='#')
	{
		set_error(l,"ERROR: invalid colour!");
		return return_val;
	}
	
	/* Check we have a proper hex colour */
	int r = 0, g = 0, b = 0;
	
	switch(strlen(colour))
	{
		case 7: /* 3 byte colour (24bit) e.g. #00ff00 */
			r = (((toupper(colour[1])-'0' > 9)?(toupper(colour[1])-'A'+10):(toupper(colour[1]-'0')))<<4) + ((toupper(colour[2])-'0' > 9)?(toupper(colour[2])-'A'+10):(toupper(colour[2]-'0')));
			g = (((toupper(colour[3])-'0' > 9)?(toupper(colour[3])-'A'+10):(toupper(colour[3]-'0')))<<4) + ((toupper(colour[4])-'0' > 9)?(toupper(colour[4])-'A'+10):(toupper(colour[4]-'0')));
			b = (((toupper(colour[5])-'0' > 9)?(toupper(colour[5])-'A'+10):(toupper(colour[5]-'0')))<<4) + ((toupper(colour[6])-'0' > 9)?(toupper(colour[6])-'A'+10):(toupper(colour[6]-'0')));
			break;
		case 4: /* 2byte colour (16bit) e.g. #0f0 */
			r = (((toupper(colour[1])-'0' > 9)?(toupper(colour[1])-'A'+10):(toupper(colour[1]-'0')))<<4) + ((toupper(colour[1])-'0' > 9)?(toupper(colour[1])-'A'+10):(toupper(colour[1]-'0')));
			g = (((toupper(colour[2])-'0' > 9)?(toupper(colour[2])-'A'+10):(toupper(colour[2]-'0')))<<4) + ((toupper(colour[2])-'0' > 9)?(toupper(colour[2])-'A'+10):(toupper(colour[2]-'0')));
			b = (((toupper(colour[3])-'0' > 9)?(toupper(colour[3])-'A'+10):(toupper(colour[3]-'0')))<<4) + ((toupper(colour[3])-'0' > 9)?(toupper(colour[3])-'A'+10):(toupper(colour[3]-'0')));
			break;
		default:
			return NULL;
	}
	return_val[0] = SDL_MapRGB(SDL_GetVideoSurface()->format,r,g,b);
	return_val[1] = r;
	return_val[2] = g;
	return_val[3] = b;
	return return_val;
}
