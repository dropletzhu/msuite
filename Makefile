CC = gcc
RM = rm
TARGET = msender mlistener pim_sender pim_listener udpserver udpclient
SCRIPTS = msource.sh mgroup.sh mpim_rp.sh mpim_source.sh downalias.sh
DOCS = README	
VERSION = 0.3

CFLAGS = -fno-stack-protector -Wall

all: msender mlistener pim_sender pim_listener udpserver udpclient

msender:
	${CC} ${CFLAGS} msender.c -o msender

mlistener:
	${CC} ${CFLAGS} mlistener.c -o mlistener

pim_sender:
	${CC} ${CFLAGS} pim_sender.c -o pim_sender

pim_listener:
	${CC} ${CFLAGS} pim_listener.c -o pim_listener

udpserver:
	${CC} ${CFLAGS} udpserver.c -o udpserver

udpclient:
	${CC} ${CFLAGS} udpclient.c -o udpclient

tarball:
	tar czf msuite_${VERSION}.tgz ${TARGET} ${SCRIPTS} ${DOCS}

clean:
	${RM} -f *.o
	${RM} -f ${TARGET}
