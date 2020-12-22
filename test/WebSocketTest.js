var client = new WebSocket();

client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(connection) {
    console.log('WebSocket Client Connected');
    connection.on('error', function(error) {
        console.log("Connection Error: " + error.toString());
    });
    connection.on('close', function() {
        console.log('echo-protocol Connection Closed');
    });
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log("Received: '" + message.utf8Data + "'");
        }
    });
    
    function sendNumber() {
        if (connection.connected) {
            const object = {
                id: 1
            };
            console.log(object);
            
            const encoded = encode(object);
            const buffer = Buffer.from(encoded.buffer, encoded.byteOffset, encoded.byteLength);

            connection.sendBytes(buffer);
            setTimeout(sendNumber, 1000);
            console.log("Data sent!");
        }
    }
    sendNumber();
});

client.connect('ws://localhost:8080/');
