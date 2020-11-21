#!/bin/bash

docker build -t webserver:latest .
docker run -it -p 80:8080 -p 81:8081 -p 90:9090 -p 91:9091 webserver:latest
