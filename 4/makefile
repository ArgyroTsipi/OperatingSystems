# File: makefile
# Author: Aggelos Kolaitis
# Date: 2022-05-23

######################################################
# SEMAPHORE shared library
sem.o: sem.c sem.h
	$(CC) -c sem.c -fPIC -o sem.o -Wall -Wextra
libsem.so: sem.o
	$(CC) sem.o -shared -o libsem.so -Wall -Wextra

######################################################
# SEMAPHORE library tests
tests: tests.c libsem.so
	$(CC) tests.c -L. -lsem -lpthread -o tests -Wall
test: tests
	@./run_test.sh

######################################################
# make clean
.PHONY = clean
clean:
	rm -rfv sem.o libsem.so tests
