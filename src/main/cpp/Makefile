CC=g++
CFLAGS=-g -Wall -shared -fPIC

SRC=multitreat
TARGET=libmultitreat

all: $(TARGET).so

$(TARGET).so: $(SRC).cpp
	$(CC) $(CFLAGS) -o $(TARGET).so $(SRC).cpp $(LIBS)

clean:
	$(RM) $(TARGET).so
