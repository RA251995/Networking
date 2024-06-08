#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

#define SERVER_PORT (2000U)
#define MAX_CLIENT_SUPPORTED (32U)

char data_buffer[1024];
int monitored_fd_set[MAX_CLIENT_SUPPORTED];

static void initialize_monitor_fd_set()
{
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        monitored_fd_set[i] = -1;
    }
}

static void add_to_monitored_fd_set(int sock_fd)
{
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (monitored_fd_set[i] == -1)
        {
            monitored_fd_set[i] = sock_fd;
            break;
        }
    }
}

static void remove_from_monitored_fd_set(int sock_fd)
{
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (monitored_fd_set[i] == sock_fd)
        {
            monitored_fd_set[i] = -1;
            break;
        }
    }
}

static void reinit_readfds(fd_set *fd_set_ptr)
{
    FD_ZERO(fd_set_ptr);
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (monitored_fd_set[i] != -1)
        {
            FD_SET(monitored_fd_set[i], fd_set_ptr);
        }
    }
}

static int get_max_fd()
{
    int max = -1;
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
    {
        if (monitored_fd_set[i] > max)
        {
            max = monitored_fd_set[i];
        }
    }

    return max;
}

static void setup_tcp_server_communication()
{
    /* 1. Initialize variables */
    int master_sock_tcp_fd = 0,
        sent_recv_bytes = 0,
        addr_len = 0;
    int comm_socket_fd;
    fd_set readfds;
    struct sockaddr_in server_addr, client_addr;

    initialize_monitor_fd_set();

    /* 2. Create Master socket */
    if ((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        printf("socket creation failed\n");
        exit(1);
    }

    /* 3. Bind */
    server_addr.sin_family = AF_INET; /* IPv4 */
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY; /* Any local interface address */

    addr_len = sizeof(struct sockaddr);

    if (bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, addr_len) == -1)
    {
        printf("socket bind failed\n");
        exit(1);
    }

    /* 4. Listen */
    if (listen(master_sock_tcp_fd, 5) == -1)
    {
        printf("listen failed\n");
        exit(1);
    }

    add_to_monitored_fd_set(master_sock_tcp_fd);

    while (1)
    {
        /* 5. Initialize and fill readfs */
        reinit_readfds(&readfds);

        /* 6. Select */
        printf("blocking on select system call...\n");
        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(master_sock_tcp_fd, &readfds))
        {
            printf("new connection received\n");

            /* 7. Accept the connection */
            comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
            if (comm_socket_fd == -1)
            {
                printf("accept failed\n");
                exit(1);
            }

            add_to_monitored_fd_set(comm_socket_fd);
            printf("Connection accepted from client %d %s:%u\n", comm_socket_fd,
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
        else
        {
            for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++)
            {
                if (FD_ISSET(monitored_fd_set[i], &readfds))
                {
                    comm_socket_fd = monitored_fd_set[i];

                    /* 8. Service the client requests */
                    sent_recv_bytes = recvfrom(comm_socket_fd, data_buffer, sizeof(data_buffer),
                                               0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
                    printf("server received %d byes from client %d\n", sent_recv_bytes, comm_socket_fd);

                    if (sent_recv_bytes == 0)
                    {
                        /* 9. Close the connection */
                        close(comm_socket_fd);
                        remove_from_monitored_fd_set(comm_socket_fd);
                        printf("server connection closed due to zero length data from client %d\n",
                               comm_socket_fd);
                        /* 10. Goto 5 */
                        break;
                    }

                    test_struct_t *client_data = (test_struct_t *)data_buffer;
                    if (client_data->a == 0 && client_data->b == 0)
                    {
                        /* 9. Close the connection */
                        close(comm_socket_fd);
                        remove_from_monitored_fd_set(comm_socket_fd);
                        printf("server connection closed due to zero data from client %d\n",
                               comm_socket_fd);
                        /* 10. Goto 5 */
                        break;
                    }

                    result_struct_t result;
                    result.c = client_data->a + client_data->b;

                    sent_recv_bytes = sendto(comm_socket_fd, &result, sizeof(result_struct_t), 0,
                                             (struct sockaddr *)&client_addr, addr_len);
                    printf("server sent %d bytes in reply to client %d\n",
                           sent_recv_bytes, comm_socket_fd);
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    setup_tcp_server_communication();
    return 0;
}
