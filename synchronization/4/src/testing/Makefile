TARGET_1 = spinlock
SRCS_1 = main.c util/util.c storage/storage.c lock/spinlock/spinlock.c ../spinlock/spinlock.c

TARGET_2 = mutex
SRCS_2 = main.c util/util.c storage/storage.c lock/mutex/mutex.c ../mutex/mutex.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1} ${TARGET_2}

${TARGET_1}: util/util.h storage/storage.h lock/lock.h ../spinlock/spinlock.h ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

${TARGET_2}: util/util.h storage/storage.h lock/lock.h ../mutex/mutex.h ${SRCS_2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}

clean:
	${RM} -f *.o ${TARGET_1} ${TARGET_2}