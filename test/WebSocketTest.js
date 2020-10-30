import WebSocket from 'ws';

const socket = new WebSocket('ws://localhost:8080');

socket.onopen = (event) => {
    console.log("Connection established ", event.data);
    socket.send('Hello');
}

socket.onclose = (event) => {
    console.log('Connection closed by remote host.');
}

socket.onmessage = (event) => {
    console.log('Message from Server ', even.data);
}

socket.onerror = function (event) {
    console.error("WebSocket error observed:", event);
}
