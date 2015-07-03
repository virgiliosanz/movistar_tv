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

/*
 * Reads an entire file to char * using mmap
 */
char *
read_file(const char *fname)
{
    FILE *fp   = NULL;
    char *data = NULL;

    error_if(!fname, error, "fname is NULL");

    fp = fopen(fname, "rb");
    error_if(!fp, error, "Error reading %s: %s", fname, strerror(errno));
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    data = malloc(fsize + 1);
    error_if(!data, error, "Not enought memory: %s", strerror(errno));

    fread(data, fsize, 1, fp);
    data[fsize] = 0;

    fclose(fp);

    return data;

error:
    if (fp) fclose(fp);
    if (data) free(data);
    return NULL;
}
