#!/bin/bash

# Generate project steps...
# ./make_project.sh CDT 	= for eclipse file generated
# ./make_project.sh CDT ARM = for eclipse file generated with Cross Compile for arm
# ./make_project.sh ARM 	= Only Cross Compile for arm

# Comment it for Release version build
DEBUG="DEBUG"

FIRST_PARA="$1"
SECOND_PARA="$2"

#$@ for printing all argument
#$# for numbers of passed argument
echo "$# Arguments : $@"

if [ $# -ge '1' ] && [ $FIRST_PARA == 'CDT' ]; then

	echo "Make project with CDT..."
	if  [ $# == '2' ] && [ $SECOND_PARA == 'ARM' ]; then
		echo "Cross compiling..."
		cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=rpi_toolchain.cmake -D CMAKE_BUILD_TYPE=$DEBUG -lpthread .
	else
		cmake -G "Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=$DEBUG -lpthread .
	fi

else

	echo "Make project without CDT..."
	if [ $# -eq '1' ] && [ $FIRST_PARA == 'ARM' ]; then
		echo "Cross compiling..."
		cmake -DCMAKE_TOOLCHAIN_FILE=rpi_toolchain.cmake -D CMAKE_BUILD_TYPE=$Debug -lpthread .
	else
		cmake -D CMAKE_BUILD_TYPE=$Debug -lpthread .
	fi

fi
