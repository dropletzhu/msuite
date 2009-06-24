CC = gcc
RM = rm
TARGET = msender mlistener pim_sender pim_listener
SCRIPTS = msource.sh mgroup.sh mpim_rp.sh mpim_source.sh
DOCS = README	
VERSION = 0.2.1

CFLAGS = -fno-stack-protector

all: msender mlistener pim_sender pim_listener

msender:
	${CC} ${CFLAGS} msender.c -o msender

mlistener:
	${CC} ${CFLAGS} mlistener.c -o mlistener

pim_sender:
	${CC} ${CFLAGS} pim_sender.c -o pim_sender

pim_listener:
	${CC} ${CFLAGS} pim_listener.c -o pim_listener

tarball:
	tar czf msuite_${VERSION}.tgz ${TARGET} ${SCRIPTS} ${DOCS}

clean:
	${RM} -f *.o
	${RM} -f ${TARGET}
