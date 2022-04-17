#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include "buffer_reader.h"

using namespace std;

class Server {
  int m_fd_listen = -1;
  int m_fd_epoll = -1;
  vector<struct epoll_event> m_events; // epoll events
  unordered_map<int, string> m_msgs; // messages wait to be sent
  unordered_map<int, BufferReader> m_readers; // buffer readers
  char m_buf[MAX_BUFFER_SIZE];

public:
  Server() :
    m_events(16)
  {
    // Create a socket.
    // For details see: https://linux.die.net/man/2/socket
    m_fd_listen = socket(PF_INET, SOCK_CLOEXEC | SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    assert(m_fd_listen >= 0);

    // Get and set options on sockets.
    // For details see: https://linux.die.net/man/2/setsockopt
    {
      int reuse_addr = 1;
      assert(setsockopt(m_fd_listen, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) >= 0);
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

      assert(bind(m_fd_listen, (struct sockaddr*)&server_addr, sizeof(server_addr)) >= 0);
    }
  }

  void spin() {
    // Mark as a passive socket.
    // For details see: https://linux.die.net/man/2/listen
    assert(listen(m_fd_listen, SOMAXCONN) >= 0);

    // Creates an epoll instance.
    // For details see: https://linux.die.net/man/2/epoll_create1
    m_fd_epoll = epoll_create1(EPOLL_CLOEXEC);

    // Register the listen socket to the epoll instance.
    // For details see: https://linux.die.net/man/2/epoll_ctl
    {
      struct epoll_event event;
      event.data.fd = m_fd_listen;
      event.events = EPOLLIN;
      epoll_ctl(m_fd_epoll, EPOLL_CTL_ADD, m_fd_listen, &event);
    }

    cout << "Server listening..." << endl;

    for (;;) {
      int sockets_ready = epoll_wait(m_fd_epoll, &*m_events.begin(), static_cast<int>(m_events.size()), -1);

      if (sockets_ready < 0) {
        if (errno == EINTR) continue;
        assert(0);
      }

      if (sockets_ready == 0) continue;

      // Enlarge event buffer if necessary.
      if (static_cast<size_t>(sockets_ready) == m_events.size()) {
        m_events.resize(m_events.size() * 2);
      }

      for (int i = 0; i < sockets_ready; ++i) {
        struct epoll_event &event = m_events[i];

        if ((event.events & EPOLLERR) ||
          (event.events & EPOLLHUP))
        {
          close(event.data.fd);
          continue;
        }

        if (event.data.fd == m_fd_listen) {
          // Creates a new connected socket.
          // For details see: https://linux.die.net/man/2/accept4
          struct sockaddr_in peer_addr;
          socklen_t peer_len = sizeof(peer_addr);

          int fd_conn = accept4(m_fd_listen, (struct sockaddr*)&peer_addr,
            &peer_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
          assert(fd_conn >= 0);

          cout << "Connection established: IP=" << inet_ntoa(peer_addr.sin_addr) <<
            ", port=" << ntohs(peer_addr.sin_port) << "." << endl;
          
          // Register new socket for read/write.
          struct epoll_event new_event;
          new_event.data.fd = fd_conn;
          new_event.events = EPOLLIN;
          epoll_ctl(m_fd_epoll, EPOLL_CTL_ADD, fd_conn, &new_event);
        }
        else {
          int fd_conn = event.data.fd;
          if (fd_conn < 0) continue;
          
          if (event.events & EPOLLIN) // read event
          {
            // Read data.
            ssize_t size = read(fd_conn, m_buf, MAX_BUFFER_SIZE);

            if (size < 0) {
              if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available, try again later.
                continue;
              }
              assert(0);
            }

            if (size == 0) { // EOF
              close_connection(fd_conn);
              continue;
            }

            read_data(fd_conn, size);
          }
          else if (event.events & EPOLLOUT) // write event
          {
            write_data(fd_conn);
          }
        }
      }
    }
  }

private:
  void read_data(int fd, int size) {
    BufferReader& reader = m_readers[fd];
    reader.read(m_buf, size);

    if (reader.empty()) {
      return;
    }

    const vector<char>& complete_buf = reader.front();
    string msg(complete_buf.data(), complete_buf.size());
    reader.pop();
    cout << "Read: " << msg << endl;

    string reply = "<Reply begin>" + msg + "<Reply end>";
    m_msgs[fd] = reply;

    struct epoll_event mod_event;
    mod_event.data.fd = fd;
    mod_event.events = EPOLLOUT | EPOLLET;
    epoll_ctl(m_fd_epoll, EPOLL_CTL_MOD, fd, &mod_event);
  }

  void write_data(int fd) {
    auto msg_iter = m_msgs.find(fd);

    if (msg_iter != m_msgs.end()) {
      // Write data.
      const string &msg = msg_iter->second;
      ssize_t size = write_msg(fd, msg);

      if(size < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          return;
        }
        assert(0);
      }
    }
    
    struct epoll_event mod_event;
    mod_event.data.fd = fd;
    mod_event.events = EPOLLIN;
    epoll_ctl(m_fd_epoll, EPOLL_CTL_MOD, fd, &mod_event);
  }

  void close_connection(int fd) {
    cout << "Connection closed.";
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);
    if (getpeername(fd, (struct sockaddr*)&peer_addr, &addr_len) >= 0) {
      cout << " IP=" << inet_ntoa(peer_addr.sin_addr) <<
        ", port=" << ntohs(peer_addr.sin_port) << ".";
    }
    cout << endl;

    // Remove fd from epoll.
    assert(epoll_ctl(m_fd_epoll, EPOLL_CTL_DEL, fd, nullptr) >= 0);
    close(fd);

    // Remove fd from buffer.
    m_readers.erase(fd);
    m_msgs.erase(fd);
  }
};

int main(int argc, char *argv[]) {
  Server server;
  server.spin();
}
