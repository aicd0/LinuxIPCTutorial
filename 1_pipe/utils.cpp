#include <unistd.h>
#include "utils.h"

using namespace std;

ssize_t write_msg(int fd, string msg) {
    return write(fd, msg.data(), msg.size() + 1);
}

ssize_t read_msg(int fd, string& msg) {
    const int MAX_READ_BYTES = 100;
    static char buffer[MAX_READ_BYTES];

    for (;;) {
        ssize_t bytes = read(fd, buffer, MAX_READ_BYTES);
        if (bytes == -1) return -1;
        if (bytes == 0) return 0;
        msg.reserve(msg.size() + bytes);
        msg.append(buffer, bytes);
        if (buffer[bytes - 1] == '\0') break;
    }

    msg.pop_back();
    return msg.size();
}
