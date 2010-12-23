#!/bin/sh

MAINSCRIPT=main.gj   #put the name of the main script to run here
GAMEPACK=		#specify a game pack (work in progress)
GOJO_INSIDE_PACK=	#tells script to use the gojo exec contained in the gamepack

## This is a template for inclusion with games. You should rename it and
## keep it executable so it will be obvious to run this script to launch
## the game.

## todo:
## * Make gamepack unpacking work.
## * Do standalone, self-extractor
## * utilize various compression tools (lzma, bzip2, zip)

#first, find gojo executable
if which gojo >/dev/null 2>&1; then
	#found gojo on the path, woo!
	GOJO=`which gojo`
elsif [ -x ./gojo ]; then
	#current directory?
	GOJO="./gojo"
elsif [ -x ../gojo ]; then
	#parent directory?
	GOJO="../gojo"
elsif [ -x ../../gojo ]; then
	#grand parent
	GOJO="../../gojo"
else
	#give up!
	echo "Can't find gojo executable! Giving up!" >&2
	exit 2
fi

if [ -e $MAINSCRIPT ]; then 
	echo "Launching Gojo..."
	if [ -z GAMEPACK ]; then
		echo this is where we would unpack the gamepack
	fi
	$GOJO $MAINSCRIPT
else
	echo "Script error: Can't find specified entry script: $MAINSCRIPT" >&2
fi
