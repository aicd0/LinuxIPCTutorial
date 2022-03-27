#include <sys/msg.h>
#include <unistd.h>
#include <cstring>
#include "utils.h"

using namespace std;

struct msg_t
{
    long msgtype = 1;
    char data[MAX_MESSAGE_SIZE];

    inline static size_t data_size() { return sizeof(data); }
};

int send_message(int msqid, const string& content) {
    static msg_t msg;

    // Assemble message.
    size_t size = content.size() + 1;
    if (size > MAX_MESSAGE_SIZE) return -1;
    memcpy(msg.data, content.data(), size);

    // Send message.
    return msgsnd(msqid, &msg, msg_t::data_size(), 0);
}

int receive_message(int msqid, string& content) {
    static msg_t msg;
    
    // Read message.
    int res = msgrcv(msqid, &msg, msg_t::data_size(), 0, 0);
    if (res == -1) return -1;
    content.assign(msg.data);
    return res;
}
