#!/bin/sh

#
# Loop to create 65535 sessions
# Destination address is 2.0.0.2, change it for your case
#
for ((i=1; i<65535; i++))
do
    ping 2.0.0.2 -c 1 &
    if ((i&0xFF == 0)); then
        sleep 1
        echo "Sleep......\n"
    fi
done
