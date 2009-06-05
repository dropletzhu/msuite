CC = gcc
RM = rm
TARGET = msender mlistener pim_sender pim_listener

all: msender mlistener pim_sender pim_listener

msender:
	${CC} msender.c -o msender

mlistener:
	${CC} mlistener.c -o mlistener

pim_sender:
	${CC} pim_sender.c -o pim_sender

pim_listener:
	${CC} pim_listener.c -o pim_listener

clean:
	${RM} -f *.o
	${RM} -f ${TARGET}
