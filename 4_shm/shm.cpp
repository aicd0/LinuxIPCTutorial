#include <cstring>
#include <iostream>
#include <string>
#include "common.h"
#include "shm_manager.h"

using namespace std;

void write_note(ipc_shared::SharedMemoryManager& manager, const string& note) {
    if (note.size() >= manager.size()) {
        cout << "Write failed. Do not have enough memory." << endl;
        return;
    }

    char *data = reinterpret_cast<char*>(manager.data());
    memcpy(data, note.data(), note.size() + 1);
}

void read_note(ipc_shared::SharedMemoryManager& manager, string& note) {
    const char* data = reinterpret_cast<char*>(manager.data());
    note.assign(data);
}

int main(int argc, char *argv[]) {
    key_t token = ipc_shared::generate_key("_tmp");
    ipc_shared::SharedMemoryManager manager(token, 4096); // 4kb fixed size
    write_note(manager, ""); // initialize buffer

    for (;;) {
        cout << "0 - read, 1 - write: ";
        string selection;
        getline(cin, selection);

        if (selection.empty()) break;

        if (selection == "0") {
            string note;
            read_note(manager, note);
            cout << "Read: " << note << endl;
        }
        else if (selection == "1") {
            cout << "Write: ";
            string note;
            getline(cin, note);
            write_note(manager, note);
        }
    }
}
