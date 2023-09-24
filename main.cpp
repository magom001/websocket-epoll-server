#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int MAX_EVENTS = 64;

inline int set_nonblocking(int fd)
{
    auto flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        perror("fcntl F_GETFL error\n");
        return -1;
    }

    auto result = fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    if (result < 0)
    {

        perror("fcntl F_SETFL error\n");
        return -1;
    }

    return 0;
}

int main(int, char **)
{
    auto ep_fd = epoll_create1(0);

    auto master_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    struct sockaddr_in sock_addr;

    const int enabled = 1;
    if (setsockopt(master_fd, SOL_SOCKET, SO_REUSEPORT, &enabled, sizeof(enabled)) < 0)
    {
        printf("setsockopt failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (master_fd < 0)
    {
        printf("Failed to create a server socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(8888);
    inet_aton("0.0.0.0", &sock_addr.sin_addr);

    if (bind(master_fd, reinterpret_cast<const sockaddr *>(&sock_addr), sizeof(sock_addr)) < 0)
    {
        printf("Failed to bind the server socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (listen(master_fd, 50) < 0)
    {
        printf("Failed to listen on the server socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Starting the loop...\n");
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = master_fd;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, master_fd, &event);

    for (;;)
    {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(ep_fd, events, MAX_EVENTS, -1);

        if (nfds < 0)
        {
            printf("epoll_wait failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
            {
                printf("Connections closed\n");
                close(events[i].data.fd);
                continue;
            }

            if (events[i].data.fd == master_fd)
            {
                while (true)
                {
                    struct sockaddr_in peer_addr;
                    socklen_t peer_addr_len;

                    auto cfd = accept(master_fd, reinterpret_cast<struct sockaddr *>(&peer_addr), &peer_addr_len);

                    if (cfd == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            printf("Processed all incoming events\n");
                            break;
                        }
                        else
                        {
                            perror("Error in accept\n");
                        }
                        break;
                    }

                    if (set_nonblocking(cfd) < 0)
                    {
                        close(cfd);
                        continue;
                    }

                    event.data.fd = cfd;
                    event.events = EPOLLIN | EPOLLET;
                    epoll_ctl(ep_fd, EPOLL_CTL_ADD, cfd, &event);
                }
            }
            else
            {
                std::vector<char> data;
                data.reserve(1024);
                while (true)
                {
                    ssize_t count;
                    char buf[1024];

                    count = read(events[i].data.fd, buf, sizeof(buf));

                    if (count == -1 && errno == EAGAIN)
                    {
                        printf("Got EAGAIN, we are good\n");
                        break;
                    }
                    else if (count == 0)
                    {
                        printf("Connection closed\n");
                        close(events[i].data.fd);
                        break;
                    }

                    data.insert(data.end(), buf, buf + count);
                }

                printf("Data received: %s\n", data.data());
            }
        }
    }

    return 0;
}
