#!/bin/sh

gcc webserver.c -o webserver
./webserver 8080 resources
