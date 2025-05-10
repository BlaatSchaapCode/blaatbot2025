#!/usr/bin/env bash

uname="$(uname)"



if  [[  "$uname" = "Haiku" ]] then
	GEBLAAT_ROOT=`pwd`/out/gcc/haiku/`uname -m`/debug
	LIBRARY_PATH=$LIBRARY_PATH:$GEBLAAT_ROOT/lib
	$GEBLAAT_ROOT/bin/blaatbot2025 -c $1
	exit
fi

if  [[  "$uname" = "Linux" ]] then
	GEBLAAT_ROOT=`pwd`/out/gcc/linux/`uname -m`/debug
	LD_LIBRARY_PATH=$GEBLAAT_ROOT/lib
	$GEBLAAT_ROOT/bin/blaatbot2025 -c $1
	exit
fi
