#!/bin/bash
#
# send three types of PIM packets consectively
#   PIM register, PIM null register, PIM register-stop
#

./pim_sender -r 3.0.0.100 -t 2 -s 4.0.0.100 -g 224.1.1.1 -p 5000 &
./pim_sender -r 3.0.0.100 -t 1 &
./pim_sender -r 3.0.0.100 -t 3 &
