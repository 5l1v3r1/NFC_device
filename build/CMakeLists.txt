cmake_minimum_required(VERSION 2.8)
project(nfc)

set(RZ_ROOT "../")
message(status "** RZ_ROOT Include: ${RZ_ROOT}")
#add_definitions(-DBOOST_ALL_NO_LIB)  # tell the compiler to define this boost macro

SET(RZ_MAIN
	${RZ_ROOT}/main/monitor.cpp
)

set(RZ_MYLIB "${RZ_ROOT}/MyLib")
INCLUDE(
	${RZ_ROOT}/MyLib/MyLib.cmake
)

FIND_PACKAGE (Threads REQUIRED )
add_executable(nfc 
	${RZ_MAIN} 
	${RZ_THREADAPI}
	${RZ_FILESYSTEMAPI}
)
TARGET_LINK_LIBRARIES ( nfc ${CMAKE_THREAD_LIBS_INIT} )

