SET(RZ_FILESYSTEMAPI
	${RZ_MYLIB}/filesystem/_base.h
	${RZ_MYLIB}/filesystem/fileApi.h
	${RZ_MYLIB}/filesystem/fileApi.c
)

source_group("filesystem" FILES ${RZ_FILESYSTEMAPI})
message(status "** RZ_FILESYSTEMAPI : ${RZ_FILESYSTEMAPI}")
