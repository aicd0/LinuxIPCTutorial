#include <sys/wait.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "utils.h"

using namespace std;

int fd[2]; // 0 - read, 1 - write

void new_proc() {
    int& fd_read = fd[0];
    int& fd_write = fd[1];

    for (;;) {
        cout << "Type a message: ";
        string msg;
        getline(cin, msg);
        write_msg(fd_read, fd_write, msg);
        if (msg.empty()) break;
        sleep(0.1);
    }

    cout << "Sender: connection closed." << endl;
}

void old_proc() {
    int& fd_read = fd[0];
    int& fd_write = fd[1];

    for (;;) {
        string msg;
        read_msg(fd_read, fd_write, msg);
        if (msg.empty()) break;
        cout << "Message received: " << msg << endl;
    }

    cout << "Receiver: connection closed." << endl;
}

int main()
{
    pipe(fd);
    pid_t pid = fork();

    if(pid == 0)
    {
        new_proc();
    }
    else if(pid > 0)
    {
        old_proc();
        int state;
        wait(&state);
    }
    else {
        cout << "[Error] Fork failed." << endl;
    }
}
