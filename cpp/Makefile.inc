CC=g++
CFLAGS=-O3 -Wall -std=c++11
SHAREDCFLAGS=-shared -fPIC
LIBDIR=lib
BINDIR=bin
INCDIR=include
SHARED_OBJ=libmultitreat.so
LIBS=-lmultitreat

define cc-command
$(CC) $(CFLAGS)
endef
