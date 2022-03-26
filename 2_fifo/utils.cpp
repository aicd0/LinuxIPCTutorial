#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "utils.h"

using namespace std;

const char *fifo_path = "_tmp";

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

void start_sender() {
    mkfifo(fifo_path, 0666);
    int fd = open(fifo_path, O_WRONLY); // block until a receiver starts

    for (;;) {
        cout << "Type a message: ";
        string msg;
        getline(cin, msg);
        write_msg(fd, msg);
        if (msg.empty()) break;
    }

    cout << "Sender: connection closed." << endl;
    close(fd);
}

void start_receiver() {
    mkfifo(fifo_path, 0666);
    int fd = open(fifo_path, O_RDONLY); // block until a sender starts

    for (;;) {
        string msg;
        ssize_t result = read_msg(fd, msg);
        if (result <= 0) break;
        cout << "Message received: " << msg << endl;
    }

    cout << "Receiver: connection closed." << endl;
    close(fd);
}
