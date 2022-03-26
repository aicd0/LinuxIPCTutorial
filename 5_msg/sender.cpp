#include <sys/msg.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "msg.h"
#include "shared.h"

using namespace std;

int main(int argc, char *argv[]) {
    key_t token = ipc_demo::generate_key("_tmp");
    int msqid = msgget(token, IPC_CREAT | 0666);
    if (msqid == -1) throw exception();
    msg_header_t header;

    for (;;) {
        cout << "Type a message: ";
        string text;
        getline(cin, text);
        
        // Assemble messages.
        size_t size = text.size() + 1;
        header.size = size;
        void* content_buf = malloc(sizeof(msg_content_t) + size);
        msg_content_t* content = new (content_buf)msg_content_t;
        memcpy(reinterpret_cast<char*>(content) + sizeof(msg_content_t), text.data(), size);

        // Send messages.
        msgsnd(msqid, &header, msg_header_t::msg_size(), 0);
        msgsnd(msqid, content, size, 0);
        
        free(content_buf);
        if (text.empty()) break;
    }

    msgctl(msqid, IPC_RMID, 0);
}
