#!/bin/sh

line=$( expr 1 + $( grep -an "__BEGIN_ARCHIVE__" $0  | tail -n1 | grep -o "[0-9]*" ) )
tmp=`mktemp -d`
#set these to the appropriate values
unpack_command="unzip "
unpack_arguments=" -d $tmp"
main_script="main.gj"
gojo_command="./gojo"
try_native_gojo=
cleanup=1

#okay, moving on.
echo "Copying...."
tail -n +$line $0 > ${tmp}/archive #|| { echo "Problem copying. X(" && exit }
echo "Unpacking..."
$unpack_command ${tmp}/archive $unpack_arguments #|| { echo "Problem unpacking. X(" && exit }
echo "Launching..."
cd $tmp
if which gojo >/dev/null 2>&1 && [ $try_native_gojo ];  then
	gojo_command=`which gojo`
fi
$gojo_command $main_script $*

#clean up
if [ $cleanup ]; then
	cd /
	rm -rf $tmp
fi

exit
__BEGIN_ARCHIVE__
