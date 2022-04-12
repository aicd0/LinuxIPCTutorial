#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include "buffer_reader.h"

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

    char buf[MAX_BUFFER_SIZE];
    BufferReader reader;

	for (;;)
	{
        cout << "Write: ";
        string msg_send;
        getline(cin, msg_send);
        if (msg_send.empty()) break;

        assert(write_msg(fd_conn, msg_send) >= 0);

        // Read data.
        while (reader.empty()) {
            ssize_t size = read(fd_conn, buf, MAX_BUFFER_SIZE);
            assert(size >= 0);
            reader.read(buf, size);
        }

        const vector<char>& complete_buf = reader.front();
        string msg_recv(complete_buf.data(), complete_buf.size());
        reader.pop();
        cout << "Read: " << msg_recv << endl;
	}

	close(fd_conn);
    return 0;
}

int main(int argc, char *argv[])
{
    start_client();
}
