#include <unistd.h>
#include "utils.h"

using namespace std;

ssize_t write_msg(int fd_read, int fd_write, string msg) {
    return write(fd_write, msg.data(), msg.size() + 1);
}

ssize_t read_msg(int fd_read, int fd_write, string& msg) {
    const int MAX_READ_BYTES = 10;
    static char buffer[MAX_READ_BYTES];
    ssize_t count = 0;

    for (;;) {
        ssize_t bytes = read(fd_read, buffer, MAX_READ_BYTES);
        if (bytes == -1) return -1;
        if (bytes <= 0) break;
        msg.reserve(msg.size() + bytes);
        msg.append(buffer, bytes);
        count += bytes;
        if (buffer[bytes - 1] == '\0') break;
    }

    msg.pop_back();
    return count;
}
