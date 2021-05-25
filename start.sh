#!/bin/bash
printf "Compiling..."

printf "\n\n\n\nServer ...\n\n\n\n"

gcc server.c -o server -pthread

printf "\n\n\n\nClient ...\n\n\n\n"


gcc client.c -o client -pthread

