#!/bin/bash
#
# Function:
#  - multiple multicast stream using one RP
#  - on multicast stream on multiple RP
#  - multiple PIM null register
#  - multiple PIM register-stop
#

if (( $# < 2 ));
then
	echo "./mpim_source.sh {unicast|multicast} rp count";
fi

count=1;

if [ ${1} == "unicast" ];
then
	echo "Create " ${3} "PIM register, unicast source 1.0.0.1 ~ 1.0.200.200" "RP " ${2};
	for (( i = 1; i < 200; i++ ))
	do
		for (( j = 1; i < 200; j++ ))
		do
			./pim_sender -i 1.0.${i}.${j} -rp ${2} -c 2 -s 1.0.0.201 -g 224.1.1.1 -p 5000
			(( count += 1 ));
			if (( ${count} > ${3} )); 
			then
				echo "Create " ${count} ${3}	" PIM register stream ";
				break;
			fi
		done

		if (( ${count} > ${3} ));
		then
			break;
		fi
	done
else
fi
