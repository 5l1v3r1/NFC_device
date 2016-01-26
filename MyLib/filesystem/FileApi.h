#include "_base.h"

#ifdef __cplusplus
extern "C" {
#endif

// C header here
int FindExt(char *fileName, char *str);
int CheckSum(char *string);
int ReadFile(char *path, char** str);
int IsFile(char *fileName);
int MoveFile(char *destPath, char *sourcePath);
NfcBool createDirectory(const char* path, __mode_t mode);
#ifdef __cplusplus
}
#endif
