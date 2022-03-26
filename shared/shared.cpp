#include <fcntl.h>
#include <unistd.h>
#include "shared.h"

namespace ipc_demo {
    key_t generate_key(const char *file_path) {
        int fd = open(file_path, O_RDWR | O_CREAT, 0666);
        if (fd == -1) return -1;
        close(fd);
        return ftok(file_path, 0);
    }
}
