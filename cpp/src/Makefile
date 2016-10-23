CC := g++
CFLAGS := -Wall -std=c++11
SHAREDCFLAGS := -shared -fPIC

LIBDIR := lib
BINDIR := bin
SRC := multitreat
EXE := $(BINDIR)/multitreat_testbed
SHARED_OBJ := $(LIBDIR)/libmultitreat.so
LIBS := -lmultitreat

all: $(SHARED_OBJ) $(EXE)

$(SHARED_OBJ): $(SRC).cpp
	mkdir -p $(LIBDIR)
	$(CC) $(CFLAGS) $(SHAREDCFLAGS) -o $(SHARED_OBJ) $(SRC).cpp

$(EXE): multitreat_testbed.cpp
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -L $(LIBDIR) -o $(EXE) multitreat_testbed.cpp $(LIBS)

clean:
	$(RM) $(LIBDIR)/*
	$(RM) $(BINDIR)/*
