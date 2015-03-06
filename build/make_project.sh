#!/bin/bash

if [ "$1" == 'CDT' ]; then
	echo "Make project with CDT..."
	cmake -G "Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug -lpthread .
else
	echo "Make project without CDT..."
	cmake -D CMAKE_BUILD_TYPE=Debug -lpthread .
fi
