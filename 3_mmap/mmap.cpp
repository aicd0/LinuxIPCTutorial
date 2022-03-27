#include <cstring>
#include <iostream>
#include <string>
#include "utils.h"

using namespace std;

void write_note(MMapManager& manager, const string& note) {
    int ret = manager.reserve(note.size() + 1);
    if (ret < 0) {
        cout << "Write failed." << endl;
        return;
    }

    char *data = reinterpret_cast<char*>(manager.data());
    memcpy(data, note.data(), note.size() + 1);
}

void read_note(MMapManager& manager, string& note) {
    const char* data = reinterpret_cast<char*>(manager.data());
    note.assign(data);
}

int main(int argc, char *argv[]) {
    auto manager = MMapManager("_tmp");
    write_note(manager, ""); // initialize buffer.

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
