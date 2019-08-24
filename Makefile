SRCS = src/str.c src/bytestream.c src/file.c src/cliOptions.c src/tcpClient.c src/tcpServer.c src/logger.c
INCLUDE = -Iinclude/
FLAGS = -O2 -fpic -c -D_FILE_OFFSET_BITS=64

.PHONY: .clean
.clean:
	rm out/*

cutils:
	cc ${FLAGS} ${SRCS} ${INCLUDE}
	mv *.o out
	cc -shared -o libcutils.so out/*.o

cutilsDebug:
	cc ${FLAGS} -g ${SRCS} ${INCLUDE}
	mv *.o out
	cc -shared -o libcutils.so out/*.o
