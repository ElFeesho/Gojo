OBJS=objs/main.o objs/binds.o objs/conv_funcs.o objs/errors.o
CFLAGS=`pkg-config --cflags sdl` -Wall -pipe -g
LIBS=`pkg-config --libs sdl` -llua -lm -lSDL_image -lSDL_gfx -lSDL_ttf -lSDL_mixer -lSDL_net
CC=gcc
PROG=gojo

all:$(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBS)

objs/%o:src/%c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(PROG)
