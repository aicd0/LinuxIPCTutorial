#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include "shared.h"

using namespace std;

int start_client() {
    // Create a socket.
    int fd_conn = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(fd_conn >= 0);

    // Initialize server address.
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6666);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Open a connection on socket.
    // For details see: https://linux.die.net/man/3/connect
    if (connect(fd_conn, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cout << "Failed to establish connection to server." << endl;
        return -1;
    }

    // Retrieve the locally-bound name of the specified socket.
    // For details see: https://linux.die.net/man/3/getsockname
	struct sockaddr_in local_addr;
	socklen_t addr_len = sizeof(local_addr);
	assert(getsockname(fd_conn, (struct sockaddr*)&local_addr, &addr_len) >= 0);

    cout << "Connection established: IP=" << inet_ntoa(local_addr.sin_addr) <<
		", port=" << ntohs(local_addr.sin_port) << "." << endl;

	char buf[MAX_BUFFER_SIZE] = {0};

	for (;;)
	{
        cout << "Write: ";
        string msg_send;
        getline(cin, msg_send);
        if (msg_send.empty()) break;

        if (msg_send.size() + 1 > MAX_BUFFER_SIZE) {
            cout << "Write failed. Message too long." << endl;
            continue;
        }

        // Write data.
		write(fd_conn, msg_send.data(), msg_send.size() + 1);

        // Read data.
		read(fd_conn, buf, MAX_BUFFER_SIZE);
        cout << "Read: " << buf << endl;
	}

	close(fd_conn);
    return 0;
}

int main(int argc, char *argv[])
{
    start_client();
}
