#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h>


int main(int argc , char *argv[])
{
	int l, n;
	char buf[80];
	int f = open("/test/aaaaaaaaaaaaaaax", O_RDONLY);
	while ((n=read(f,buf,80)) > 0)
		write(1,buf,n);
	close(f);

	f = open("/test/bla", O_CREAT|O_WRONLY);
	assert(f != -1);
	write(f, "bla\n", 4);
	close(f);

	f = open("/test/bla", O_RDONLY);
	while ((n=read(f,buf,80)) > 0)
		write(1,buf,n);
	close(f);

	f = open("/dev/random", O_RDONLY);
	assert(f != -1);
	n=read(f,buf,8);
	write(1,buf,n);
	close(f);

    printf("\nContent of /test\n");
    DIR *d;
    struct dirent *dir;
    d = opendir("/test");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }

	return 0;
}
