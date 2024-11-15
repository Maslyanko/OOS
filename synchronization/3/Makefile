TARGET_1 = spinlock
SRCS_1 = src/main.c src/util/util.c src/storage/storage.c src/lock/spinlock/spinlock.c

TARGET_2 = mutex
SRCS_2 = src/main.c src/util/util.c src/storage/storage.c src/lock/mutex/mutex.c

TARGET_3 = rw_lock
SRCS_3 = src/main.c src/util/util.c src/storage/storage.c src/lock/rw_lock/rw_lock.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1} ${TARGET_2} ${TARGET_3}

${TARGET_1}: src/util/util.h src/storage/storage.h src/lock/lock.h ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

${TARGET_2}: src/util/util.h src/storage/storage.h src/lock/lock.h ${SRCS_2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}

${TARGET_3}: src/util/util.h src/storage/storage.h src/lock/lock.h ${SRCS_3}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_3} ${LIBS} -o ${TARGET_3}

clean:
	${RM} -f *.o ${TARGET_1} ${TARGET_2} ${TARGET_3}