#include "shared.h"
#include <unistd.h>

ssize_t write_msg(int fd, const std::string& msg) {
    ssize_t size = 0;

    // Write header.
    MessageHeader header = { msg.size() };
    {
        int ret = write(fd, &header, sizeof(MessageHeader));
        if (ret < 0) return ret;
        size += ret;
    }

    // Write data.
    {
        int ret = write(fd, msg.data(), header.size);
        if (ret < 0) return ret;
        size += ret;
    }

    return size;
}