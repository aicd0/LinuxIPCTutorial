#pragma once

#include <string>

ssize_t write_msg(int fd_read, int fd_write, std::string msg);
ssize_t read_msg(int fd_read, int fd_write, std::string& msg);
