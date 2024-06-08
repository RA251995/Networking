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

char data_buffer[1024];

void setup_tcp_server_communication()
{
    /* 1. Initialize variables */
    int master_sock_tcp_fd = 0,
        sent_recv_bytes = 0,
        addr_len = 0;
    int comm_socket_fd = 0;
    fd_set readfds;
    struct sockaddr_in server_addr, client_addr;

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

    /* 5. Initialize and fill readfs */
    FD_ZERO(&readfds);
    FD_SET(master_sock_tcp_fd, &readfds);

    while (1)
    {
        /* 6. Select */
        printf("blocking on select system call...\n");
        select(master_sock_tcp_fd + 1, &readfds, NULL, NULL, NULL);

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
            printf("Connection accepted from client %s:%u\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            while (1)
            {
                printf("server ready to service client msgs\n");

                memset(data_buffer, 0, sizeof(data_buffer));

                /* 8. Service the client requests */
                sent_recv_bytes = recvfrom(comm_socket_fd, data_buffer, sizeof(data_buffer),
                                           0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
                printf("server received %d byes from client %s:%u\n", sent_recv_bytes,
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                if (sent_recv_bytes == 0)
                {
                    /* 9. Close the connection */
                    close(comm_socket_fd);
                    printf("server connection closed due to zero length data from client %s:%u\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    /* 10. Goto 5 */
                    break;
                }

                test_struct_t *client_data = (test_struct_t *)data_buffer;
                if (client_data->a == 0 && client_data->b == 0)
                {
                    /* 9. Close the connection */
                    close(comm_socket_fd);
                    printf("server connection closed due to zero data from client %s:%u\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    /* 10. Goto 5 */
                    break;
                }

                result_struct_t result;
                result.c = client_data->a + client_data->b;

                sent_recv_bytes = sendto(comm_socket_fd, &result, sizeof(result_struct_t), 0,
                                         (struct sockaddr *)&client_addr, addr_len);
                printf("server sent %d bytes in reply to client %s:%u\n", sent_recv_bytes,
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
        }
    }
}

int main(int argc, char **argv)
{
    setup_tcp_server_communication();
    return 0;
}
