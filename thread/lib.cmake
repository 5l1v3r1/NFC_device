SET(RZ_THREADAPI
	${RZ_ROOT}/thread/tinythread.cpp
	${RZ_ROOT}/thread/tinythread.h
	${RZ_ROOT}/thread/fast_mutex.h	
)

#file(GLOB SOURCES ${RZ_ROOT}/*.cpp ${RZ_ROOT}/*.h)
#file(GLOB LIB_HEADERS ${RZ_ROOT}/*.h)
#add_library( threads ${LIB_SOURCES} ${LIB_HEADERS} )

source_group("threads" FILES ${RZ_THREADAPI})
message(status "** RZ_THREADAPI : ${RZ_THREADAPI}")
