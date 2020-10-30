# Overview
Describes the basic protocol between the µC, this Server and the Frontend.

## Server
Accepts the following request types:

```c++
// From client
enum class RequestType : std::uint8_t
{
    Handshake = 0x00,
};
// From server
enum class ResponseType : std::uint8_t
{
    Handshake = 0x00,
};
```

### Explanations

__Handshake__
Defines the custom µC handshake during the first connect. This is sent from the µC and not from the server. It must be sent right after connecting to the server. The handshake is not required for the frontend, as the frontend will talk with WebSockets and perform their own handshake.

## Examples
The µC TCP-Client performs the following actions:

1. TCP Connect to Server
2. TCP Send 'Handshake' request packet
3. TCP Recv 'Handshake' response packet
4. Connection is either established or the server refused the connection