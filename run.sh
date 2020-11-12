#!/bin/sh

docker build -t webserver:latest .
docker run -it -p 80:8080 -p 81:8081 webserver:latest

