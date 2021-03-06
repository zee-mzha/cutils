CC=clang
SRCS=src/*.c
INCLUDE=-Iinclude/
FLAGS=-O2 -fpic -c -D_FILE_OFFSET_BITS=64

.PHONY: .clean
.clean:
	rm out/*

cutils:
	${CC} ${FLAGS} ${SRCS} ${INCLUDE}
	mkdir out
	mv *.o out
	${CC} -shared -o libcutils.so out/*.o

cutilsDebug:
	${CC} ${FLAGS} -g ${SRCS} ${INCLUDE}
	mkdir out
	mv *.o out
	${CC} -shared -o libcutils.so out/*.o
