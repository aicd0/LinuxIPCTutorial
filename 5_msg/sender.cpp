#include <sys/msg.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "utils.h"
#include "common.h"

using namespace std;

int main(int argc, char *argv[]) {
  key_t token = ipc_shared::generate_key("_tmp");
  int msqid = msgget(token, IPC_CREAT | 0666);
  if (msqid == -1) throw exception();

  for (;;) {
    cout << "Type a message: ";
    string msg;
    getline(cin, msg);
    int res = send_message(msqid, msg);
    if (res == -1) cout << "Failed to send message. Returns " << res << "." << endl;
    if (msg.empty()) break;
  }

  msgctl(msqid, IPC_RMID, 0);
}
