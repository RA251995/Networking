#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT (2000U)

typedef struct
{
    char name[32];
    char org[32];
    unsigned int year;
} course_t;

#define N_COURSES (5U)

course_t courses[N_COURSES] = {
    {"LDD", "FastBit", 2024},
    {"DSP", "EPFL", 2023},
    {"Cryptography", "UCB", 2022},
    {"Matplotlib", "Udemy", 2021},
    {"Python", "HackerRank", 2020}};

char data_buffer[1024];
char header[1024];
char html[1024];
char response[2048];

char *process_GET_request(char *url, unsigned int *response_len)
{
    /* URL format: /courses.html?name=<COURSE_NAME>&org=<ORG_NAME> */
    char *token[4];
    token[0] = strtok(url, "?");      /* /courses.html */
    token[1] = strtok(NULL, "?");     /* name=<COURSE_NAME>&org=<ORG_NAME> */
    token[2] = strtok(token[1], "&"); /* name=<COURSE_NAME> */
    token[3] = strtok(NULL, "&");     /* org=<ORG_NAME> */

    char *name_key = strtok(token[2], "="); /* name */
    char *name_value = strtok(NULL, "=");   /* <COURSE_NAME> */
    char *org_key = strtok(token[3], "=");  /* org */
    char *org_value = strtok(NULL, "=");    /* <ORG_NAME> */

    (void)org_key;
    (void)name_key;

    if (name_value == NULL || org_value == NULL)
    {
        *response_len = 0;
        return NULL;
    }

    int i = 0;
    for (; i < N_COURSES; i++)
    {
        if ((strcmp(name_value, courses[i].name) != 0) && strcmp(org_value, courses[i].org) != 0)
        {
            continue;
        }
        break;
    }

    if (i == N_COURSES)
    {
        *response_len = 0;
        return NULL;
    }

    snprintf(html, sizeof(html), "<html>"
                                 "<head><title>HTTP Server Response</title></head>"
                                 "<body><table>"
                                 "<thead><tr><th>Course</th><th>Issuing Organisation</th><th>Year</th></tr></thead>"
                                 "<tbody><tr><td>%s</td><td>%s</td><td>%d</td></tr></tbody>"
                                 "</table></body>"
                                 "</html>",
             courses[i].name, courses[i].org, courses[i].year);
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\n"
                                     "Server: My HTTP Server\n"
                                     "Connection: close\n"
                                     "Content-Length: %ld\n"
                                     "Content-Type: text/html; charset=UTF-8\n"
                                     "\n",
             strlen(html));
    sprintf(response, "%s%s", header, html);
    *response_len = strlen(response);

    return response;
}

void setup_tcp_server_communication()
{
    /* 1. Initialize variables */
    int master_sock_tcp_fd, sent_recv_bytes, addr_len, comm_socket_fd;
    int opt = 1;
    fd_set readfds;
    struct sockaddr_in server_addr, client_addr;

    /* 2. Create Master socket */
    if ((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        printf("== socket creation failed\n");
        exit(1);
    }

    if (setsockopt(master_sock_tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        printf("== socket setting failing\n");
        exit(1);
    }

    /* 3. Bind */
    server_addr.sin_family = AF_INET; /* IPv4 */
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; /* Any local interface address */

    addr_len = sizeof(struct sockaddr);

    if (bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, addr_len) == -1)
    {
        printf("== socket bind failed\n");
        exit(1);
    }

    /* 4. Listen */
    if (listen(master_sock_tcp_fd, 5) == -1)
    {
        printf("== listen failed\n");
        exit(1);
    }

    /* 5. Initialize and fill readfs */
    FD_ZERO(&readfds);
    FD_SET(master_sock_tcp_fd, &readfds);

    while (1)
    {
        /* 6. Select */
        printf("== blocking on select system call...\n");
        select(master_sock_tcp_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(master_sock_tcp_fd, &readfds))
        {
            printf("== new connection received\n");

            /* 7. Accept the connection */
            comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
            if (comm_socket_fd == -1)
            {
                printf("== accept failed\n");
                exit(1);
            }
            printf("== Connection accepted from client %s:%u\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            while (1)
            {
                printf("== server ready to service client msgs\n");

                memset(data_buffer, 0, sizeof(data_buffer));

                /* 8. Service the client requests */
                sent_recv_bytes = recvfrom(comm_socket_fd, data_buffer, sizeof(data_buffer),
                                           0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
                printf("== server received %d byes from client %s:%u\n", sent_recv_bytes,
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                if (sent_recv_bytes == 0)
                {
                    /* 9. Close the connection */
                    close(comm_socket_fd);
                    printf("== server connection closed due to zero length data from client %s:%u\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    /* 10. Goto 5 */
                    break;
                }

                printf("== Message received: %s", data_buffer);
                char *request_line, *method, *url;
                request_line = strtok(data_buffer, "\n");
                method = strtok(request_line, " ");
                url = strtok(NULL, " ");

                printf("== Method: %s\n", method);
                printf("== URL: %s\n", url);

                char *response = NULL;
                unsigned int response_len = 0;

                if (strncmp(method, "GET", 3) == 0)
                {
                    response = process_GET_request(url, &response_len);
                }
                else if (strncmp(method, "POST", 4) == 0)
                {
                    // response = process_POST_request(url, &response_len);
                }
                else
                {
                    printf("== Unsupported URL method\n");
                    close(comm_socket_fd);
                    break;
                }

                if (response)
                {
                    printf("== Response to be sent to client: %s\n", response);
                    sent_recv_bytes = sendto(comm_socket_fd, response, response_len, 0,
                                             (struct sockaddr *)&client_addr, addr_len);
                    printf("== server sent %d bytes in reply to client %s:%u\n", sent_recv_bytes,
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
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
