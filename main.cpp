#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>

#define NULL_CHAR '\0'
#define PORT 8080
#define BUFFER_SIZE 2048
#define QUEUE_SIZE 5
#define MAX_EVENTS 10

int create_server_socket(in_addr_t, in_port_t);
int create_epoll_socket(int);
void log_client(int, const char *);

class ChatService
{
private:
    std::unordered_set<int> clients_fd;

public:
    ChatService();
    void add_client(int);
    void remove_client(int);
    void host_broadcast(int, const char *);
    void send_message(int, const char *);
};

int main()
{
    int server_fd = create_server_socket(INADDR_ANY, PORT);
    ChatService chat_service = ChatService();
    // https://man7.org/linux/man-pages/man7/epoll.7.html
    int epoll_fd = create_epoll_socket(server_fd);

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];
    for (;;)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int n = 0; n < nfds; ++n)
        {
            if (events[n].data.fd == server_fd)
            {
                int client_fd = accept(server_fd, NULL, NULL);
                log_client(client_fd, "Connected");
                chat_service.add_client(client_fd);
                chat_service.send_message(client_fd, "[server] |> Welcome to the chat!\n");
                chat_service.host_broadcast(client_fd, "[server] |> New user joined the chat!\n");
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
            }
            else
            {
                int client_fd = events[n].data.fd;
                log_client(client_fd, "Sent message");
                int bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
                if (bytes_read == 0)
                {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    log_client(client_fd, "Disconnected");
                    chat_service.remove_client(client_fd);
                    close(client_fd);
                }
                else
                {
                    buffer[bytes_read] = NULL_CHAR;
                    chat_service.host_broadcast(client_fd, buffer);
                }
            }
        }
    }

    return 0;
}

int create_server_socket(in_addr_t host, in_port_t port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = host;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, QUEUE_SIZE);
    return server_fd;
}

int create_epoll_socket(int server_fd)
{
    int epoll_fd = epoll_create1(0);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
    return epoll_fd;
}

void log_client(int client_fd, const char *message)
{
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    getpeername(client_fd, (struct sockaddr *)&client_address, &client_address_len);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("Client [ %s:%d ] %s\n", client_ip, ntohs(client_address.sin_port), message);
}

ChatService::ChatService()
{
    clients_fd = std::unordered_set<int>();
}

void ChatService::add_client(int client_fd)
{
    clients_fd.insert(client_fd);
}

void ChatService::remove_client(int client_fd)
{
    clients_fd.erase(client_fd);
}

void ChatService::host_broadcast(int client_fd, const char *message)
{
    char client_message[BUFFER_SIZE];
    sprintf(client_message, "[client %d] |> %s", client_fd, message);
    for (int fd : clients_fd)
        if (fd != client_fd)
            send(fd, client_message, strlen(client_message), 0);
}

void ChatService::send_message(int client_fd, const char *message)
{
    send(client_fd, message, strlen(message), 0);
}
