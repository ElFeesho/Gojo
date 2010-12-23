#ifndef __BINDS_H__
#define __BINDS_H__

#include "config.h"
#include <lua.h>
#include <lauxlib.h>

/* Prototypes */
int BIND_surface_load(struct lua_State *l);
int BIND_surface_create(struct lua_State *l);
int BIND_surface_fill(struct lua_State *l);
int BIND_surface_blit(struct lua_State *l);
int BIND_surface_blit_area(struct lua_State *l);
int BIND_surface_get_pixel(struct lua_State *l);
int BIND_surface_destroy(struct lua_State *l);
int BIND_draw_rect(struct lua_State *l);
	#ifdef SDL_GFX
int BIND_draw_line(struct lua_State *l);
	#endif
	#ifdef SDL_TTF
int BIND_text_load(struct lua_State *l);
int BIND_text_draw(struct lua_State *l);
int BIND_text_create(struct lua_State *l);
int BIND_text_destroy(struct lua_State *l);
	#endif
	#ifdef SDL_MIXER

int BIND_music_load(struct lua_State *l);
int BIND_music_play(struct lua_State *l);
int BIND_music_pause(struct lua_State *l);
int BIND_music_resume(struct lua_State *l);
int BIND_music_stop(struct lua_State *l);
int BIND_music_set_volume(struct lua_State *l);
int BIND_music_fade_in(struct lua_State *l);
int BIND_music_fade_out(struct lua_State *l);
int BIND_music_destroy(struct lua_State *l);

int BIND_sound_load(struct lua_State *l);
int BIND_sound_play(struct lua_State *l);
int BIND_sound_stop(struct lua_State *l);
int BIND_sound_stop_all(struct lua_State *l);
int BIND_sound_resume(struct lua_State *l);
int BIND_sound_resume_all(struct lua_State *l);
int BIND_sound_pause(struct lua_State *l);
int BIND_sound_pause_all(struct lua_State *l);
int BIND_sound_destroy(struct lua_State *l);

	#endif
	#ifdef SDL_NET
int BIND_socket_tcp_connect(struct lua_State *l);
int BIND_socket_tcp_host(struct lua_State *l);
int BIND_socket_tcp_accept(struct lua_State *l);
int BIND_socket_tcp_send(struct lua_State *l);
int BIND_socket_tcp_sendstring(struct lua_State *l);
int BIND_socket_tcp_recv(struct lua_State *l);
int BIND_socket_tcp_readline(struct lua_State *l);
int BIND_socket_tcp_readstring(struct lua_State *l);
int BIND_socket_tcp_close(struct lua_State *l);
int BIND_socket_set_create(struct lua_State *l);
int BIND_socket_set_tcp_add(struct lua_State *l);
int BIND_socket_set_tcp_remove(struct lua_State *l);
int BIND_socket_set_destroy(struct lua_State *l);
int BIND_socket_set_check(struct lua_State *l);
int BIND_socket_ready(struct lua_State *l);
	#endif

int BIND_mouse_show(struct lua_State *l);
int BIND_mouse_hide(struct lua_State *l);

int BIND_send_fatal_error(struct lua_State *l);

int BIND_get_ticks(struct lua_State *l);
int BIND_set_ticks(struct lua_State *l);
int BIND_get_key(struct lua_State *l);
int BIND_get_mouse(struct lua_State *l);
int BIND_get_joy_count(struct lua_State *l);
int BIND_get_joy_button(struct lua_State *l);
int BIND_get_joy_axis(struct lua_State *l);
int BIND_get_joy_axis_count(struct lua_State *l);
int BIND_get_joy_button_count(struct lua_State *l);
int BIND_reset_video(struct lua_State *l);

int BIND_quit(struct lua_State *l);

int META_surface_gc(struct lua_State *l);

#ifdef SDL_TTF

int META_text_gc(struct lua_State *l);

#endif

#ifdef SDL_MIXER

int META_music_gc(struct lua_State *l);
int META_sound_gc(struct lua_State *l);

#endif

void binds_register(struct lua_State *l);

#endif
