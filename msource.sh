#!/bin/bash
#
# Function:
#  - setup multiple source address
#  - send multicast traffic from multiple source address
#  - verify mp2p sessions on gateway
#

if (( $# < 2 )) ;
then
	echo "./msource.sh group_address stream_count";
	exit 0;
fi

echo "Create " ${2} " multicast streams to " ${1} " source ip 1.0.0.1 ~ 1.0.200.200"

count=1;

for (( i = 0; i <= 200; i++ ))
do
	for (( j = 1; i <= 200; j++ ))
	do
		ifconfig eth1:${count} 1.0.${i}.${j} netmask 255.0.0.0
		(( count += 1 ));
		if (( ${count} > ${2} ));
		then
			echo "Create " ${2} "alias interface";
			break;
		fi
	done

	if (( ${count} > ${2} ));
	then
		break;
	fi
done

count=1;

for (( i = 0; i <= 200; i++ ))
do
	for (( j = 1; j <= 200; j++ ))
	do
		./msender -s 1.0.${i}.${j} -g ${1} -p 5000 -c 2 -t 32
		(( count += 1 ));
		if (( ${count} > ${2} ));
		then
			echo "Create " ${2} " sender stream";
			break;
		fi
	done

	if (( ${count} > ${2} ));
	then
		break;
	fi
done
