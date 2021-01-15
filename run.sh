#!/bin/bash

docker() {
	if hash podman 2>/dev/null; then
        command podman "$@"
    else
        command docker "$@"
    fi
}

docker build -t webserver:latest .
docker run -it -p 8080:8080 -p 8081:8081 -p 9090:9090 -p 9091:9091 webserver:latest
