#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <time.h>

size_t getFilesize(const char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

int main(int argc, char** argv) {
    size_t filesize = getFilesize(argv[1]);
    char *buf = malloc(4096);
    size_t i;
    assert(buf);

    //Open file
    int fd = open(argv[1], O_RDONLY, 0);
    assert(fd != -1);
    puts("opened");

    for (i = 0; i < filesize; i += 4096) {
	    int r = pread(fd, buf, 4096, i);
	    assert(r == 4096);
    }

    sleep(1 << 30);

    close(fd);
}
