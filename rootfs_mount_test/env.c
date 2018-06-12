#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h>

extern char **environ;
 
int list_dirs(char *path)
{
    struct dirent *de;  // Pointer for directory entry
 
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(path);
 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open directory" );
        return 0;
    }
 
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
            printf("%s\n", de->d_name);
 
    closedir(dr);    
    return 0;
}

int main(int argc , char *argv[])
{
	int i = 1;
	char *s = *environ;

	for (; s; i++) {
		printf("%s\n", s);
		s = *(environ+i);
	}

	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		fprintf(stdout, "Current working dir: %s\n", cwd);
	else
		perror("getcwd() error");

	list_dirs("/");
	list_dirs("/dev");

	return 0;
}
