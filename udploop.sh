#!/bin/sh

#
# Loop to create 65535 sessions
# Udp server listen on 9999 
#
for ((i=1; i<65535; i++))
do
    ./udpclient 2.0.0.2 ${i} 9999
done
