#include <sys/msg.h>
#include <iostream>
#include "msg.h"
#include "shared.h"

using namespace std;

int main(int argc, char *argv[]) {
    key_t token = ipc_demo::generate_key("_tmp");
    int msqid = msgget(token, IPC_CREAT | 0666);
    if (msqid == -1) throw exception();

    for (;;) {
        // Reader header.
        msg_header_t header;
        msgrcv(msqid, &header, msg_header_t::msg_size(), static_cast<long>(MessageType::Header), 0);

        // Read content.
        msg_content_t* content_buf = reinterpret_cast<msg_content_t*>(malloc(sizeof(msg_content_t) + header.size));
        msgrcv(msqid, content_buf, header.size, static_cast<long>(MessageType::Content), 0);

        string text = reinterpret_cast<char*>(content_buf) + sizeof(msg_content_t);
        free(content_buf);
        if (text.empty()) break;
        cout << "Message received: " << text << endl;
    }
}
