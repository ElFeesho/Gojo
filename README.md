# Gojo

## What is Gojo?

Gojo is a program that reads in script files and produces visual
and audio multimedia based on the contents of the script file. 
Basically put, it is a LUA script interpretter with SDL bindings.

## What is Gojo's aim?

Gojo aims to make it easy to create cross platform games. By 
providing bindings to SDL, a portable lowlevel graphics library
with abstractions to other parts of OS's, it is easy to make
simple games that work on all platforms with minimal optimising
to any script files.

## How do I use Gojo?

Gojo needs 2 things in a script file to do anything useful, 
these are:

Game attributes in form of
```lua
	Game.w = 320
	Game.h = 240
	Game.name = "My Game"
	Game.author = "Me!"
```

An `on_update` function in form of
```lua
	function on_update(ticks)

	end
```

With these two things you can go on to start creating games.

## Isn't Gojo like Project XYZ?

Probably. Gojo wasn't made to beat any existing projects, but to just
offer another choice. If Gojo isn't quite doing it for you, I advise you
check out Fenix, or if you're so inclined, PyGame. The advantage of Gojo
is it has few (obscure) dependencies, it's portable and easy to port to new
systems. The main goals are to run code fast and efficiently and ease of 
coding games.
