OBJS=objs/main.o objs/binds.o objs/conv_funcs.o objs/errors.o
CFLAGS=`pkg-config --cflags sdl` -Wall -march=core2 -pipe -g -I/usr/include/lua5.2
LIBS=`pkg-config --libs sdl` -llua5.1 -lm -lSDL_image -lSDL_gfx -lSDL_ttf -lSDL_mixer -lSDL_net
CC=gcc
PROG=gojo

all:$(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBS)

objs/%o:src/%c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(PROG)
