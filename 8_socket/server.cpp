#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "shared.h"

using namespace std;

int start_server() {
    // Create a socket.
    // For details see: https://linux.die.net/man/2/socket
    int fd_listen = socket(PF_INET, SOCK_CLOEXEC | SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    assert(fd_listen >= 0);

    // Get and set options on sockets.
    // For details see: https://linux.die.net/man/2/setsockopt
    {
        int reuse_addr = 1;
        assert(setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) >= 0);
    }

    // Initialize server address and bind to the socket.
    // For details see:
    // https://linux.die.net/man/7/ip,
    // https://linux.die.net/man/2/bind
    {
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(struct sockaddr_in));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(6666);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        assert(bind(fd_listen, (struct sockaddr*)&server_addr, sizeof(server_addr)) >= 0);
    }

    // Mark as a passive socket.
    // For details see: https://linux.die.net/man/2/listen
    assert(listen(fd_listen, SOMAXCONN) >= 0);

    // Creates an epoll instance.
    // For details see: https://linux.die.net/man/2/epoll_create1
    int fd_epoll = epoll_create1(EPOLL_CLOEXEC);

    // Register the listen socket to the epoll instance.
    // For details see: https://linux.die.net/man/2/epoll_ctl
    {
        struct epoll_event event;
        event.data.fd = fd_listen;
        event.events = EPOLLIN;
        epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_listen, &event);
    }

    vector<struct epoll_event> events(16); // epoll events
    unordered_map<int, string> msgs; // messages to be sent to clients

    cout << "Server listening..." << endl;

    for (;;) {
        int sockets_ready = epoll_wait(fd_epoll, &*events.begin(), static_cast<int>(events.size()), -1);

        if (sockets_ready < 0) {
            if (errno == EINTR) continue;
            assert(0);
        }

        if (sockets_ready == 0) continue;

        // Enlarge event buffer if necessary.
        if (static_cast<size_t>(sockets_ready) == events.size()) {
            events.resize(events.size() * 2);
        }

        for (int i = 0; i < sockets_ready; ++i) {
            struct epoll_event &event = events[i];

            if ((event.events & EPOLLERR) ||
                (event.events & EPOLLHUP))
            {
                close(event.data.fd);
                continue;
            }

            if (event.data.fd == fd_listen) {
                // Creates a new connected socket.
                // For details see: https://linux.die.net/man/2/accept4
                struct sockaddr_in peer_addr;
                socklen_t peer_len = sizeof(peer_addr);

                int fd_conn = accept4(fd_listen, (struct sockaddr*)&peer_addr,
                    &peer_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
                assert(fd_conn >= 0);

                cout << "Connection established: IP=" << inet_ntoa(peer_addr.sin_addr) <<
                    ", port=" << ntohs(peer_addr.sin_port) << "." << endl;
                
                // Register new socket for read/write.
                struct epoll_event new_event;
                new_event.data.fd = fd_conn;
                new_event.events = EPOLLIN;
                epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_conn, &new_event);
            }
            else {
                int fd_conn = event.data.fd;
                if (fd_conn < 0) continue;
                
                if (event.events & EPOLLIN) // read event
                {
                    // Read data.
                    char buf[MAX_BUFFER_SIZE] = {0};
                    int ret = read(fd_conn, buf, MAX_BUFFER_SIZE);

                    if (ret < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            // No data available, try again later.
                            continue;
                        }
                        assert(0);
                    }

                    if (ret == 0) { // EOF
                        cout << "Connection closed.";

                        struct sockaddr_in peer_addr;
                        socklen_t addr_len = sizeof(peer_addr);
                        if (getpeername(fd_conn, (struct sockaddr*)&peer_addr, &addr_len) >= 0) {
                            cout << " IP=" << inet_ntoa(peer_addr.sin_addr) <<
                                ", port=" << ntohs(peer_addr.sin_port) << ".";
                        }

                        cout << endl;
                        close(fd_conn);
                        epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd_conn, &event);
                        continue;
                    }

                    cout << "Read: " << buf << endl;
                    
                    string reply(buf);
                    reply = "<Server begin>" + reply + "<Server end>";
                    msgs[fd_conn] = reply;

                    struct epoll_event mod_event;
                    mod_event.data.fd = fd_conn;
                    mod_event.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd_conn, &mod_event);
                }
                else if (event.events & EPOLLOUT) // write event
                {
                    auto msg_iter = msgs.find(fd_conn);

                    if (msg_iter != msgs.end()) {
                        const string &msg = msg_iter->second;

                        if (msg.size() + 1 <= MAX_BUFFER_SIZE) {
                            // Write data.
                            int ret = write(fd_conn, msg.data(), msg.size() + 1);

                            if(ret == -1) {
                                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                    continue;
                                }
                                assert(0);
                            }
                        }

                        msgs.erase(msg_iter);
                    }
                    
                    struct epoll_event mod_event;
                    mod_event.data.fd = fd_conn;
                    mod_event.events = EPOLLIN;
                    epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd_conn, &mod_event);
                }
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    start_server();
}
