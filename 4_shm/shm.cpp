#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

class SharedMemoryManager {
private:
    int m_shmid = 0;
    void *m_data = nullptr;
    size_t m_size = 0;

public:
    SharedMemoryManager(key_t key, size_t size) {
        m_shmid = shmget(key, size, IPC_CREAT | 0666);
        if (m_shmid == -1) throw exception();
        m_data = shmat(m_shmid, NULL, 0);
        m_size = size;
    }

    ~SharedMemoryManager() {
        if (m_data) {
            shmdt(m_data);
        }
        
        shmctl(m_shmid, IPC_RMID, NULL);
    }

    void *data() noexcept {
        return m_data;
    }

    size_t size() const noexcept {
        return m_size;
    }
};

void write_note(SharedMemoryManager& manager, const string& note) {
    if (note.size() >= manager.size()) {
        cout << "Write failed. Do not have enough memory." << endl;
        return;
    }

    char *data = reinterpret_cast<char*>(manager.data());
    memcpy(data, note.data(), note.size() + 1);
}

void read_note(SharedMemoryManager& manager, string& note) {
    const char* data = reinterpret_cast<char*>(manager.data());
    note.assign(data);
}

int main(int argc, char *argv[]) {
    // Generate IPC token.
    const char *tok_file_path = "_tmp";
    int fd = open(tok_file_path, O_RDWR | O_CREAT, 0666);
    close(fd);
    key_t token = ftok(tok_file_path, 0);

    auto manager = SharedMemoryManager(token, 4096); // 4kb fixed size
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
