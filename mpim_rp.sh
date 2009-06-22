#!/bin/bash
#
# Function:
#  - listen on multiple address for PIM packet
#  - PIM register send to multiple RP address
#

if (( $# < 2 ));
then
	echo "./mpim_rp.sh {sender|listener} count";
	echo " count should be less than 200";
	exit 0
fi

if [ ${1} == "sender" ];
then
	echo "Create " ${2} "PIM register stream to 3.0.0.1 ~ 3.0.0."${2};
	for (( i = 1; i <= ${2}; i++ ))
	do
		./pim_sender -i 1.0.0.201 -t 2 -r 3.0.0.${i} -s 1.0.0.201 -g 224.1.1.1 -p 5000 -c 2
	done	
else
	for (( i = 1; i <= ${2}; i++ ))
	do
		# setup address
		ifconfig eth1:${i} 3.0.0.${i} netmask 255.0.0.0
	done
	for (( i = 1; i <= ${2}; i++ ))
	do
		# put it to backgroud
		./pim_listener -r 3.0.0.${i} &
	done
fi
