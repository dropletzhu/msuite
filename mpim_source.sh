#!/bin/bash
#
# Function:
#  - change PIM source
#  - change embedded multicast source
#  - change PIM source and embedded multicast source
#

if (( $# < 2 ));
then
	echo "./mpim_source.sh {unicast|multicast|both} rp count";
	exit 0;
fi

count=1;

if [ ${1} == "unicast" ];
then
	echo "Create " ${3} "PIM register, unicast source 1.0.0.1 ~ 1.0.200.200" "RP " ${2};
	for (( i = 0; i <= 200; i++ ))
	do
		for (( j = 1; i <= 200; j++ ))
		do
			# PIM register
			./pim_sender -i 1.0.${i}.${j} -t 2 -r ${2} -c 2 -s 1.0.0.201 -g 224.1.1.1 -p 5000;
			(( count += 1 ));
			if (( ${count} > ${3} )); 
			then
				echo "Create " ${3} " PIM register stream ";
				break;
			fi
		done

		if (( ${count} > ${3} ));
		then
			break;
		fi
	done
elif [ ${1} == "multicast" ];
then
	echo "Create " ${3} "PIM register, multicast source 1.0.0.1 ~ 1.0.200.200" "RP " ${2};
	for (( i = 0; i <= 200; i++ ))
	do
		for (( j = 1; j <= 200; j++ ))
		do
			# PIM register
			./pim_sender -i 1.0.0.201 -t 2 -r ${2} -c 2 -s 1.0.${i}.${j} -g 224.1.1.1 -p 5000;
			(( count += 1 ));
			if (( ${count} > ${3} ));
			then
				echo "Create " ${2} " PIM register stream ";
				break;
			fi
		done

		if (( ${count} > ${3} ));
		then
			break;
		fi
	done
else
	echo "Create " ${3} "PIM register, unicast source 1.0.0.1 ~ 1.0.200.200" "multicast source 1.0.0.1 ~ 1.0.200.200" "RP " ${2};
	for (( i = 0; i <= 200; i++ ))
	do
		for (( j = 1; j <= 200; j++ ))
		do
			./pim_sender -i 1.0.${i}.${j} -t 2 -r ${2} -c 2 -s 1.0.${i}.${j} -g 224.1.1.1 -p 5000;
			(( count += 1 ));
			if (( ${count} > ${3} ));
			then
				echo "Create " ${2} " PIM register stream ";
				break;
			fi
		done

		if (( ${count} > ${3} ));
		then
			break;
		fi
	done
fi
