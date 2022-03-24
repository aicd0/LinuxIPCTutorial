#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "utils.h"

using namespace std;

int fd[2];
int& fd_read = fd[0];
int& fd_write = fd[1];

void sender_proc() {
    for (;;) {
        cout << "Type a message: ";
        string msg;
        getline(cin, msg);
        write_msg(fd_write, msg);
        if (msg.empty()) break;
        sleep(0.1);
    }

    cout << "Sender: connection closed." << endl;
    close(fd_read);
    close(fd_write);
}

void receiver_proc() {
    for (;;) {
        string msg;
        ssize_t result = read_msg(fd_read, msg);
        if (result <= 0) break;
        cout << "Message received: " << msg << endl;
    }

    cout << "Receiver: connection closed." << endl;
    close(fd_read);
    close(fd_write);
}

int main(int argc, char *argv[]) {
    pipe(fd);
    pid_t pid = fork();

    if(pid == 0)
    {
        // new process
        sender_proc();
    }
    else if(pid > 0)
    {
        // old process
        receiver_proc();
        wait(nullptr);
    }
    else {
        cout << "[Error] Fork failed." << endl;
    }
}
