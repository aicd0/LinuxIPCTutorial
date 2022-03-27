#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;

void sig_handler(int signum) {
    cout << "Signal handler called." << endl;
}

int main(int argc, char *argv[]) {
    struct sigaction new_action, old_action;
    new_action.sa_handler = sig_handler;
    new_action.sa_flags = 0;
    
    const int signum = SIGUSR1;
    sigaction(signum, &new_action, &old_action);
    
    pid_t pid = getpid();
    
    for (;;) {
        cout << "1 - raise, 2 - kill: ";
        string command;
        getline(cin, command);

        if (command.empty()) break;

        if (command == "1") {
            raise(signum);
        }
        else if (command == "2") {
            kill(pid, signum);
        }
        else {
            cout << "Invalid command '" << command << "'." << endl;
        }
    }
}
