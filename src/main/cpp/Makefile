CC=g++
CFLAGS=-g -Wall -shared -fPIC -std=c++11

SRC=multitreat
TARGET=libmultitreat

all: $(TARGET).so

$(TARGET).so: $(SRC).cpp
	$(CC) $(CFLAGS) -o $(TARGET).so $(SRC).cpp $(LIBS)

clean:
	$(RM) $(TARGET).so
