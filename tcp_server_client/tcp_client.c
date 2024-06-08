#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"

#define DEST_PORT (2000U)
#define SERVER_IP_ADDRESS "127.0.0.1"

test_struct_t client_data;
result_struct_t result;

void setup_tcp_communication(void)
{
    /* 1. Initialize variables */
    int sockfd,
        sent_recv_bytes,
        addr_len = sizeof(struct sockaddr);
    struct sockaddr_in dest;

    /* 2. Speciy server credentials */
    dest.sin_family = AF_INET;
    dest.sin_port = DEST_PORT;
    inet_pton(AF_INET, SERVER_IP_ADDRESS, &dest.sin_addr);

    /* 3. Create a communication socket */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* 4. Connect with server (Send connection initiation request) */
    connect(sockfd, (struct sockaddr *)&dest, addr_len);

    while (1)
    {
        /* 5. Send data to server */
        printf("Enter a: ");
        scanf("%u", &client_data.a);
        printf("Enter b: ");
        scanf("%u", &client_data.b);

        sent_recv_bytes = sendto(sockfd, &client_data, sizeof(test_struct_t), 0, (struct sockaddr *)&dest, addr_len);
        printf("%d bytes sent\n", sent_recv_bytes);

        /* 6. Receive response */
        sent_recv_bytes = recvfrom(sockfd, &result, sizeof(result_struct_t), 0, (struct sockaddr *)&dest, (socklen_t *)&addr_len);
        printf("%d bytes received\n", sent_recv_bytes);
        printf("Result received: %u\n", result.c);

        /* 7. Close the connection if no more data needs to be sent to server */
        if (client_data.a == 0 && client_data.b == 0)
        {
            close(sockfd);
            break;
        }
    } /* 8. Go to 5 if more data needs to sent to server */
}

int main(int argc, char **argv)
{
    setup_tcp_communication();
    printf("application exit\n");
    return 0;
}
