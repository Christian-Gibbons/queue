CC=gcc
SRC := src
LIB := lib
OBJ := objects
INC := include
BIN := bin
CFLAGS= -std=c11 -I ./${INC}

${LIB}/queue.a: ${OBJ}/queue.o
	mkdir -p ${LIB}
	ar rcs $@ $^

${OBJ}/queue.o: ${SRC}/queue.c ${INC}/queue.h
	mkdir -p ${OBJ}
	${CC} -c ${CFLAGS} $< -o $@

all: ${BIN}/qtest

${BIN}/qtest: ${OBJ}/qtest.o ${OBJ}/queue.o
	mkdir -p ${BIN}
	${CC} -o $@ $^

${OBJ}/qtest.o: ${SRC}/qtest.c ${INC}/queue.h
	mkdir -p ${OBJ}
	${CC} -c ${CFLAGS} $< -o $@

.PHONY: clean all

clean:
	rm -rf ${OBJ} ${LIB} ${BIN}
