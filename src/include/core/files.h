#ifndef __files_h__
#define __files_h__

int   mkpath(char *file_path, mode_t mode);
int   rmrf(const char *file_path);
char *read_file(const char *fname);

#endif
