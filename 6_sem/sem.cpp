#include <semaphore.h>
#include <cassert>
#include <iostream>
#include "common.h"
#include "sem_manager.h"

using namespace std;

int main(int argc, char *argv[]) {
    key_t token = ipc_shared::generate_key("_tmp");
    ipc_shared::SemaphoreManager manager(token, 1);

    for (;;) {
        cout << "0 - Try access resource: ";
        string command;
        getline(cin, command);

        if (command.empty()) break;

        if (command == "0") {
            cout << "Waiting for resource..." << endl;
            manager.wait();
            cout << "Now we have access to the resource. Press ENTER to release. ";
            { string tmp; getline(cin, tmp); }
            manager.post();
            cout << "Resource released." << endl;
        }
        else {
            cout << "Invalid command '" << command << "'." << endl;
        }
    }
}
