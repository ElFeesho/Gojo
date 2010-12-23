#!/bin/sh

if [ -z $1 ]; then
	echo Usage: $0 "<dynamically linked binary>"
	exit
fi

rm -f library_list 

recursive_track()
{
	while [ $1 ]; do
	ldd $1 | grep "=>" | egrep -o "/[^ ]*" >> library_list 
	recursive_track `ldd $1 | grep "=>" | egrep -o "/[^ ]*" | tr "\n" " "`
	
	shift
	done
	
}

recursive_track $1

sort library_list | uniq

rm library_list
