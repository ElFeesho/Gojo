#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_net.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "conv_funcs.h"
#include "binds.h"
#include "globals.h"
#include "errors.h"

/* All the binded functions are listed here with their lua name counter parts */

const luaL_Reg sdl_binds[] =
{
	{ "surface_load", BIND_surface_load },
	{ "surface_create", BIND_surface_create },
	{ "surface_fill", BIND_surface_fill },
	{ "surface_blit", BIND_surface_blit },
	{ "surface_blit_area", BIND_surface_blit_area },
	{ "surface_get_pixel", BIND_surface_get_pixel },
	{ "surface_destroy", BIND_surface_destroy },
	{ "draw_rect", BIND_draw_rect },
	{ "draw_line", BIND_draw_line },
	{ "text_load", BIND_text_load },
	{ "text_draw", BIND_text_draw },
	{ "text_create", BIND_text_create },
	{ "text_destroy", BIND_text_destroy },
	{ "music_load", BIND_music_load },
	{ "music_play", BIND_music_play },
	{ "music_pause", BIND_music_pause },
	{ "music_resume", BIND_music_resume },
	{ "music_stop", BIND_music_stop },
	{ "music_fade_in", BIND_music_fade_in },
	{ "music_fade_out", BIND_music_fade_out },
	{ "music_set_volume", BIND_music_set_volume },
	{ "music_destroy", BIND_music_destroy },

	{ "sound_load", BIND_sound_load },
	{ "sound_play", BIND_sound_play },
	{ "sound_stop", BIND_sound_stop },
	{ "sound_stop_all", BIND_sound_stop_all },
	{ "sound_resume", BIND_sound_resume },
	{ "sound_resume_all", BIND_sound_resume_all },
	{ "sound_pause", BIND_sound_pause },
	{ "sound_pause_all", BIND_sound_pause_all },
	{ "sound_destroy", BIND_sound_destroy },

	{ "socket_tcp_connect", BIND_socket_tcp_connect },
	{ "socket_tcp_host", BIND_socket_tcp_host },
	{ "socket_tcp_accept", BIND_socket_tcp_accept },
	{ "socket_tcp_send", BIND_socket_tcp_send },
	{ "socket_tcp_sendstring", BIND_socket_tcp_sendstring },
	{ "socket_tcp_recv", BIND_socket_tcp_recv },
	{ "socket_tcp_readstring", BIND_socket_tcp_readstring },
	{ "socket_tcp_readline", BIND_socket_tcp_readline },
	{ "socket_tcp_close", BIND_socket_tcp_close },
	{ "socket_ready", BIND_socket_ready },

	{ "socket_set_create", BIND_socket_set_create },
	{ "socket_set_tcp_add", BIND_socket_set_tcp_add },
	{ "socket_set_tcp_remove", BIND_socket_set_tcp_remove },
	{ "socket_set_destroy", BIND_socket_set_destroy },
	{ "socket_set_check", BIND_socket_set_check },
	
	{ "mouse_show", BIND_mouse_show },
	{ "mouse_hide", BIND_mouse_hide },

	{ "send_fatal_error", BIND_send_fatal_error },

	{ "get_ticks" , BIND_get_ticks },
	{ "set_ticks" , BIND_set_ticks },
	{ "get_key", BIND_get_key },
	{ "get_mouse", BIND_get_mouse },
	{ "get_joy_count", BIND_get_joy_count },
	{ "get_joy_button", BIND_get_joy_button },
	{ "get_joy_axis", BIND_get_joy_axis },
	{ "get_joy_axis_count", BIND_get_joy_axis_count },
	{ "get_joy_button_count", BIND_get_joy_button_count },
	{ "reset_video", BIND_reset_video },
	{ "quit", BIND_quit },
	{ 0, 0 }
};

/* METATABLE prototypes */

int META_surface_gc(struct lua_State *l)
{
	/*
		I'm going to do something nasty- and assume that
		the top value is going to be the userdata!
	*/
	if(lua_isuserdata(l,1))
	{
		void *handle = lua_touserdata(l, 1);
		SDL_FreeSurface((SDL_Surface*)(*(long*)handle));
	}
	else
	{
		printf("It's not user data!\n");
	}
	return 0;
}

#ifdef SDL_TTF
int META_text_gc(struct lua_State *l)
{
	/*
		I'm going to do something nasty- and assume that
		the top value is going to be the userdata!
	*/
	if(lua_isuserdata(l,1))
	{
		void *handle = lua_touserdata(l, 1);
		TTF_CloseFont((TTF_Font*)(*(long*)handle));
	}
	else
	{
		printf("It's not user data!\n");
	}
	return 0;
}
#endif

#ifdef SDL_MIXER
int META_music_gc(struct lua_State *l)
{
	/*
		I'm going to do something nasty- and assume that
		the top value is going to be the userdata!
	*/
	if(lua_isuserdata(l,1))
	{
		void *handle = lua_touserdata(l, 1);
		Mix_FreeMusic((Mix_Music*)(*(long*)handle));
	}
	else
	{
		printf("It's not user data!\n");
	}
	return 0;
}

int META_sound_gc(struct lua_State *l)
{
	/*
		I'm going to do something nasty- and assume that
		the top value is going to be the userdata!
	*/
	if(lua_isuserdata(l,1))
	{
		void *handle = lua_touserdata(l, 1);
		Mix_FreeChunk((Mix_Chunk*)(*(long*)handle));
	}
	else
	{
		printf("It's not user data!\n");
	}
	return 0;
}
#endif

/* 
	Initialise * Functions 
	
	Gojo uses some functions that are only relevant to certain
	types of objects. So, it's nice to put these in the objects
	as callable functions, rather than having to do something
	similar to
	
		Gojo.function_name(some_object_reference, 10, 20, 30,40,50)
		
	When it's possible to do
		
		some_object_reference:function_name(10,20,30,40,50)
		
	Since several methods can create one object, the code is modularised.
*/

void init_surface(struct lua_State *l, void *image)
{
	/* 
		This function assumes that the lua state is ready to take 
		new stack items for a surface table
	*/
	lua_pushcfunction(l, BIND_surface_blit);
	lua_setfield(l,-2,"blit");
	
	lua_pushcfunction(l, BIND_surface_blit_area);
	lua_setfield(l,-2,"blit_area");
	
	lua_pushcfunction(l, BIND_surface_get_pixel);
	lua_setfield(l,-2,"get_pixel");
	
	lua_pushcfunction(l, BIND_surface_destroy);
	lua_setfield(l,-2,"destroy");
	
	lua_pushcfunction(l, BIND_surface_fill);
	lua_setfield(l,-2,"fill");

	void *handle = lua_newuserdata(l, sizeof(void*));
	/*
		This handle is... not so nice, but it does what it needs to do 
		Lua doesn't do garbage collection on light userdata, so we need
		some full userdata to use as a beacon for when it is time to 
		free some surface data!
	*/
	long surf_ptr = (long)image;
	memcpy(handle,(void*)&surf_ptr,sizeof(void*)); /* Store the surface's address in the handle */
	lua_setfield(l,-2,"beacon");
	lua_getfield(l,-1,"beacon");
	lua_newtable(l);
	/* Create a blank table to store metatable functions */
	lua_pushcfunction(l, META_surface_gc);
	lua_setfield(l, -2, "__gc");
	lua_setmetatable(l,-2);

	lua_remove(l, -1); /* remove beacon from the stack */
}

int BIND_surface_load(struct lua_State *l)
{
	int arg_count = lua_gettop(l);
	if(arg_count!=1 && arg_count!=3) /* Checks how many arguements were passed into the function */
	{
		set_error(l,"ERROR: surface_load - Arg Count Mismatch - Expected filename [, x, y ]");
		return 0;
	}
	
	char *filename = parse_filename((char*)luaL_checkstring(l,1));
	SDL_Surface *image = IMG_Load(filename);
	if(!image)
	{
		set_error(l,"ERROR: surface_load - Failed to load image (does it exist?)");
		return 0;
	}
	free(filename);
	SDL_Surface *temp = SDL_DisplayFormatAlpha(image);
	SDL_FreeSurface(image);
	image = temp;
	
	int x = 0, y = 0;
	if(arg_count == 3)
	{
		x = lua_tonumber(l,2);
		y = lua_tonumber(l,3);
	}
	
	/* First, and a pretty complex, example of stack usage in bindings */
	
	/* First of all, I create a new table (think array or hash table). */
	lua_newtable(l);
	
	/* 
		I push the address of the image we just created with IMG_load onto the stack 
		Stack Contents now - Table, Image address
	*/
	lua_pushlightuserdata(l,image);
	
	/* 
		Now I name the Image address to be 'addr' in the scope of Table 
		Stack was: Table, Image Address, now it is just Table, as setfield
		pops the last object off the stack.
	*/
	lua_setfield(l,-2,"addr");

	/*
		Now I put a default X and Y value for the surface, similar to how I put
		the address into the table also.
	*/
	lua_pushinteger(l,image->format->BitsPerPixel);
	lua_setfield(l,-2,"bpp");
	
	lua_pushinteger(l,x);
	lua_setfield(l,-2,"x");
	
	lua_pushinteger(l,y);
	lua_setfield(l,-2,"y");
	
	/* Same with width and height */
	lua_pushinteger(l,image->w);
	lua_setfield(l,-2,"w");
	lua_pushinteger(l,image->h);
	lua_setfield(l,-2,"h");
	
	lua_pushinteger(l,255);
	lua_setfield(l,-2,"alpha");
	
	lua_pushnumber(l,0.0);
	/* And rotation and scale */
	lua_setfield(l,-2,"rot");
	lua_pushnumber(l,1.0);
	lua_setfield(l,-2,"scale");
	
	/* Surfaces get created a lot- so init_surface adds all the table methods */
	init_surface(l, image);

	/* 
		Now the stack's contents is just the Table I made with lua_newtable, so we make
		this binding return 1, to notify lua, that there is one item to pull off the stack
		to return from the function.
	*/
	
	return 1;
}

int BIND_surface_create(struct lua_State *l)
{
	int arg_count = lua_gettop(l);
	if(arg_count!=4 && arg_count!=5) /* Checks how many arguements were passed into the function */
	{
		set_error(l,"ERROR: surface_create - Arg Count Mismatch - Expected x, y, w, h[, transparent colour]");
		return 0;
	}
	
	int x, y, w, h;
	x = luaL_checkint(l, 1);
	y = luaL_checkint(l, 2);
	w = luaL_checkint(l, 3);
	h = luaL_checkint(l, 4);
	char *colour = NULL;
	Uint32 *cols = NULL;
	if(arg_count == 5)
	{
		colour = (char*)lua_tostring(l, 5);
		cols = colour_parse(l,colour);
	}
	SDL_Surface *surface = NULL;
	surface = SDL_CreateRGBSurface(init_surfaces|SDL_SRCCOLORKEY,w,h,24,0,0,0,0);
	if(!surface)
	{
		set_error(l,"ERROR: surface_create - Failed creating surface");
		return 0;
	}
	if(colour)
	{
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format,cols[1],cols[2],cols[3]));
		SDL_SetColorKey(surface,SDL_SRCCOLORKEY,SDL_MapRGB(surface->format,cols[1],cols[2],cols[3]));
		free(cols);
	}
	else
	{
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format,255,0,255));
		SDL_SetColorKey(surface,SDL_SRCCOLORKEY,SDL_MapRGB(surface->format,255,0,255));
	}
	lua_newtable(l);
	lua_pushlightuserdata(l,surface);
	lua_setfield(l,-2,"addr");

	lua_pushinteger(l,surface->format->BitsPerPixel);
	lua_setfield(l,-2,"bpp");
	
	lua_pushinteger(l,x);
	lua_setfield(l,-2,"x");
	
	lua_pushinteger(l,y);
	lua_setfield(l,-2,"y");
	
	/* Same with width and height */
	lua_pushinteger(l,surface->w);
	lua_setfield(l,-2,"w");
	lua_pushinteger(l,surface->h);
	lua_setfield(l,-2,"h");
	
	lua_pushinteger(l,255);
	lua_setfield(l,-2,"alpha");
	
	lua_pushnumber(l,0.0);
	/* And rotation and scale */
	lua_setfield(l,-2,"rot");
	lua_pushnumber(l,1.0);
	lua_setfield(l,-2,"scale");
	
	/* Surface methods! */
	init_surface(l, surface);
	
	return 1;
}

int BIND_surface_fill(struct lua_State *l)
{
	int arg_count = lua_gettop(l);
	
	if(arg_count!=6) 
	{
		set_error(l,"ERROR: surface_fill - Arg Count Mismatch - Expecting surface table, x, y, w ,h, colour string");
		return 0;
	}
	
	/* Check if the arguement passed was infact a surface table */
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: surface_fill - surface table isn't a table! (argument 1)");
		return 0;
	}
	int x, y, w, h;
	x = luaL_checkint(l,2);	
	y = luaL_checkint(l,3);	
	w = luaL_checkint(l,4);	
	h = luaL_checkint(l,5);	
	lua_getfield(l,1,"addr");
	
	/*
		Now that all the values we want are in the stack, lets have at em!
	*/
	
	SDL_Surface *image = (SDL_Surface*)lua_touserdata(l,-1);
	if(image == NULL)
	{
		set_error(l,"ERROR: surface_fill - Provided surface is invalid!");
		return 0;
	}
	SDL_Rect pos = { x, y, w, h };
	char *colour = (char*)luaL_checkstring(l, 6);
	Uint32 *cols = colour_parse(l, colour);
	SDL_FillRect(image, &pos, SDL_MapRGB(image->format,cols[1],cols[2],cols[3]));
	free(cols);
	return 0;
}

int BIND_surface_blit(struct lua_State *l)
{
	/* Get's the SDL Screen Surface */
	SDL_Surface *screen = SDL_GetVideoSurface();
	if(!screen)
	{
		set_error(l,"ERROR: surface_blit - The screen hasn't been initialised yet.");
		return 0;
	}
	
	int arg_count = lua_gettop(l);
	
	/* Polymorphic function! 1 arg = blit with info from table,2 = blit onto surface, 3 = blit at position provided, 5 = with rot and scale, 6 = dst surface*/
	if(arg_count!=1 && arg_count!=2 && arg_count!=3 && arg_count!=5 && arg_count!=6) 
	{
		set_error(l,"ERROR: surface_blit - Arg Count Mismatch - Expecting surface [, dst surface] OR surface [[[, x, y], rot, scale], dst surface]");
		return 0;
	}
	
	/* Check if the arguement passed was infact a surface table */
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: surface_blit - Provided surface is invalid");
		return 0;
	}
	
	/* 
		Retrieve values from the keys in the table 
		Calls to getfield, merely puts the value in the stack,
		we need to retrieve them a little later	
	*/
	lua_getfield(l,1,"addr");
	lua_getfield(l,1,"x");
	lua_getfield(l,1,"y");
	lua_getfield(l,1,"alpha");
	lua_getfield(l,1,"rot");
	lua_getfield(l,1,"scale");
	
	/*
		Now that all the values we want are in the stack, lets have at em!
	*/
	
	SDL_Surface *image = (SDL_Surface*)lua_touserdata(l,-6);
	if(image == NULL)
	{
		fprintf(stderr, "No surface found whilst trying to blit a surface!\n");
		set_error(l,"No surface found whilst trying to blit a surface!");
		return 0;
	}
	SDL_Surface *rotozoomed = NULL;
	
	int x = 0;
	int y = 0;
	int alpha = 255;
	alpha = luaL_checkint(l,-3);
	if(arg_count==1 || arg_count == 2)
	{
		x = luaL_checkint(l,-5);
		y = luaL_checkint(l,-4);
	}
	else
	{ 
		x = luaL_checkint(l,2);
		y = luaL_checkint(l,3);
	}
	double rot = 0;
	double scale = 1.0;
	if(arg_count!=5 && arg_count!=6) 
	{
		rot = luaL_checknumber(l,-2);
		scale = luaL_checknumber(l,-1);
	}
	else
	{
		rot = luaL_checknumber(l,4);
		scale = luaL_checknumber(l,5);
	}
	
	if(arg_count == 2 || arg_count == 6)
	{
		if(lua_istable(l,arg_count))
		{
			lua_getfield(l,arg_count,"addr");
			screen = lua_touserdata(l,-1);
			if(!screen)
			{
				set_error(l, "ERROR: surface_blit - Destination surface is invalid");
				return 0;
			}
		}
		else
		{
			set_error(l,"ERROR: surface_blit - Destination surface isn't a table");
			return 0;
		}
	}
	
#ifdef SDL_GFX
	/* Don't really wanna waste cpu time rotozooming if nothing will happen! */
	if(rot != 0.0 || scale != 1.0)
	{
		rotozoomed = rotozoomSurface(image, rot, scale, 1);
		if(!rotozoomed)
		{
			set_error(l,"ERROR: surface_blit - Couldn't rotate and scale image");
		}
	}
	
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	if(rotozoomed)
	{
		pos.x -= (rotozoomed->w-image->w)/2;
		pos.y -= (rotozoomed->h-image->h)/2;
		pos.w = rotozoomed->w;
		pos.h = rotozoomed->h;
	}
	else
	{
		pos.w = image->w;
		pos.h = image->h;
	}
	
	/* If we rotozoomed lets blit that instead */
	if(rotozoomed)
	{
		SDL_SetAlpha(rotozoomed,SDL_SRCALPHA,alpha);
		SDL_BlitSurface(rotozoomed, NULL, screen, &pos);
		SDL_FreeSurface(rotozoomed);
	}
	else
	{
		SDL_SetAlpha(image,SDL_SRCALPHA,alpha);
		SDL_BlitSurface(image, NULL, screen, &pos);
	}
#else
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = image->w;
	pos.h = image->h;
	SDL_BlitSurface(image, NULL, screen, &pos);
#endif
	return 0;
}

int BIND_surface_blit_area(struct lua_State *l)
{
	/* Get's the SDL Window */
	SDL_Surface *screen = SDL_GetVideoSurface();
	if(!screen)
	{
		set_error(l,"ERROR: surface_blit_area - Screen hasn't been initialised yet.");
		return 0;
	}
	
	int arg_count = lua_gettop(l);
	
	/* Polymorphic function! 1 arg = blit with info from table, 3 = blit at position provided*/
	if(arg_count!=7 && arg_count!=8) 
	{
		set_error(l,"ERROR: surface_blit_area - Arg Count Mismatch - Expecting surface table, cookie-x, cookie-y, cookie-w, cookie-h,dst x, dst y [, dst surface]");
		return 0;
	}
	
	/* Check if the arguement passed was infact a surface table */
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: surface_blit_area - Surface passed was not a table");
		return 0;
	}
	/* 
		Retrieve values from the keys in the table 
		Calls to getfield, merely puts the value in the stack,
		we need to retrieve them a little later	
	*/
	lua_getfield(l,1,"addr");
	
	/*
		Now that all the values we want are in the stack, lets have at em!
	*/
	
	SDL_Surface *image = (SDL_Surface*)lua_touserdata(l,-1);
	if(image == NULL)
	{
		set_error(l,"ERROR: surface_blit_area - Surface is invalid");
		return 0;
	}
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	int dx = 0;
	int dy = 0;
	x = luaL_checkint(l,2);
	y = luaL_checkint(l,3);
	w = luaL_checkint(l,4);
	h = luaL_checkint(l,5);
	dx = luaL_checkint(l,6);
	dy = luaL_checkint(l,7);
	
	if(arg_count == 8)
	{
		if(lua_istable(l,8))
		{
			lua_getfield(l,8,"addr");
			screen = lua_touserdata(l,-1);
			if(!screen)
			{
				set_error(l,"ERROR: surface_blit_area - Destination surface is invalid");
				return 0;
			}
		}
		else
		{
			set_error(l,"ERROR: surface_blit_area - Destination surface passed is not a table");
			return 0;
		}
	}
	
	SDL_Rect pos, cookie;
	cookie.x = x;
	cookie.y = y;
	cookie.w = w;
	cookie.h = h;
	pos.x = dx;
	pos.y = dy;
	pos.w = w;
	pos.h = h;
	/* Screen may end up being a passed in surface! ! ! */
	SDL_BlitSurface(image, &cookie, screen, &pos);

	return 0;
}

int BIND_surface_get_pixel(struct lua_State *l)
{
	if(lua_gettop(l)!=3)
	{
		set_error(l,"ERROR: surface_get_pixel - Arg Count Mismatch - Expecting surface table, x, y");
		return 0;
	}
	
	/* Check if the arguement passed was infact a surface table */
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: surface_get_pixel - Surface is not a table");
		return 0;
	}
	
	lua_getfield(l,1,"addr");
	
	SDL_Surface *image = (SDL_Surface*)lua_touserdata(l,-1);
	int x, y;
	
	x = lua_tointeger(l,2);
	y = lua_tointeger(l,3);
	
	if(image == NULL)
	{
		set_error(l,"ERROR: surface_get_pixel - Surface is invalid");
		return 0;
	}
	
	if(x > image->w || x < 0 || y > image->h || y <0)
	{
		lua_pushinteger(l,-1);
		return 1;
	}
	
	unsigned char pixels[image->format->BytesPerPixel];
	
	memcpy(pixels,(image->pixels+(x*image->format->BytesPerPixel)+(y*image->w*image->format->BytesPerPixel)),image->format->BytesPerPixel);
	lua_pushinteger(l, pixels[0]);
	lua_pushinteger(l, pixels[1]);
	lua_pushinteger(l, pixels[2]);
	return 3;
}


int BIND_surface_destroy(struct lua_State *l)
{	
	if(lua_gettop(l)!=1)
	{
		set_error(l,"ERROR: surface_destroy - Arg Count Mismatch - Expecting a surface table");
		return 0;
	}
	
	/* Check if the arguement passed was infact a surface table */
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: surface_destroy - Surface is not a table");
		return 0;
	}
	
	lua_getfield(l,1,"addr");
	
	SDL_Surface *image = (SDL_Surface*)lua_touserdata(l,-1);
	if(image == NULL)
	{
		set_error(l,"ERROR: surface_destroy - Surface is invalid");
		return 0;
	}
	
	SDL_FreeSurface(image);
	lua_pushnumber(l,0);
	lua_setfield(l,1,"addr");
	
	lua_getfield(l,1,"beacon");
	lua_pushnil(l);
	lua_setmetatable(l, -2); /* Remove the metatable to prevent double free'ing */
	lua_remove(l, -1);
	lua_pushnil(l);
	lua_setfield(l, 1, "beacon"); /* Set the beacon to nil- should encourage lua's garbage collection */

	return 0;
}

int BIND_draw_rect(struct lua_State *l)
{
	/* Does what it says on the tin really */
	/* Get's the SDL Window */
	SDL_Surface *screen = SDL_GetVideoSurface();
	if(!screen)
	{
		set_error(l,"ERROR: draw_rect - Screen hasn't been initialised yet");
		return 0;
	}
	int arg_count = lua_gettop(l);
	if(arg_count !=5 && arg_count !=6)
	{
		set_error(l,"ERROR: draw_rect - Arg Count Mismatch - Expecting x, y, w, h, Colour String [, border colour string]");
		return 0;
	}
	
	SDL_Rect pos;
	pos.x = luaL_checkint(l,1);
	pos.y = luaL_checkint(l,2);
	pos.w = luaL_checkint(l,3);
	pos.h = luaL_checkint(l,4);
	Uint32 *cols[2];
	if(pos.x+pos.w-1<=0)
		return 0;
	else if(pos.x>screen->w)
		return 0;

	if(pos.y+pos.h-1<=0)
		return 0;
	else if(pos.y>screen->h)
		return 0;

	if(arg_count==5)
	{
		cols[0] = colour_parse(l,(char*)luaL_checkstring(l,5));
		SDL_FillRect(screen, &pos, cols[0][0]);
		free(cols[0]);
	}
	else
	{
		cols[0] = colour_parse(l,(char*)luaL_checkstring(l,5));
		cols[1] = colour_parse(l,(char*)luaL_checkstring(l,6));
		SDL_FillRect(screen, &pos, cols[1][0]);
		pos.x++;
		pos.y++;
		pos.w-=2;
		pos.h-=2;
		SDL_FillRect(screen, &pos, cols[0][0]);
		free(cols[0]);
		free(cols[1]);
	}
	return 0;
}

int BIND_draw_line(struct lua_State *l)
{
	/* Get's the SDL Window */
	SDL_Surface *screen = SDL_GetVideoSurface();
	if(!screen)
	{
		set_error(l,"ERROR: draw_line - Screen hasn't been initialised yet");
		return 0;
	}
	if(lua_gettop(l)!=5)
	{
		set_error(l,"ERROR: draw_line - Arg Count Mismatch - Expecting x, y, x2, y2, Colour String");
		return 0;
	}
	
	int x = luaL_checkint(l,1);
	int y = luaL_checkint(l,2);
	int x2 = luaL_checkint(l,3);
	int y2 = luaL_checkint(l,4);
	Uint32 *cols = colour_parse(l, (char*)luaL_checkstring(l,5));
	/* It ain't pretty I know :( */
	aalineRGBA(screen, x, y, x2, y2, cols[1], cols[2], cols[3] ,255);
	free(cols);
	return 0;
}

#ifdef SDL_TTF

int BIND_text_load(struct lua_State *l)
{
	if(lua_gettop(l)!=2)
	{
		set_error(l,"ERROR: text_load - Arg Count Mismatch - Expecting font filename, font pt size");
		return 0;
	}
	char *font_name = parse_filename((char*)luaL_checkstring(l,1));
	int size = luaL_checkint(l,2);
	
	TTF_Font *font = TTF_OpenFont(font_name, size);
	free(font_name);
	if(!font)
	{
		set_error(l,"ERROR: text_load - Failed loading font - (Might not come in that size!)");
		return 0;
	}
	lua_newtable(l);
	lua_pushlightuserdata(l,font);
	lua_setfield(l,-2,"addr");

	lua_pushcfunction(l,BIND_text_draw);
	lua_setfield(l, -2, "draw");

	lua_pushcfunction(l,BIND_text_create);
	lua_setfield(l, -2, "create");

	lua_pushcfunction(l,BIND_text_destroy);
	lua_setfield(l, -2, "destroy");

	void *handle = lua_newuserdata(l, sizeof(void*));
	/*
		This handle is... not so nice, but it does what it needs to do 
		Lua doesn't do garbage collection on light userdata, so we need
		some full userdata to use as a beacon for when it is time to 
		free some surface data!
	*/
	long font_ptr = (long)font;
	memcpy(handle,(void*)&font_ptr,sizeof(void*)); /* Store the surface's address in the handle */
	lua_setfield(l,-2,"beacon");
	lua_getfield(l,-1,"beacon");
	lua_newtable(l);
	/* Create a blank table to store metatable functions */
	lua_pushcfunction(l, META_text_gc);
	lua_setfield(l, -2, "__gc");
	lua_setmetatable(l,-2);
	/* 
		Push on meta function for surface clean up, name it __gc, 
		and finally wrap up by setting 'addr's metatable to the
		new table that was just made
	*/
	lua_remove(l,-1);
	return 1;
}

int BIND_text_draw(struct lua_State *l)
{
	/* Get's the SDL Window */
	SDL_Surface *screen = SDL_GetVideoSurface();
	if(!screen)
	{
		set_error(l,"ERROR: text_draw - Screen has not been initialised yet");
		return 0;
	}
	if(lua_gettop(l)!=5)
	{
		set_error(l,"ERROR: text_draw - Arg Count Mismatch - Expecting font table, x, y, string, colour string");
		return 0;
	}
	
	int x = luaL_checkint(l,2);
	int y = luaL_checkint(l,3);
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: text_draw - Font is not a table");
		return 0;
	}
	lua_getfield(l,1,"addr");
	TTF_Font *font = lua_touserdata(l,-1);
	Uint32 *cols = colour_parse(l, (char*)luaL_checkstring(l,4));
	int r = cols[1];
	int g = cols[2];
	int b = cols[3];
	free(cols);
	
	char *text = (char*)luaL_checkstring(l,5);
	if(!strlen(text))
		return 0;
	
	/* If there is no font - ERROR!!! */
	if(font==NULL)
	{
		set_error(l,"ERROR: text_draw - Font is invalid");
		return 0;
	}
	/* gcc extension for defining a struct's contents in a one liner.. neat */
	SDL_Color tcol = { r, g, b };
	
	SDL_Surface *text_surface;
	text_surface = TTF_RenderText_Solid(font,text,tcol);
	
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = text_surface->w;
	pos.h = text_surface->h;
	SDL_BlitSurface(text_surface,NULL,screen,&pos);
	SDL_FreeSurface(text_surface);
	return 0;
}

int BIND_text_create(struct lua_State *l)
{
	if(lua_gettop(l)!=5)
	{
		set_error(l,"ERROR: text_create - Arg Count Mismatch - Expecting font table, x, y, string, colour string");
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l, "ERROR: text_create - Font is not a table");
		return 0;
	}
	lua_getfield(l, 1, "addr");
	TTF_Font* font = lua_touserdata(l,-1);
	int x = luaL_checkint(l,2);
	int y = luaL_checkint(l,3);
	Uint32 *cols = colour_parse(l, (char*)luaL_checkstring(l,4));
	int r = cols[1];
	int g = cols[2];
	int b = cols[3];
	free(cols);
	char *text = (char*)luaL_checkstring(l,5);
	
	/* If no font was passed in - ERROR!!! */
	if(font == NULL)
	{
		set_error(l,"ERROR: text_create - Font is invalid");
		return 0;
	}
	/* gcc extension for defining a struct's contents in a one liner.. neat */
	SDL_Color tcol = { r, g, b };
	
	SDL_Surface *text_surface;
	text_surface = TTF_RenderText_Solid(font,text,tcol);
		
	lua_newtable(l);
	
	lua_pushlightuserdata(l, text_surface);
	lua_setfield(l,-2,"addr");
	
	lua_pushinteger(l,x);
	lua_setfield(l,-2,"x");
	
	lua_pushinteger(l,y);
	lua_setfield(l,-2,"y");
	
	lua_pushinteger(l,255);
	lua_setfield(l,-2,"alpha");
	
	lua_pushinteger(l,text_surface->w);
	lua_setfield(l,-2,"w");
	
	lua_pushinteger(l,text_surface->h);
	lua_setfield(l,-2,"h");
	
	lua_pushstring(l,text);
	lua_setfield(l,-2,"string");
	
	lua_pushnumber(l,0.0);
	lua_setfield(l,-2,"rot");
	
	lua_pushnumber(l,1.0);
	lua_setfield(l,-2,"scale");
		
	init_surface(l, text_surface);
	
	return 1;
}

int BIND_text_destroy(struct lua_State *l)
{
	/* Get's the SDL Window */
	if(lua_gettop(l)!=1)
	{
		set_error(l,"ERROR: text_destroy - Arg Count Mismatch - Expecting font table");
		return 0;
	}
	
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: text_destroy - Font is not a table");
		return 0;
	}
	lua_getfield(l, 1, "addr");
	TTF_Font *font = lua_touserdata(l,-1);
	if(!font)
	{
		set_error(l, "ERROR: text_destroy - Font is invalid");
		return 0;
	}
	TTF_CloseFont(font);
	
	lua_getfield(l,1,"beacon");
	lua_pushnil(l);
	lua_setmetatable(l, -2); /* Remove the metatable to prevent double free'ing */
	lua_remove(l, -1);
	lua_pushnil(l);
	lua_setfield(l, 1, "beacon"); /* Set the beacon to nil- should encourage lua's garbage collection */

	return 0;
}


#endif

/* Sound / Music Binds */

#ifdef SDL_MIXER

int BIND_music_load(struct lua_State *l)
{
	/* Load a piece of music */
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: music_load - Arg Count Mismatch - Expecting music file name");
		return 0;
	}
	if(!lua_isstring(l,1))
	{
		set_error(l,"ERROR: music_load - File name is not a string");
		return 0;
	}
		
	Mix_Music *temp_music = NULL;
	char *filename = parse_filename((char*)lua_tostring(l,1));
	if(init_audio)
	{
		temp_music = Mix_LoadMUS(filename);
	}
	free(filename);
	if(temp_music == NULL && init_audio)
	{
		set_error(l,"ERROR: music_load - Failed loading music");
		return 0;
	}
	lua_newtable(l);
	lua_pushlightuserdata(l, temp_music);
	lua_setfield(l,-2,"addr");
	lua_pushcfunction(l,BIND_music_play);
	lua_setfield(l,-2,"play");
	lua_pushcfunction(l,BIND_music_fade_in);
	lua_setfield(l,-2,"fade_in");

	void *handle = lua_newuserdata(l, sizeof(void*));
	/*
		This handle is... not so nice, but it does what it needs to do 
		Lua doesn't do garbage collection on light userdata, so we need
		some full userdata to use as a beacon for when it is time to 
		free some surface data!
	*/
	long music_ptr = (long)temp_music;
	memcpy(handle,(void*)&music_ptr,sizeof(void*)); /* Store the surface's address in the handle */
	lua_setfield(l,-2,"beacon");
	lua_getfield(l,-1,"beacon");
	lua_newtable(l);
	/* Create a blank table to store metatable functions */
	lua_pushcfunction(l, META_music_gc);
	lua_setfield(l, -2, "__gc");
	lua_setmetatable(l,-2);
	/* 
		Push on meta function for music clean up, name it __gc, 
		and finally wrap up by setting 'addr's metatable to the
		new table that was just made
	*/
	lua_remove(l, -1); /* remove beacon from the stack */
	return 1;
}

int BIND_music_play(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=1)
	{
		set_error(l,"ERROR: music_play - Arg Count Mismatch - Expecting music table");
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: music_play - Music isn't a table");
	}
	
	lua_getfield(l,1,"addr");
	
	Mix_Music *temp_music = lua_touserdata(l,-1);
	if(!temp_music)
	{	
		set_error(l,"ERROR: music_play - Music is invalid");
		return 0;
	}
	Mix_PlayMusic(temp_music, -1);
	return 0;
}

int BIND_music_pause(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	Mix_PauseMusic();
	return 0;
}

int BIND_music_resume(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	Mix_ResumeMusic();
	return 0;
}

int BIND_music_stop(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	Mix_HaltMusic();
	return 0;
}

int BIND_music_fade_in(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	if(lua_gettop(l)!=3)
	{
		set_error(l, "ERROR: music_fade_in - Arg Count Mismatch - Expecting music table, loops, miliseconds to fade");
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: music_fade_in - Music is not a table");
		return 0;
	}
	
	if(!lua_isnumber(l,2))
	{
		set_error(l,"ERROR: music_fade_in - loop amount is not a number");
		return 0;
	}
	
	if(!lua_isnumber(l,3))
	{
		set_error(l,"ERROR: music_fade_in - fade in time is not a number");
		return 0;	
	}
	int loops = lua_tointeger(l,2);
	int milisecs = lua_tointeger(l,3);
	lua_getfield(l,1,"addr");
	
	Mix_Music *temp_music = lua_touserdata(l,-1);
	if(!temp_music)
	{
		set_error(l, "ERROR: music_fade_in - Music is invalid");
		return 0;
	}
	Mix_FadeInMusic(temp_music,loops,milisecs);
	return 0;
}

int BIND_music_fade_out(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: music_fade_out - Arg Count Mismatch - Expecting fade out time in milis");
		return 0;
	}
	if(!lua_isnumber(l,1))
	{
		set_error(l,"ERROR: music_fade_out - Fade out time is not a number");
		return 0;
	}
	
	int milisecs = lua_tointeger(l,1);
	
	Mix_FadeOutMusic(milisecs);
	return 0;
}

int BIND_music_set_volume(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count!=1)
	{
		set_error(l, "ERROR: music_set_volume - Arg Count Mismatch - Expecting global volume");
		return 0;
	}
	if(!lua_isnumber(l,1))
	{
		set_error(l,"ERROR: music_fade_out - Fade out time is not a number");
		return 0;
	}
	int volume = lua_tointeger(l,1);
	Mix_VolumeMusic(volume);
	return 0;
}

int BIND_music_destroy(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* This bind free's the music currently loaded */
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: music_destroy - Arg Count Mismatch - Expecting music table"); // You can't stop rock n roll
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: music_destroy - Music is not a table");
		return 0;
	}
	
	lua_getfield(l,1,"addr");
	Mix_HaltMusic();
	if(!lua_isuserdata(l,-1))
	{
		set_error(l, "ERROR: music_destroy - Music is invalid");
		return 0;
	}
	Mix_Music *temp_music = lua_touserdata(l,-1);
	Mix_FreeMusic(temp_music);

	lua_getfield(l,1,"beacon");
	lua_pushnil(l);
	lua_setmetatable(l, -2); /* Remove the metatable to prevent double free'ing */
	lua_remove(l, -1);
	lua_pushnil(l);
	lua_setfield(l, 1, "beacon"); /* Set the beacon to nil- should encourage lua's garbage collection */

	return 0;
}

int BIND_sound_load(struct lua_State *l)
{
	if(lua_gettop(l)!=1)
	{
		set_error(l,"ERROR: sound_load - Arg Count Mismatch - Expecting sound file name");
		return 0;
	}
	
	if(!lua_isstring(l,1))
	{
		set_error(l,"ERROR: sound_load - Sound file name is not a string");
		return 0;
	}
	Mix_Chunk *temp_sound = NULL;
	
	char *filename = parse_filename((char*)lua_tostring(l,1));
	if(init_audio)
		temp_sound = Mix_LoadWAV(filename);
	free(filename);
	if(!temp_sound && init_audio)
	{
		set_error(l,"ERROR: sound_load - Sound couldn't be loaded");
		return 0;
	}
	lua_newtable(l);
	lua_pushnumber(l,-1);
	lua_setfield(l,-2,"channel");
	lua_pushlightuserdata(l,temp_sound);
	lua_setfield(l,-2,"addr");
	
	void *handle = lua_newuserdata(l, sizeof(void*));
	/*
		This handle is... not so nice, but it does what it needs to do 
		Lua doesn't do garbage collection on light userdata, so we need
		some full userdata to use as a beacon for when it is time to 
		free some surface data!
	*/
	long sound_ptr = (long)temp_sound;
	memcpy(handle,(void*)&sound_ptr,sizeof(void*)); /* Store the surface's address in the handle */
	lua_setfield(l,-2,"beacon");
	lua_getfield(l,-1,"beacon");
	lua_newtable(l);
	/* Create a blank table to store metatable functions */
	lua_pushcfunction(l, META_sound_gc);
	lua_setfield(l, -2, "__gc");
	lua_setmetatable(l,-2);
	/* 
		Push on meta function for sound clean up, name it __gc, 
		and finally wrap up by setting 'addr's metatable to the
		new table that was just made
	*/
	lua_remove(l, -1); /* remove beacon from the stack */
	
	lua_pushcfunction(l, BIND_sound_play);
	lua_setfield(l, -2, "play");

	lua_pushcfunction(l, BIND_sound_stop);
	lua_setfield(l, -2, "stop");

	lua_pushcfunction(l, BIND_sound_pause);
	lua_setfield(l, -2, "pause");
	
	lua_pushcfunction(l, BIND_sound_resume);
	lua_setfield(l, -2, "resume");

	lua_pushcfunction(l, BIND_sound_destroy);
	lua_setfield(l, -2, "destroy");

	return 1;
}

int BIND_sound_play(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: sound_play - Arg Count Mismatch - Expecting sound table");
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: sound_play - Sound is not a table");
		return 0;
	}
	
	lua_getfield(l,1,"addr");
	
	Mix_Chunk *temp_sound = lua_touserdata(l,-1);
	if(!temp_sound)
	{
		set_error(l, "ERROR: sound_play - Sound is invalid");
		return 0;
	}
	int channel = Mix_PlayChannel(-1,temp_sound, 0);
	lua_pushinteger(l,channel);
	lua_setfield(l,1,"channel");
	return 0;
}

int BIND_sound_stop(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: sound_stop - Arg Count Mismatch - Expecting sound table");
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: sound_stop - Sound is not a table");
	}
	
	lua_getfield(l,1,"channel");
	int channel = lua_tointeger(l,-1);
	
	Mix_HaltChannel(channel);
	
	lua_pushinteger(l,-1);
	lua_setfield(l,1,"channel");
	return 0;
}

int BIND_sound_stop_all(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=0)
	{
		set_error(l, "ERROR: sound_stop_all - Arg Count Mismatch - Requires no arguements");
		return 0;
	}
	
	Mix_HaltChannel(-1);
	
	return 0;
}

int BIND_sound_resume(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: sound_resume - Arg Count Mismatch - Expecting sound table");
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: sound_resume - Sound is not a table");
		return 0;
	}
	
	lua_getfield(l,1,"channel");
	int channel = lua_tointeger(l,-1);
	
	Mix_Resume(channel);

	return 0;
}

int BIND_sound_resume_all(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=0)
	{
		set_error(l, "ERROR: sound_resume_all - Arg Count Mismatch - Expecting no arguments");
		return 0;
	}
	
	Mix_Resume(-1);
	
	return 0;
}

int BIND_sound_pause(struct lua_State *l)
{	
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: sound_pause - Arg Count Mismatch - Expecting sound table");
		return 0;
	}
	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: sound_pause - Sound is not a table");
		return 0;
	}
	
	lua_getfield(l,1,"channel");
	int channel = lua_tointeger(l,-1);
	
	Mix_Pause(channel);
	
	return 0;
}

int BIND_sound_pause_all(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=0)
	{
		set_error(l, "ERROR: sound_pause - Arg Count Mismatch - Expecting no arguments");
		return 0;
	}

	Mix_Pause(-1);
	return 0;
}

int BIND_sound_destroy(struct lua_State *l)
{
	if(!init_audio)
		return 0;
	/* Play music */
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: sound_destroy - Arg Count Mismatch - Expecting sound table");
		return 0;
	}

	if(!lua_istable(l,1))
	{
		set_error(l,"ERROR: sound_destroy - Sound is not a table");
	}

	lua_getfield(l,1,"addr");

	Mix_Chunk *temp_sound = lua_touserdata(l,-1);
	if(!temp_sound)
	{
		set_error(l,"ERROR: sound_destroy - Sound is invalid");
		return 0;
	}
	Mix_FreeChunk(temp_sound);

	lua_getfield(l,1,"beacon");
	lua_pushnil(l);
	lua_setmetatable(l, -2); /* Remove the metatable to prevent double free'ing */
	lua_remove(l, -1);
	lua_pushnil(l);
	lua_setfield(l, 1, "beacon"); /* Set the beacon to nil- should encourage lua's garbage collection */

	return 0;
}

#endif
#ifdef SDL_NET

/*
	SDL_net Bindings
	
	It's really hard to abstract networking to the point that it is seamless without
	destroying any form of smoothness, so sadly, networking is something advanced
	users will only probably be able to get.
	
	PROVE ME WRONG!
*/

int BIND_socket_tcp_connect(struct lua_State *l)
{
	if(!init_net)
	{
		lua_pushboolean(l, 0); /* Never return true if net wasn't init'd */
		return 1;
	}
	int arg_count = lua_gettop(l);
	if(arg_count != 2)
	{
		set_error(l, "ERROR: socket_tcp_connect - Arg Count Mismatch - Expecting host string, port");
		return 0;
	}
	
	char *host = (char*)luaL_checkstring(l,1);
	int port = luaL_checkinteger(l,2);
	
	IPaddress ip;
	TCPsocket tcpsock = NULL;
	if(SDLNet_ResolveHost(&ip,host,port)==-1) 
	{
		/* fprintf(stderr,"SDLNet_ResolveHost: %s\n", SDLNet_GetError()); */
		lua_pushboolean(l, 0);
		return 1;
	}
	tcpsock = SDLNet_TCP_Open(&ip);
	if(!tcpsock) 
	{
		/* fprintf(stderr,"SDLNet_TCP_Open: %s\n", SDLNet_GetError()); */
		lua_pushboolean(l, 0);
		return 1;
	}
	
	lua_newtable(l);
	lua_pushlightuserdata(l,tcpsock);
	lua_setfield(l,-2,"socketfd");
	lua_pushstring(l,host);
	lua_setfield(l,-2,"host");
	lua_pushinteger(l,port);
	lua_setfield(l,-2,"port");
	lua_pushboolean(l, 1);
	lua_setfield(l, -2, "open");
	lua_pushboolean(l, 0);
	lua_setfield(l, -2, "listening");
	/* Non-listening TCP Socket Specific Functions */
	lua_pushcfunction(l,BIND_socket_tcp_send);
	lua_setfield(l, -2, "send");
	lua_pushcfunction(l,BIND_socket_tcp_sendstring);
	lua_setfield(l, -2, "sendstring");
	lua_pushcfunction(l,BIND_socket_tcp_recv);
	lua_setfield(l, -2, "recv");
	lua_pushcfunction(l,BIND_socket_tcp_readstring);
	lua_setfield(l, -2, "readstring");
	lua_pushcfunction(l,BIND_socket_tcp_readline);
	lua_setfield(l, -2, "readline");
	lua_pushcfunction(l,BIND_socket_ready);
	lua_setfield(l, -2, "ready");
	lua_pushcfunction(l,BIND_socket_tcp_close);
	lua_setfield(l, -2, "close");
	return 1;
}

int BIND_socket_tcp_host(struct lua_State *l)
{
	if(!init_net)
	{
		lua_pushboolean(l, 0); /* Never return a table if net wasn't init'd */
		return 1;
	}
	/* Create a hosting socket! */
	int arg_count = lua_gettop(l);
	if(arg_count != 1)
	{
		set_error(l,"ERROR: socket_tcp_host - Arg Count Mismatch - Expecting port");
		return 0;
	}
	
	int port = luaL_checkinteger(l,1);
	
	IPaddress ip;
	TCPsocket tcpsock;
	if(SDLNet_ResolveHost(&ip,NULL,port)==-1) 
	{
		/* fprintf(stderr,"SDLNet_ResolveHost: %s\n", SDLNet_GetError()); */
		lua_pushboolean(l, 0);
		return 1;
	}
	tcpsock = SDLNet_TCP_Open(&ip);
	if(!tcpsock) 
	{
		/* fprintf(stderr,"SDLNet_TCP_Open: %s\n", SDLNet_GetError()); */
		lua_pushboolean(l, 0);
		return 1;
	}
	
	lua_newtable(l);
	lua_pushlightuserdata(l,tcpsock);
	lua_setfield(l,-2,"socketfd");
	lua_pushstring(l,"Unknown"); /* TODO: Get the new socket's hostname */
	lua_setfield(l,-2,"host");
	lua_pushinteger(l,0);		/* TODO: Get the new socket's port */
	lua_setfield(l,-2,"port");
	lua_pushboolean(l, 1);
	lua_setfield(l, -2, "open");
	lua_pushboolean(l, 1);
	lua_setfield(l, -2, "listening");
	
	/* Non-listening TCP Socket Specific Functions */
	lua_pushcfunction(l,BIND_socket_ready);
	lua_setfield(l, -2, "ready");
	lua_pushcfunction(l,BIND_socket_tcp_accept);
	lua_setfield(l, -2, "accept");
	lua_pushcfunction(l,BIND_socket_tcp_close);
	lua_setfield(l, -2, "close");
	
	return 1;
}

int BIND_socket_tcp_accept(struct lua_State *l)
{
	if(!init_net)
	{
		lua_pushboolean(l, 0); /* Never return true if net wasn't init'd */
		return 0;
	}
	/* ACCEPT INCOMING CONNECTIONS! */
	int arg_count = lua_gettop(l);
	if(arg_count != 1)
	{
		set_error(l, "ERROR: socket_tcp_accept - Arg Count Mismatch - Expecting socket table");
		return 0;
	}
	lua_getfield(l,1,"open");
	if(!lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_accept - Socket isn't open - This socket isn't hosting, make sure the listen function didn't fail.");
		return 0;
	}
	lua_getfield(l,1,"listening");
	if(!lua_toboolean(l,-1))
	{
		fprintf(stderr, "ERROR: socket_tcp_accept - The socket isn't listening. You can only accept connections on sockets created using the socket_tcp_host function.");
		return 0;
	}
	
	lua_getfield(l, 1, "socketfd");
	TCPsocket host = (TCPsocket)lua_touserdata(l,-1);
	if(!host)
	{
		set_error(l, "ERROR: socket_tcp_accept - Socket is invalid");
		return 0;
	}
	TCPsocket storesock;

	storesock = SDLNet_TCP_Accept(host);
	if(!storesock) 
	{
		lua_pushboolean(l, 0);
		return 1;
	}
	
	lua_newtable(l);
	lua_pushlightuserdata(l,storesock);
	lua_setfield(l,-2,"socketfd");
	lua_pushstring(l,"Unknown"); /* TODO: Get the new socket's hostname */
	lua_setfield(l,-2,"host");
	lua_pushinteger(l,0);		/* TODO: Get the new socket's port */
	lua_setfield(l,-2,"port");
	lua_pushboolean(l, 1);
	lua_setfield(l, -2, "open");
	lua_pushboolean(l, 0);
	lua_setfield(l, -2, "listening");
	
	/* Non-listening TCP Socket Specific Functions */
	lua_pushcfunction(l,BIND_socket_tcp_send);
	lua_setfield(l, -2, "send");
	lua_pushcfunction(l,BIND_socket_tcp_sendstring);
	lua_setfield(l, -2, "sendstring");
	lua_pushcfunction(l,BIND_socket_tcp_recv);
	lua_setfield(l, -2, "recv");
	lua_pushcfunction(l,BIND_socket_tcp_readstring);
	lua_setfield(l, -2, "readstring");
	lua_pushcfunction(l,BIND_socket_tcp_readline);
	lua_setfield(l, -2, "readline");
	lua_pushcfunction(l,BIND_socket_ready);
	lua_setfield(l, -2, "ready");
	lua_pushcfunction(l,BIND_socket_tcp_close);
	lua_setfield(l, -2, "close");

	return 1;
}

int BIND_socket_tcp_send(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count != 3)
	{
		set_error(l, "ERROR: socket_tcp_send - Arg Count Mismatch - Expecting socket table, data, length");
		return 0;
	}
	lua_getfield(l,1,"open");
	if(!lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_send - Socket is not open for sending. Make sure you've connected to a host.");
		return 0;
	}
	lua_getfield(l,1,"listening");
	if(lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_send - Socket is listening. You cannot send data from a listening socket.");
		return 0;
	}
	lua_getfield(l, 1, "socketfd");
	TCPsocket tcpsock = lua_touserdata(l,-1);
	if(!tcpsock)
	{
		set_error(l, "ERROR: socket_tcp_send - Socket is invalid");
		return 0;
	}
	char *data = (char*)lua_tostring(l,2);
	int len = 0; 
	if(arg_count==2)
	{
		len = strlen(data)+1;
	}
	else
	{
		len = luaL_checkint(l, 3);
	}

	int result = SDLNet_TCP_Send(tcpsock,data,len);
	lua_pushnumber(l, result);
	return 1;
}

int BIND_socket_tcp_sendstring(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count != 2 && arg_count != 3)
	{
		set_error(l, "ERROR: socket_tcp_sendstring - Arg Count Mismatch - Expecting socket table, string[, send 0 byte ]");
		return 0;
	}
	lua_getfield(l,1,"open");
	if(!lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_send - Socket is not open for sending. Make sure you've connected to a host.");
		return 0;
	}
	lua_getfield(l,1,"listening");
	if(lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_send - Socket is listening. You cannot send data from a listening socket.");
		return 0;
	}
	lua_getfield(l, 1, "socketfd");
	TCPsocket tcpsock = lua_touserdata(l,-1);
	if(!tcpsock)
	{
		set_error(l, "ERROR: socket_tcp_send - Socket is invalid");
		return 0;
	}
	char *data = (char*)lua_tostring(l,2);
	int len = 0; 
	if(arg_count==3)
	{
		len = strlen(data)+1;
	}
	else
	{
		len = strlen(data);
	}
	
	int result = SDLNet_TCP_Send(tcpsock,data,len);
	lua_pushnumber(l, result);
	return 1;
}

int BIND_socket_tcp_recv(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count != 2)
	{
		set_error(l, "ERROR: socket_tcp_recv - Arg Count Mismatch - Expecting socket table, length");
		return 0;
	}
	lua_getfield(l,1,"open");
	if(!lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_recv - Socket not open for reading. Make sure you've connected to a host");
		return 0;
	}
	lua_getfield(l,1,"listening");
	if(lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_recv - Socket is listening. You cannot read data from a listening socket, if there is data, it will be an incoming connection. Hint: Use socket_tcp_accept()");
		return 0;
	}
	lua_getfield(l,1,"socketfd");
	TCPsocket tcpsock = lua_touserdata(l,-1);
	if(!tcpsock)
	{
		set_error(l, "ERROR: socket_tcp_recv - Socket is invalid");
		return 0;
	}
	int amount = luaL_checkint(l, 2);
	char *data = malloc(amount+1);
	memset(data, '\0', amount+1);
	SDLNet_TCP_Recv(tcpsock, &data, amount);
	lua_pushstring(l, data);
	
	return 1;
}

int BIND_socket_tcp_readline(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count !=1)
	{
		set_error(l, "ERROR: socket_tcp_readline - Arg Count Mismatch - Expecting socket table");
		return 0;
	}
	lua_getfield(l,1,"open");
	if(!lua_toboolean(l,-1))
	{
		set_error(l,"ERROR: socket_tcp_readline - Socket not open for reading. Make sure you've connected to a host");
		return 0;
	}
	lua_getfield(l,1,"listening");
	if(lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_readline - Socket is listening. You cannot read data from a listening socket, if there is data, it will be an incoming connection. Hint: Use socket_tcp_accept()");
		return 0;
	}
	lua_getfield(l,1,"socketfd");
	TCPsocket tcpsock = lua_touserdata(l,-1);
	if(!tcpsock)
	{
		set_error(l, "ERROR: socket_tcp_readline - Socket is invalid");
		return 0;
	}
	char *data = malloc(1500); /* MTU of LAN */
	memset(data, '\0', 1500);
	char buffer = -1;
	int offset = 0;
	while(buffer!='\n')
	{
		SDLNet_TCP_Recv(tcpsock, &buffer, 1);
		data[offset++] = buffer;
		if(offset == 1499)
		{
			fprintf(stderr, "SDLNet_TCP_Rec, packet too big!");
			return 0;
		}
	}	
	char *ac_data = strdup(data); 
	free(data); /* Why feesh, WHY!!, there might be some 1450 bytes not used.. and well they may never get freed, so copy what we need and free the rest */
	lua_pushstring(l, ac_data);

	return 1;
}

int BIND_socket_tcp_readstring(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count !=1 && arg_count != 2)
	{
		set_error(l, "ERROR: socket_tcp_readstring - Arg Count Mismatch - Expecting socket table");
		return 0;
	}
	lua_getfield(l,1,"open");
	if(!lua_toboolean(l,-1))
	{
		set_error(l,"ERROR: socket_tcp_readstring - Socket not open for reading. Make sure you've connected to a host");
		return 0;
	}
	lua_getfield(l,1,"listening");
	if(lua_toboolean(l,-1))
	{
		set_error(l, "ERROR: socket_tcp_readstring - Socket is listening. You cannot read data from a listening socket, if there is data, it will be an incoming connection. Hint: Use socket_tcp_accept()");
		return 0;
	}
	lua_getfield(l,1,"socketfd");
	TCPsocket tcpsock = lua_touserdata(l,-1);
	if(!tcpsock)
	{
		set_error(l, "ERROR: socket_tcp_readstring - Socket is invalid");
		return 0;
	}
	char *data = malloc(1500); /* MTU of LAN */
	memset(data, '\0', 1500);
	char buffer = -1;
	int offset = 0;
	while(buffer!=0)
	{
		SDLNet_TCP_Recv(tcpsock, &buffer, 1);
		data[offset++] = buffer;
		if(offset == 1499)
		{
			fprintf(stderr, "SDLNet_TCP_Rec, packet too big!");
			return 0;
		}
	}	
	char *ac_data = strdup(data); 
	free(data); /* Why feesh, WHY!!, there might be some 1450 bytes not used.. and well they may never get freed, so copy what we need and free the rest */
	lua_pushstring(l, ac_data);

	return 1;
}

int BIND_socket_tcp_close(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count !=1)
	{
		set_error(l, "socket_tcp_close - Arg Count Mismatch - Expecting socket table");
		return 0;
	}

	lua_getfield(l,1,"socketfd");
	TCPsocket tcpsock = lua_touserdata(l,-1);
	if(!tcpsock)
	{
		set_error(l, "ERROR: socket_tcp_close - Socket is invalid");
		return 0;
	}
	lua_pushboolean(l,0);
	lua_setfield(l, 1, "open");
	SDLNet_TCP_Close(tcpsock);
	return 0;
}

int BIND_socket_set_create(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count!=1)
	{
		set_error(l,"ERROR: socket_set_create - Arg Count Mismatch - Expecting number of sockets maximum");
		return 0;
	}
	int amount = lua_tonumber(l,1);
	SDLNet_SocketSet new_set = SDLNet_AllocSocketSet(amount);
	if(!new_set)
	{
		set_error(l, "ERROR: socket_set_create - Failed to create the socket set");
		return 0;
	}
	lua_newtable(l);
	lua_pushlightuserdata(l,new_set);
	lua_setfield(l, -2, "addr");
	lua_pushcfunction(l, BIND_socket_set_tcp_add);
	lua_setfield(l, -2, "tcp_add");
	lua_pushcfunction(l, BIND_socket_set_tcp_remove);
	lua_setfield(l, -2, "tcp_remove");
	lua_pushcfunction(l, BIND_socket_set_destroy);
	lua_setfield(l, -2, "destroy");
	lua_pushcfunction(l, BIND_socket_set_check);
	lua_setfield(l, -2, "check");
	
	return 1;
}

int BIND_socket_set_tcp_add(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count!=2)
	{
		set_error(l, "ERROR: socket_set_add - Arg Count Mismatch - Expecting socket set table, socket table");
		return 0;
	}
	lua_getfield(l, 1, "addr");
	SDLNet_SocketSet socket_set = lua_touserdata(l,-1);
	lua_getfield(l, 2, "socketfd");
	TCPsocket sock = lua_touserdata(l,-1);
	if(SDLNet_TCP_AddSocket(socket_set, sock)<0)
	{
		/* set_error(l, "ERROR: socket_set_tcp_add - Failed adding socket - Might of exceeded original storage size");*/
		lua_pushboolean(l, 1);
		return 1;
	}
	lua_pushboolean(l,0);
	return 1;
}

int BIND_socket_set_tcp_remove(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count!=2)
	{
		set_error(l, "ERROR: socket_set_tcp_remove - Arg Count Mismatch - Expecting socket set table, socket table");
		return 0;
	}
	lua_getfield(l, 1, "addr");
	SDLNet_SocketSet socket_set = lua_touserdata(l,-1);
	lua_getfield(l,2,"addr");
	TCPsocket sock = lua_touserdata(l,-1);
	SDLNet_TCP_DelSocket(socket_set, sock);
	
	return 0;
}

int BIND_socket_set_destroy(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count!=1)
	{
		set_error(l, "ERROR: socket_set_destroy - Arg Count Mismatch - Expecting socket set");
		return 0;
	}
	lua_getfield(l, 1, "addr");
	SDLNet_SocketSet socket_set = lua_touserdata(l,-1);
	SDLNet_FreeSocketSet(socket_set);
	
	return 0;
}

int BIND_socket_set_check(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count!=2)
	{
		set_error(l, "ERROR: socket_set_check - Arg Count Mismatch - Expecting socket set, timeout");
		return 0;
	}
	lua_getfield(l, 1, "addr");
	SDLNet_SocketSet socket_set = lua_touserdata(l,-1);
	int timeout = luaL_checkint(l, 2);

	int numready = SDLNet_CheckSockets(socket_set, timeout);

	lua_pushnumber(l, numready);	

	return 1;
}

int BIND_socket_ready(struct lua_State *l)
{
	if(!init_net)
		return 0;
	int arg_count = lua_gettop(l);
	if(arg_count!=1)
	{
		set_error(l, "ERROR: socket_ready - Arg Count Mismatch - Expecting socket table");
		return 0;
	}
	lua_getfield(l,1,"open");
	if(!lua_toboolean(l,-1))
	{
		fprintf(stderr, "ERROR: socket_ready - Socket is not open for polling. You have to connect to a host, or the socket has to be hosting to check if it's ready.");
		return 0;
	}
	lua_getfield(l, 1, "socketfd");
	TCPsocket test_sock = lua_touserdata(l,-1);
	if(SDLNet_SocketReady(test_sock))
	{
		lua_pushnumber(l, 1);
		return 1;
	}
	lua_pushnumber(l, 0);	
	return 1;
}

#endif


/* Misc. Binds */

int BIND_set_ticks(struct lua_State *l)
{
	if(lua_gettop(l)!=1)
	{
		set_error(l, "ERROR: set_ticks - Arg Count Mismatch - Expecting new ms");
	}
	int new_ms = lua_tonumber(l,1);
	if(new_ms<10)
		new_ms = 10;

	vm_timer = new_ms;

	return 0;
}

int BIND_get_ticks(struct lua_State *l)
{
	if(lua_gettop(l)>0)
	{
		fprintf(stderr, "ERROR: get_ticks, takes no arguements\n");
		set_error(l, "get_ticks invalid arg count");
	}
	lua_pushnumber(l,SDL_GetTicks());
	return 1;
}

int BIND_mouse_show(struct lua_State *l)
{
	SDL_ShowCursor(SDL_ENABLE);
	return 0;
}

int BIND_mouse_hide(struct lua_State *l)
{
	SDL_ShowCursor(SDL_DISABLE);
	return 0;
}

int BIND_send_fatal_error(struct lua_State *l)
{
	if(lua_gettop(l)!=1)
	{
		set_error(l,"ERROR: send_fatal_error - Arg Count Mismatch - Expecting error string");
		return 0;
	}
	if(!lua_isstring(l,1))
	{
		set_error(l, "ERROR: send_fatal_error - Error is not of type string");
		return 0;
	}
	set_error(l,(char*)lua_tostring(l,1));
	return 0;
}

int BIND_quit(struct lua_State *l)
{
	/* Cheeky way of quitting the vm - I'm a bad wittle boi */
	SDL_Event quit_event;
	quit_event.type = SDL_QUIT;
	SDL_PushEvent(&quit_event);
	return 0;
}

int BIND_get_key(struct lua_State *l)
{
	int args = lua_gettop(l);
	if(args==0)
	{
		SDL_PumpEvents();
		Uint8 *keystate = SDL_GetKeyState(NULL);
		int i;
		for(i=0;i<255;i++)
		{
			if(keystate[i]==1)
			{
				lua_pushinteger(l,i);
				return 1;
			}
		}
		lua_pushboolean(l,0);
		return 1;
	}
	
	int check;
	check = luaL_checkint(l,1);
	
	SDL_PumpEvents();
	Uint8 *keystate = SDL_GetKeyState(NULL);
	lua_pushboolean(l,keystate[check]);
	return 1;
}

int BIND_get_mouse(struct lua_State *l)
{
	int args = lua_gettop(l);
	int x, y, button = 0;
	if(args>0)
	{
		button = luaL_checkint(l,1);	
	}
	SDL_PumpEvents();
	button = SDL_GetMouseState(&x, &y)&SDL_BUTTON(button);
	lua_pushinteger(l,x);
	lua_pushinteger(l,y);
	lua_pushinteger(l,button);
	return 3;
}

int BIND_get_joy_count(struct lua_State *l)
{
	lua_pushinteger(l,joystick_count);
	return 1;
}

int BIND_get_joy_button(struct lua_State *l)
{
	int args = lua_gettop(l);
	int button;
	int joystick = 0;
	if(args!=1 && args!=2)
	{
		set_error(l,"ERROR: get_joy_button - Arg Count Mismatch - button to check [, joystick (default 0)]");
		return 0; 
	}
	if(args == 2)
	{
		joystick = luaL_checkint(l, 2);
	}

	button = luaL_checkint(l,1);	
	if(button > SDL_JoystickNumButtons(joy[joystick]))
	{
		/* set_error(l, "Joystick doesn't have that many buttons!");  Stupid error ? */
		lua_pushboolean(l, 0);
		return 1;
	}
	SDL_JoystickUpdate();
	button = SDL_JoystickGetButton(joy[joystick], button);
	lua_pushboolean(l,button);
	return 1;
}

int BIND_get_joy_axis(struct lua_State *l)
{
	int args = lua_gettop(l);
	int joystick = 0;
	if(args!=1 && args!=2)
	{
		set_error(l,"ERROR: get_joy_axis - Arg Count Mismatch - Expecting joy axis to check [, joystick (default 0)]");
		return 0;
	}
	if(args == 2)
	{
		joystick = luaL_checkint(l, 2);
	}
	int axis = luaL_checkint(l,1);
#ifdef GP2X
	SDL_JoystickUpdate();
	int button = 0;
	if(axis == 0) /* X AXIS */
	{
		button = SDL_JoystickGetButton(joy[0], 2);
		if(button)
		
		else
#else
	SDL_JoystickUpdate();
	axis = SDL_JoystickGetAxis(joy[joystick],axis);
	axis = (axis/32768.0)*100; /* Return motion in a percentage */
	lua_pushinteger(l,axis);
#endif
	return 1;
}

int BIND_get_joy_axis_count(struct lua_State *l)
{
	int args = lua_gettop(l);
	int joystick = 0;
	if(args == 1)
	{
		joystick = luaL_checkint(l, 1);
	}
#ifdef GP2X
	lua_pushinteger(l,SDL_JoystickNumAxes(joy[0]));
#else
	lua_pushinteger(l,SDL_JoystickNumAxes(joy[joystick]));
#endif
	return 1;
}

int BIND_get_joy_button_count(struct lua_State *l)
{
	int args = lua_gettop(l);
	int joystick = 0;
	if(args == 1)
	{
		joystick = luaL_checkint(l, 1);
	}
#ifdef GP2X
	lua_pushinteger(l,SDL_JoystickNumButtons(joy[joystick]));
#else
	lua_pushinteger(l,SDL_JoystickNumButtons(joy[joystick]));
#endif
	return 1;
}

int BIND_reset_video(struct lua_State *l)
{
	/* This can be used to set fullscreen and change resolution size on the fly! */
	lua_getglobal(l, "Game");
	lua_getfield(l,-1,"w");
	unsigned int w = lua_tointeger(l,-1);
	
	lua_getfield(l,-2,"h");
	unsigned int h = lua_tointeger(l,-1);
	lua_getfield(l,-3,"fullscreen");
	unsigned int fs = lua_tointeger(l,-1);
	if(fs)
	{
		if(!fullscreen)
			SDL_WM_ToggleFullScreen(screen);
		screen = SDL_SetVideoMode(w, h, bpp, init_surfaces|SDL_FULLSCREEN);
	}
	else
	{
		screen = SDL_SetVideoMode(w, h, bpp, init_surfaces);
	}
		
	if(screen)
	{
		screenw = w;
		screenh = h;
		fullscreen = fs;
		lua_getglobal(l,"on_resize");
		if(!lua_isnil(l,-1))
		{
			int err = lua_pcall(l,0,0,0);
			if(err)
			{
				set_error(l,(char*)lua_tostring(l, -1));
			}
		}
	}
	else
	{
		SDL_SetVideoMode(screenw, screenh, bpp, init_surfaces);
		lua_pushboolean(l,0);
		return 1;
	}
	
	lua_pushboolean(l, 1);
	return 1;
}

void binds_register(struct lua_State *l)
{
	lua_newtable(l);
	luaL_setfuncs(l,sdl_binds, 0);
	lua_setglobal(l, "SDL");
}
