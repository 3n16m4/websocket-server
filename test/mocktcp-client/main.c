#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#pragma pack(push, 1)
// in
typedef struct handshake_packet
{
    uint8_t header;
    uint8_t uuid[16];
    uint8_t station_id;
} handshake_packet_t;
typedef struct ping_packet
{
    uint8_t header;
} ping_packet_t;
typedef struct weather_status_request_packet
{
    uint8_t header;
    uint8_t uuid[16];
    uint8_t flag;
} weather_status_request_packet_t;

// out
typedef struct pong_packet
{
    uint8_t header;
} pong_packet_t;
typedef struct weather_status_response_packet
{
    uint8_t header;
    uint8_t uuid[16];
    float temperature; // -40 - 80°C
    float humidity;    // 0 - 100%
    uint8_t flag;
} weather_status_response_packet_t;
#pragma pack(pop)

int input_timeout(int filedes, unsigned int seconds)
{
    fd_set set;
    struct timeval timeout;

    /* Initialize the file descriptor set. */
    FD_ZERO(&set);
    FD_SET(filedes, &set);

    /* Initialize the timeout data structure. */
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    /* select returns 0 if timeout, 1 if input available, -1 if error. */
    return select(FD_SETSIZE, &set, NULL, NULL, &timeout);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "insufficient arguments.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    uint8_t const station_id = (uint8_t)strtol(argv[1], NULL, 10);

    char buffer[64];
    int sockfd;
    struct sockaddr_in server;
    struct hostent* hp = gethostbyname("127.0.0.1");
    if (hp == NULL) {
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(9090);

    bcopy(hp->h_addr, &server.sin_addr, hp->h_length);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sockfd, (const struct sockaddr*)&server,
                sizeof(struct sockaddr_in)) < 0) {
        return 1;
    }

    // mark the socket non-blocking
    int status = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    if (status == -1) {
        printf("calling fcntl\n");
        return 1;
    }

    printf("connected!\n");

    memset(buffer, 0, sizeof(buffer));

    while (1) {
        int result = input_timeout(sockfd, 1);
        // timeout
        if (result == 0) {
            printf("select() timeout.\n");
            continue;
        }

        ssize_t bytes = recv(sockfd, buffer, sizeof(buffer), 0);
        if (errno == EAGAIN) {
            printf("recv() would block.\n");
            continue;
        }

        uint8_t header = buffer[0] & 0xff;

        printf("received %zu bytes.\n", bytes);
        printf("packet header: %x\n", header);

        switch (header) {
            // handshake
        case 0x00: {
            printf("handshake received\n");
            handshake_packet_t packet = {
                .header = 0x00,
                .station_id = station_id,
                .uuid = {0xa8, 0x51, 0x17, 0x3e, 0x82, 0x64, 0x4b, 0x35, 0x80,
                         0xe2, 0x80, 0x01, 0x71, 0x12, 0xcc, 0x9d}};

            bytes = send(sockfd, (const void*)&packet, sizeof(packet), 0);
            printf("sent handshake %zu bytes.\n", bytes);
        } break;
            // ack
        case 0x01: {
            printf("handshake ack received\n");
        } break;
            // nak
        case 0x02: {
            printf("handshake nak received\n");
        } break;
            // ping
        case 0x03: {
            printf("ping received\n");

            pong_packet_t packet = {.header = 0x01};
            bytes = send(sockfd, (const void*)&packet, sizeof(packet), 0);
            printf("sent pong packet %zu bytes.\n", bytes);
        } break;
            // weather_status request
        case 0x04: {
            weather_status_request_packet_t p;
            memcpy(&p, buffer, sizeof(p));

            printf("weather_status request received: %d\n", p.flag);
            printf("weather_status UUID:\n");
            for (uint8_t i = 0; i < 16; ++i) {
                printf("%x ", p.uuid[i]);
            }
            printf("\n");

            // simulate a sensor read
            //sleep(1);
            usleep(100000);
            // send random data
            float const temp = ((float)rand() / (float)(RAND_MAX)) * 70.f;
            float const hum = ((float)rand() / (float)(RAND_MAX)) * 100.f;
            weather_status_response_packet_t response = {.header = 0x02,
                                                         .temperature = temp,
                                                         .humidity = hum,
                                                         .flag = p.flag};

            // copy original uuid to response
            memcpy(response.uuid, p.uuid, 16);

            bytes = send(sockfd, (const void*)&response, sizeof(response), 0);
            printf("sent weather status packet %zu bytes.\n", bytes);
        } break;
        }
    }

    sleep(100000);

    return 0;
}
