#!/bin/bash

if (( $# < 2 ));
then
	echo "./downalias.sh interface count";
	echo " shutdown count alias on interface, count start from 1";
	exit 0;
fi

for (( i = 1; i <= ${2}; i++ ))
do
	ifconfig ${1}:${i} down
done
