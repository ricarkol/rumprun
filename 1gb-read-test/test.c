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
    char *buf = malloc(filesize);
    assert(buf);

    //Open file
    int fd = open(argv[1], O_RDONLY, 0);
    assert(fd != -1);
    puts("opened");

    int r = read(fd, buf, filesize);
    assert(r == filesize);

    sleep(1 << 30);

    close(fd);
}
