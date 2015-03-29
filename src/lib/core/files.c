#include <core/all.h>

/*
http://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix

By calling the function below you ensure that all dirs leading to the file path
specified exist. Note that file_path argument is not directory name here but rather
a path to a file that you are going to create after calling mkpath().

Eg., mkpath("/home/me/dir/subdir/file.dat", 0755) shall create /home/me/dir/subdir
if it does not exist. mkpath("/home/me/dir/subdir/", 0755) does the same.

Works with relative paths as well.

Returns -1 and sets errno in case of an error.
*/
int
mkpath(char *file_path, mode_t mode)
{
	error_if(!file_path, error, "file_path is NULL");

  	char *p;
  	for (p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/')) {
    		*p = '\0';
    		if (mkdir(file_path, mode) == -1) {
      			if (errno != EEXIST) {
				*p = '/';
				error("Directory already exists");
				goto error;
			}
    		}
    		*p = '/';
  	}

  	return 0;

error:
	return -1;
}
