CC = gcc
RM = rm
TARGET = msender mlistener pim_sender pim_listener udpserver udpclient msender6\
         mlistener6 tcpserver tcpclient
SCRIPTS = msource.sh mgroup.sh mpim_rp.sh mpim_source.sh downalias.sh
DOCS = README
VERSION = 0.4

CFLAGS = -fno-stack-protector -Wall

all: ${TARGET}

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

msender6:
	${CC} ${CFLAGS} msender6.c -o msender6

mlistener6:
	${CC} ${CFLAGS} mlistener6.c -o mlistener6

tcpserver:
	${CC} ${CFLAGS} tcpserver.c -o tcpserver

tcpclient:
	${CC} ${CFLAGS} tcpclient.c -o tcpclient

tarball:
	tar czf msuite_${VERSION}.tgz ${TARGET} ${SCRIPTS} ${DOCS}

clean:
	${RM} -f *.o
	${RM} -f ${TARGET}
