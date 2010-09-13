#!/bin/sh

#
# Loop to create 1000 sessions
# Tcp server listen on 9999 
#
for ((i=1; i<1000; i++))
do
    ./tcpclient 2.0.0.2 &
    ./udpclient 2.0.0.2 ${i} 9999 &
    ping 2.0.0.2 -c 1 &
done
