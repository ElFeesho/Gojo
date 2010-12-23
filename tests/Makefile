OPTS=`pkg-config --libs --cflags sdl`

all: net.c gfx.c ttf.c mixer.c image.c
	gcc net.c -o net $(OPTS) -lSDL_net
	gcc gfx.c -o gfx $(OPTS) -lSDL_gfx
	gcc ttf.c -o ttf $(OPTS) -lSDL_ttf
	gcc mixer.c -o mixer $(OPTS) -lSDL_mixer
	gcc image.c -o image $(OPTS) -lSDL_image
	gcc sdl.c -o sdl $(OPTS)

net:net.c
	gcc net.c -o net $(OPTS) -lSDL_net

gfx:gfx.c
	gcc gfx.c -o gfx $(OPTS) -lSDL_gfx

ttf:ttf.c
	gcc ttf.c -o ttf $(OPTS) -lSDL_ttf

mixer:mixer.c
	gcc mixer.c -o mixer $(OPTS) -lSDL_mixer

image:image.c
	gcc image.c -o image $(OPTS) -lSDL_image

sdl:sdl.c
	gcc sdl.c -o sdl $(OPTS) 

ondemand:on_demand.c
	gcc on_demand.c -o on_demand $(OPTS) 

clean:
	rm -f image mixer ttf gfx net
