import WebSocket from 'ws';

const socket = new WebSocket('ws://localhost:8080');
const Message = "Hello, World!";

socket.onopen = (event) => {
    console.log("Connection established ");
    console.log("Sending the message: \'" + Message + "\'");
    socket.send(Message);
}

socket.on('ping', function heartbeat() {
    console.log("Ping received!");
});

socket.on('pong', function heartbeat() {
    console.log("Pong received!");
});

socket.onclose = (event) => {
    console.log('Connection closed by remote host.');
}

socket.onmessage = (event) => {
    console.log('Message from Server:', event.data);
}

socket.onerror = function (event) {
    console.error("WebSocket error observed:", event);
}
