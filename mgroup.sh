#!/bin/bash
#
# Function:
#	- send to multiple multicast group
#   - listen on multiple multicast group
#

if (( $# < 2 )) ;
then
	echo "./msource.sh {sender|listener} source stream_count";
	exit 0;
fi

count=0;

if [ ${1} == "sender" ]; 
then
	echo "Create multicast sender to 224.1.1.1 ~ 224.1.200.200"

	for (( i = 1; i <= 200; i++ ))
	do
		for (( j = 1; j <= 200; j++ ))
		do
			./msender -s ${2} -g 224.1.${i}.${j} -p 5000 -c 2 -t 32
			(( count += 1 ));
			if (( ${count} >= ${3} ));
			then
				echo "Create " ${3} ${count} "sender stream";
				exit 0;
			fi
		done
	done
else
	echo "Create multicast listener on 224.1.1.1 ~ 224.1.200.200"

	for (( i = 1; i <= 200; i++ ))
	do
		for (( j = 1; j <= 200; j++ ))
		do
			# send to background
			./mlistener -s ${2} -g 224.1.${i}.${j} -p 5000 &
			(( count += 1 ));
			if (( ${count} >= ${3} ));
			then
				echo "Create " ${3} ${count} "listener stream"
				exit 0;
			fi
		done
	done
fi
