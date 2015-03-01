cmake_minimum_required(VERSION 2.8)
project(nfc)

set(RZ_ROOT "../source")
message(status "** RZ_ROOT Include: ${RZ_ROOT}")
#add_definitions(-DBOOST_ALL_NO_LIB)  # tell the compiler to undefine this boost macro

SET(RZ_MAIN
	${RZ_ROOT}/../monitor.cpp
)

INCLUDE(
	${RZ_ROOT}/lib.cmake
)

FIND_PACKAGE (Threads REQUIRED )
add_executable(nfc 
	${RZ_MAIN} 
	${RZ_THREADAPI}
)
TARGET_LINK_LIBRARIES ( nfc ${CMAKE_THREAD_LIBS_INIT} )
