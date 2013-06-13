#!/bin/bash

function usage() {
    printf "Usage: ./udploop.sh server_ip server_port connection_count\n"
    exit
}

if (( $# != 3 )) ;
then
    usage
fi

server_ip=$1
server_port=$2
connection_count=$3

#
# Loop to create $connection_count udp connections to $server_ip/$server_port
#
for ((i=1024; i<$connection_count+1024; i++))
do
    ./udpclient $server_ip ${i} $server_port &

    if (( ${i}&0xFF == 0)); then
	    sleep 2
        echo  "Sleep......\n"
    fi
done
