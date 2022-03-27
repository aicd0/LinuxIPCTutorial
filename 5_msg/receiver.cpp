#include <sys/msg.h>
#include <iostream>
#include "utils.h"
#include "common.h"

using namespace std;

int main(int argc, char *argv[]) {
    key_t token = ipc_shared::generate_key("_tmp");
    int msqid = msgget(token, IPC_CREAT | 0666);
    if (msqid == -1) throw exception();

    for (;;) {
        string msg;
        receive_message(msqid, msg);
        if (msg.empty()) break;
        cout << "Message received: " << msg << endl;
    }
}
