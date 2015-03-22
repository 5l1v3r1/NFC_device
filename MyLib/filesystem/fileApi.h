#include "_base.h"

#ifdef __cplusplus
extern "C"
{
#endif

// C header here
int ReadFile( char *path, char** str);
int FindExt(char *fileName, char *str);
int IsFile(char *fileName);
int MoveFile(char *destPath, char *sourcePath);

#ifdef __cplusplus
}
#endif
