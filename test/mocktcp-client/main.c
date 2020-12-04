#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct handshake_packet
{
    uint8_t header;
    uint8_t uuid[16];
    uint8_t station_id;
} handshake_packet_t;
#pragma pack(pop)

int main()
{
    char buffer[64];
    int sockfd;
    struct sockaddr_in server;
    struct hostent* hp = gethostbyname("127.0.0.1");
    if (hp == NULL) {
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(9090);

    bcopy(hp->h_addr, &server.sin_addr, hp->h_length);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sockfd, (const struct sockaddr*)&server,
                sizeof(struct sockaddr_in)) < 0) {
        return -1;
    }

    printf("connected!\n");

    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recv(sockfd, buffer, sizeof(buffer), 0);
    printf("received %zu bytes.\n", bytes);
    printf("packet header: %x\n", buffer[0] & 0xff);

    handshake_packet_t packet = {.header = 0x00,
                                 .station_id = 0x00,
                                 .uuid = {0xa8, 0x51, 0x17, 0x3e, 0x82, 0x64,
                                          0x4b, 0x35, 0x80, 0xe2, 0x80, 0x01,
                                          0x71, 0x12, 0xcc, 0x9d}};

    bytes = send(sockfd, (const void*)&packet, sizeof(packet), 0);
    printf("sent %zu bytes.\n", bytes);

    bytes = recv(sockfd, buffer, sizeof(buffer), 0);
    printf("received %zu bytes.\n", bytes);
    printf("packet header: %x\n", buffer[0] & 0xff);

    return 0;
}
