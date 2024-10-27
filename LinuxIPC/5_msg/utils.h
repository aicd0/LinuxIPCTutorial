#pragma once

#include <string>

#define MAX_MESSAGE_SIZE 4096

int send_message(int msqid, const std::string& content);
int receive_message(int msqid, std::string& content);
