CC=g++
CFLAGS=-Wall -std=c++11
SHAREDCFLAGS=-shared -fPIC

SRC=multitreat
EXE=multitreat.a
TARGET=libmultitreat

all: $(TARGET).so $(EXE)

$(TARGET).so: $(SRC).cpp
	$(CC) $(CFLAGS) $(SHAREDCFLAGS) -o $(TARGET).so $(SRC).cpp $(LIBS)

$(EXE): $(SRC).cpp
	$(CC) $(CFLAGS) -o $(EXE) $(SRC).cpp $(LIBS)

clean:
	$(RM) $(TARGET).so $(EXE)
