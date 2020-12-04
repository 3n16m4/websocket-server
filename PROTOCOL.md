# Overview
Describes the basic protocol between the µC, this Server and the Frontend.

## Packet Types
```c++
// From client
typedef enum packet_type
{
    HANDSHAKE      = 0x00,
    PONG           = 0x01,
    WEATHER_STATUS = 0x02,
} packet_type_t;

// From server
typedef enum packet_type
{
    HANDSHAKE      = 0x00,
    HANDSHAKE_ACK  = 0x01,
    HANDSHAKE_NAK  = 0x02,
    PING           = 0x03,
    WEATHER_STATUS = 0x04,
} packet_type_t;
```

### Explanations

__Handshake__
Defines the custom µC handshake during the first connect. This is sent from the server right after establishing a TCP connection. The handshake is not required for the frontend, as the frontend will talk with WebSockets and perform their own handshake.

### µC (Client App)
------------------------
__Establishing a TCP connection__
1. TCP Connect to server with port 8081 (81 if using docker image).
2. You'll receive a handshake packet which you'll have to respond with.
3. After replying with the handshake packet, you'll get a handshake ack or nak packet.
4. Handle handshake ack packet (ACK, NAK).
5. ???
6. Profit.

__Handshake__
```c
// Handshake packet with payload (from client)
// Header: 0x00
// Size: 18 bytes
// Explanation:
// uuid: uniquely generated UUID that is known to the server. Each client will have exactly ONE UUID which is registered on the server.
// station: unique station id. Each client will have exactly ONE unique stationId. The server will not allow to have duplicate station ids from different µcs.
#pragma pack(push, 1)
typedef struct handshake_packet {
    uint8_t header;
    uint8_t uuid[16];
    uint8_t station_id; // station_id
} handshake_packet_t;

// can be further extended...
typedef enum station_id {
    GOE = 0,
    WF  = 1,
    BS  = 2,
} station_id_t;

// Pong packet. (from client)
// Explanation: Must be sent right after receiving a ping packet.
// Header: 0x01
// Size: 1 byte
typedef struct pong_packet {
    uint8_t header;
} pong_packet_t;

// Weather Status packet (from client)
// Explanation: The response to the weather status request packet from the server.
// Header: 0x02
// Size: 9 bytes
typedef struct weather_status_packet {
    uint8_t header;
    float temperature; // -40 - 80°C
    float humidity;    // 0 - 100%
} weather_status_packet_t;
#pragma pack(pop)
```

### Server
------------------------
```c
// Handshake packet with payload (from server)
// Header: 0x00
// Size: 1 byte
#pragma pack(push, 1)
typedef struct handshake_packet {
    uint8_t header;
} handshake_packet_t;

// Handshake Acknowledged Packet. (from server)
// Server has established connection with client.
// Header: 0x01
// Size: 1 byte
typedef struct handshake_ack_packet {
    uint8_t header;
} handshake_ack_packet_t;

// Handshake Not Acknowledged Packet. (from server)
// Server will close the remote connection.
// Explanation: The reason is described in the 'reason' field flag: example: 3, which means REASON_STATION_ID_ALREADY & REASON_UUID_FORMAT are set.
// Header: 0x02
// Size: 2 bytes
typedef struct handshake_nak_packet {
    uint8_t header;
    uint8_t reason; // handshake_reason
} handshake_nak_packet_t;

typedef enum handshake_reason {
    REASON_STATION_ID_ALREADY = 1, // StationId was already assigned on the server.
    REASON_UUID_FORMAT        = 2, // Wrong UUID format.
    REASON_UUID_ALREADY       = 4, // The UUID already exists.
} handshake_reason_t;

// Ping packet. (from server)
// Header: 0x03
// Size: 1 byte
typedef struct ping_packet {
    uint8_t header;
} ping_packet_t;

// from server --> Give me the weather data
// Header: 0x04
// Size: 1 byte
typedef struct weather_status_packet {
    uint8_t header;
} weather_status_packet_t;
#pragma pack(pop)
```

### Frontend
------------------------
The communication between the frontend and the server is realized through WebSockets and the HTTP Protocol with MessagePack as the underlying encoding / decoding process for the requests / responses.

References:
https://github.com/msgpack/msgpack-javascript
https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API

The key 'id' in the json request is related to the request / response type.
Currently, the following ids are specified:

Client to Server:
0 --> WeatherStatusRequest

Server to Client:
1 --> WeatherStatusResponse

> Sending a weather status request:
```json
{
    "id": 0,
    "stationId": 1
}
```

> Receiving a weather status response:
```json
{
    "id": 0,
    "stationId": 1,
    "temperature": 27.4,
    "humidity": 44.2,
    "time": "2021-01-01 14:03:55"
}
```
