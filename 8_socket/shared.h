#pragma once

#include <sys/types.h>
#include <string>

const size_t MAX_BUFFER_SIZE = 4096;

struct MessageHeader {
    unsigned long size;
};

ssize_t write_msg(int fd, const std::string& msg);