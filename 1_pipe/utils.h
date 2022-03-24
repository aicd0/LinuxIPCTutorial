#pragma once

#include <string>

ssize_t write_msg(int fd, std::string msg);
ssize_t read_msg(int fd, std::string& msg);
